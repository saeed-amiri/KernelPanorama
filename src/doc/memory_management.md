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