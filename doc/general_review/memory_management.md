# What is Memory Management in the Kernel?
The kernel manages memory through a complex system that includes:
## Physical & Virtual Memory Mapping:
 
   - In OS like Linux, processes do not directly access physical memory (`RAM`). Instead, the kernel provides each process with a virtual address space, and the Memory Management Unit (`MMU`) translates virtual addressees memory locations:
   - Physical memory is the actual `RAM` on installed on the system,
   - Virtual Memory is an abstraction provided by the OS, each process gets its own isolated memory space.

**Why Virtual Memory**:
- Process isolation: each proxies gets a private address space to prevent accidental corruption.
- Efficient Memory Utilization: The kernel loads only needed memory pages.
- Address Space Expansion Programs can use memory than physical available by swapping memory to disk.
- Security Prevents processes from accessing each other's memory.

**How Virtual Memory Works: Address Translation**
- The CPU does not directly access `RAM`.
- Instead, it sends virtual addresses to the `MMU`, which translates into physical addresses.
- The `page` table maps virtual addresses to physical addresses.
```bash
cat /proc/self/maps  # See the virtual memory layout of a process.
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
# ffffffffff600000-ffffffffff601000: start and end addresses of the mapped memory region
# --xp: memory permission, here it mean: Not readable, not writable, but executable, Private mapping (not shared with other processes).
# 00000000 is offset in the mapped file, and here it means the memory starts from the beginning
# 00:00: Device, here, this is not backed by an actual file (a virtual memory)
# 0: Inode, here it means no associated file on disk.
# [vsyscall]: is alegacy virtual system call region. For fast system calls.
```

## Kernel vs. User-Space Memory:

Linux uses a virtual memory system, i.e.,:

-   User-space applications run in an isolated address space, and cannot directly access kernel memory
-   The kernel runs in privileged mode and can access all memory.

-   This prevents user-space programs from corrupting kernel memory.
-   The kernel must map user-space memory when needed.

## Pages & `Page` Tables in Linux Kernel Memory Management

### What is Page?
A `page` is the smallest unit of memory management in the modern operating systems of handling memory byte-by-byte, Linux breaks it into fixed-sized blocks called pages.

**`Page` Basics**

- `Page` size is `4 KB` (on x86-64 systems).
- Divides memory into fixed chunks instead of variable-sized regions.
- Simplifies memory allocation and swapping.
- Each process gets its own set of pages.
In terminal:
```bash
getconf PAGESIZE
4096
```
also:
```bash
cat /proc/meminfo | grep Page
AnonPages:       1697000 kB
PageTables:        32956 kB
SecPageTables:         0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
FileHugePages:         0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
```

The following are important page-related memory metrics from `/proc/meminfo`. These help understand how Linux manages memory.

| **Field**             | **Meaning**                                  | **Example Value** |
|----------------------|--------------------------------|----------------|
| **`AnonPages`**       | Memory pages **not backed by files** (e.g., `malloc()` allocations) | `1697000 kB` (1.6 GB) |
| **`PageTables`**      | Memory used by **`page` tables** to manage virtual-to-physical mappings | `32956 kB` (~32 MB) |
| **`SecPageTables`**   | Security-related secondary `page` tables (used in some architectures) | `0 kB` |
| **`AnonHugePages`**   | Anonymous pages mapped via **Transparent Huge Pages (THP)** | `0 kB` (THP disabled or unused) |
| **`ShmemHugePages`**  | Huge pages allocated for **shared memory segments** (`shmget()`) | `0 kB` |
| **`FileHugePages`**   | Huge pages allocated for **memory-mapped files** (`mmap()`) | `0 kB` |
| **`HugePages_Total`** | Total **preallocated Huge Pages** available in the system | `0` (not configured) |
| **`HugePages_Free`**  | Number of free huge pages | `0` |
| **`HugePages_Rsvd`**  | Huge pages **reserved** for future allocations | `0` |
| **`HugePages_Surp`**  | Extra huge pages dynamically allocated beyond `HugePages_Total` | `0` |

---
What is what:
- **Regular paging is used** (`AnonPages` is high).
- **`Page` tables consume ~32 MB**, which is normal.
- **Huge Pages (`HugePages_Total`) are not enabled.**
- **Transparent Huge Pages (THP) is either disabled or unused.**

### What is `Page` Table:
Since the OS uses virtual memory, each process does nit directly access physical memory. instead, the `MMU` translates the virtual memory addresses into physical memory addresses using `page` tables.
-   One can compare to a phone book:
-   Virtual addresses are like people's names,
-   Physical addresses are like phone numbers.
-   The `page` table acts as the lookup table between them
* A process accesses a virtual address
* The MMU looks up the `page` table to find the corresponding physical address.
* The CPU accesses the correct memory location

**How `Page` Tables Work:**
Because modern systems have huge amounts of `RAM`, a single-level `page` table would be too large. Instead, Linux uses a multi-level `page` table to keep it efficient.
- `PGD` (`Page` Global Directory) -> Top-level table pointing to `P4D` -> 512 (Entries per table)
- `P4D` (Page 4th Level Directory) -> Points to `PUD` -> 512
- `PUD` (Page Upper Directory) -> Points to `PMD` -> 512
- `PMD` (Page Middle Directory) -> Points to `PTE` -> 512
- `PTE` (Page Table Entry) -> Maps virtual address to physical address -> 512
    
`PTE` entry contains the `page` frame number (`PFN`).
Offset within the `page` is added to find the exact byte.

### *Page Faults* (When a Page is Not Found):
If a process accesses a virtual address not in RAM, the MMU raises a "`Page Fault`".

Invalid Address → Segmentation Fault (`SIGSEGV`).

## Paging vs. Segmentation

Paging and segmentation are two different memory management techniques. Modern Linux systems use **paging**, not segmentation.

| **Feature**        | **Paging**                                | **Segmentation**                        |
|--------------------|---------------------------------|---------------------------------|
| **Memory Model**   | Fixed-size pages                | Variable-sized segments        |
| **Fragmentation**  | Solves external fragmentation  | Causes fragmentation           |
| **Speed**         | Faster lookup via `page` tables  | Slower due to variable sizes  |
| **Flexibility**    | Less flexible, fixed-size chunks | More flexible, variable-sized chunks |

### **Why Linux Uses Paging, Not Segmentation**
- Paging **avoids external fragmentation** by using fixed-size pages.
- It allows for **efficient virtual memory management**.
- Modern **CPUs are optimized for paging**, making it faster.
- Segmentation was **used in older architectures** (e.g., x86 real mode).

## How the Kernel Allocates Pages

The Linux kernel provides several functions for **memory allocation**, depending on the use case.

| **Function**         | **Description**                               | **Use Case**                          |
|----------------------|---------------------------------------------|--------------------------------------|
| `kmalloc(size, flags)`  | Allocates contiguous memory in physical RAM | Small kernel allocations (<1 MB) |
| `vmalloc(size)`      | Allocates large non-contiguous memory        | Large kernel memory (>1 MB) |
| `alloc_pages(flags, order)` | Allocates memory at `page` level (2^order pages) | Large contiguous allocations |
| `mmap()`            | Maps files/devices into user-space memory    | Shared memory, file mapping |
| `remap_pfn_range()` | Maps physical memory to user-space           | Device drivers, hardware mapping |
| `get_free_page(flags)` | Allocates a single 4 KB `page`               | Low-level kernel memory |

### 🔹 **Key Differences**
- **`kmalloc()`** is **fast** but **only works for small allocations** because it requires contiguous physical memory.
- **`vmalloc()`** works for **large allocations** by mapping non-contiguous memory into a contiguous virtual space.
- **`alloc_pages()`** is used for **direct page-level allocations** (useful for multi-page kernel buffers).
- **`mmap()`** allows mapping **files or hardware memory into user space**.


