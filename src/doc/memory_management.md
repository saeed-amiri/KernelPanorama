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
- The page table maps virtual addresses to physical addresses.
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

## Pages & Page Tables in Linux Kernel Memory Management

### What is Page?
A page is the smallest unit of memory management in the modern operating systems of handling memory byte-by-byte, Linux breaks it into fixed-sized blocks called pages.

**Page Basics**

- Page size is 4 KB (on x86-64 systems).
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
```

### What is Page Table:
Since the OS uses virtual memory, each process does nit directly access physical memory. instead, the `MMU` translates the virtual memory addresses into physical memory addresses using page tables.
-   One can compare to a phone book:
-   virtual addresses are like people's names,
-   physical assresses are like phone numbers.
-   The page table acts as the lookup table between them
* A process accesses a virtual address
* The MMU looks up the page table to find the corresponding physical address.
* The CPU accesses the correct memory location
