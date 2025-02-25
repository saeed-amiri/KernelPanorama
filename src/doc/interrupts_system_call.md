# Interrupts and System Calls
The core of how the Linux kernel interacts with hardware and user processes.

## What Are Interrupts?
Interrupts are signals that tell the CPU to stop what it is doing and handle something urgent/

They allow the CPU to respond to hardware (keyboard presses, network packets) and prioritize tasks efficiently.

### Types of Interrupts:
|Type                |Triggered by                    | Example                                     |
|--------------------|--------------------------------|---------------------------------------------|
|Hardware Interrupts |External hardware (keyboard, ...)|Pressing a key, receiving a network packet   |
|Software Interrupts |CPU-internal events             |System calls (`read()`, `write()`, `fork()`) |
|Exceptions          |Error or faults                 |Segmentation fault (SIGSEGV), divided by zero|

## How the CPU Handles Interrupts?
1. An interrupt occurs,
2. The CPU pauses the current process,
3. It saves the registers and program counter (so it can resume later),
4. It jumps to the **Interrupt Service Routine (ISR)**, a kernel function that handles the event,
5. Once handled, the CPU restores the process and continues executions.

We can check the active interrupts:
```bash
cat /proc/interrupts
```
It shows something like:
```bash
           CPU0       CPU1       CPU2       CPU3       CPU4       CPU5       CPU6       CPU7       
  1:          0          0          0          0          0          0          0       4091   IO-APIC   1-edge      i8042
  8:          0          0          0          0          0          0          0          0   IO-APIC   8-edge      rtc0
  9:          0          0          0          0          0          0          0          0   IO-APIC   9-fasteoi   acpi
 12:          0          0          0          0          0          0        144          0   IO-APIC  12-edge      i8042
 16:          0          0          0          0          0          0          0          0   IO-APIC  16-fasteoi   i801_smbus
 21:          0          0          0          0          0       1073          0          0   IO-APIC  21-fasteoi   qxl
 22:          0          0          0          0          0          0          0          0   IO-APIC  22-fasteoi   virtio3
 ...
 ```


 ## What Are System Calls?
 System calls (syscalls) are how user-space programs request services from the kernel.

 For example, when run:
 ```c
 write(1, "Hello", 5);
 ```
 The `write()` function is not directly writing to the screen; it is making a **system call** (`sys_write`) to ask the kernel to do it.
 * System calls allow user-space to access hardware safely,
 * They are entry points into the kernel without giving full control.

 To see the available system calls:
 ```bash
 man 2 syscalls
 ```
(There is a lot!)

or (if exists)
```bash
cat /proc/syscall
```

## Kernel vs. User Space The Barrier:
User-space applications cannot access kernel functions directly; instead, they must go through: **system calls** (`read`, `write`, `open`)
- `ioctl()` for device drivers,
- `mmap()` for memory management.

To check what syscalls a program make:
```bash
strace ls
```
which shows every system call made when run `ls`.

## Software Interrupts and Context Switching:
Interrupts also trigger context switches, which let the kernel swap processes efficiently.
- A timer interrupt tells the scheduler to switch tasks, 
- A software interrupt (`int 0x80`) is how syscalls work in x86

To see all running processes and their context switches:
```bash
cat /proc/sched_debug | grep -A10 "task"
```
