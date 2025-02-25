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
  1:          0          0          0          0          0          0          0       8744   IO-APIC   1-edge      i8042
  8:          0          0          0          0          0          0          0          0   IO-APIC   8-edge      rtc0
  9:          0          0          0          0          0          0          0          0   IO-APIC   9-fasteoi   acpi
 12:          0          0          0          0          0          0        144          0   IO-APIC  12-edge      i8042
 16:          0          0          0          0          0          0          0          0   IO-APIC  16-fasteoi   i801_smbus
 21:          0          0          0          0          0       2846          0          0   IO-APIC  21-fasteoi   qxl
 ...
 54:          0          0          0         85          0          0          0          0  PCI-MSIX-0000:02:00.0   0-edge      xhci_hcd
 63:          0          0          0          0       1966          0          0          0  PCI-MSI-0000:00:1f.2   0-edge      ahci[0000:00:1f.2]
 64:          0          0          0          0          0          0          0        562  PCI-MSI-0000:00:1b.0   0-edge      snd_hda_intel:card0
NMI:          0          0          0          0          0          0          0          0   Non-maskable interrupts
LOC:     218350     265674     235427     244692     239381     243945     222000     232790   Local timer interrupts
...
SPU:          0          0          0          0          0          0          0          0   Spurious interrupts
CAL:     644036     541046     438163     460180     443055     429467     426495     428718   Function call interrupts
TLB:      86010      89776      86646      87977      90198      85229      89249      89287   TLB shootdowns
...
MCE:          0          0          0          0          0          0          0          0   Machine check exceptions
MCP:         12         12         12         12         12         12         12         12   Machine check polls
ERR:          0
MIS:          0
...
PIW:          0          0          0          0          0          0          0          0   Posted-interrupt wakeup event
 ```
#### How to read those?
This file provides real-time statistics on interrupts handled by each CPU core. It shows how hardware devices and the kernel interact with the CPU.

**Understanding the columns:**
|Columns                         |Meaning                                                                       |
|--------------------------------|------------------------------------------------------------------------------|
|Number (`1, 8, 9, 12...`)       |The IRQ (interrupt Requests) number assigned to a device                       |
|CPU0, CPU1, ...                 |The count of times that interrupt has been handled by each CPU                 |
|Type (`IO-APIC, PCI-MSIX, etc.`)|Type of interrupt controller handling the IRQ                                |
|Description                     |The driver of a device associated with the interrupt                           |

**Breaking down:**
```bash
           CPU0       CPU1       CPU2       CPU3       CPU4       CPU5       CPU6       CPU7       
  1:          0          0          0          0          0          0          0       8744   IO-APIC   1-edge      i8042
```
* IRG `1` (Interrupt Request Number 1): `i8042` (keyboard controller),
* Handled only by CPU7 (8744 times)
```bash
 21:          0          0          0          0          0       2846          0          0   IO-APIC  21-fasteoi   qxl
```
* IRQ `21`: `qxl` (a virtual GPU driver for QEMU.KVM VMs),
* Processed `2846` times on CPU5 only

**Special Interrupt Types**:
|Type               |Meaning                                                                               |
|-------------------|--------------------------------------------------------------------------------------|
|`IO-APIC`          |Used for handling interrupts on modern systems (Edge or FastEOI)                      |
|`PCI-MSIX`         |MSI-X (Message Signaled Interrupts), used by modern PCIs devices for better performance|
|`NMI`              |None-masckable Interrupts (critical hardware failures)                               |
|`SPU`              |Spurious interrupts (unexpected/unhanded interrupts)                                   |
|`CAL`              |Function call interrupts (kernel scheduling work)                                      |
|`TLB`              |Translation Look aside Buffer, shootdown (MMU/cache operations)                        |
|`HYP`              | Hypervisor-related interrupts (VM host-guest communication)                          |

**How to use this data?**
- Find CPU-intensive devices,
    * If one CPU is handling most of the interrupts, consider balancing the load,
- Check if the hardware is being used efficiently:
    * if a device has very few interrupts it might be inactive or not working properly,
- Diagnose performance:
    * A high interrupt rate on a CPU could indicate a bottleneck (e. g., a network card receiving too many packets).


**Check the real-time interrupts:**
```bash
watch -n 1 cat /proc/interrpts
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

---
---
## Writing a Custom System Call:
1. Modify `syscalls.btl` to add a new entry,
2. Write a function in `kernel/sys.c` that prints a message,
3. Recompile the kernel with the new system call,
4. Use `syscall()` in **C** to call it.

**adding a custom system call to the Linux kernel**


### **Overview: What We Will Do**
We will **add a custom system call** that simply prints `"Hello from my system call!"` to the kernel log.

1. **Modify `syscalls.tbl`** to register the new syscall.
2. **Create a syscall function (`sys_mycall`)** in the kernel.
3. **Declare the syscall in the header files (`unistd.h`).**
4. **Recompile the kernel.**
5. **Write a user-space program** to call the new syscall using `syscall()`.

---

#### **Step 1: Modify `syscalls.tbl`**
Each syscall in Linux has an **assigned number** in `syscalls.tbl`, which maps syscall names to numbers.

**Find the correct `syscalls.tbl` file:**
```bash
cd /usr/src/linux/arch/x86/entry/syscalls
ls
```
There is **`syscall_64.tbl`** for **64-bit** systems and **`syscall_32.tbl`** for **32-bit**.

**Edit the table to add a new entry:**
Open the **`syscall_64.tbl`** file:
```bash
sudo nano syscall_64.tbl
```
Add the following **at the end of the table** (choose a number not in use, e.g., `450`):
```
450  common  mycall   sys_mycall
```

---

#### **Step 2: Write the Syscall Implementation**
**Open the system call source file:**
```bash
cd /usr/src/linux/kernel
sudo nano sys.c
```
**Add this function at the end:**
```c
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(mycall) {
    printk(KERN_INFO "Hello from my system call!\n");
    return 0;
}
```
* `SYSCALL_DEFINE0(mycall)` defines a syscall with **zero arguments**.
* `printk(KERN_INFO "...")` logs the message in the kernel log (`dmesg`).

---

#### **Step 3: Declare the Syscall in Header Files**
**Modify `unistd_64.h`**
```bash
cd /usr/src/linux/include/uapi/asm-generic
sudo nano unistd.h
```
Add:
```c
#define __NR_mycall  450
```
* **This associates our syscall number (`450`) with `sys_mycall`.**

---

#### **Step 4: Recompile the Kernel**
**Recompile the kernel with the new system call:**
```bash
cd /usr/src/linux
sudo make -j$(nproc)
sudo make modules_install
sudo make install
```
**Reboot into the new kernel:**
```bash
sudo reboot
```
* Now, the new syscall **exists in the kernel**!

---

## ** Step 5: Write a User-Space Program to Test It**
**Create a test program (`test_mycall.c`):**
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_MYCALL 450  // The syscall number we assigned

int main() {
    long res = syscall(SYS_MYCALL);
    printf("Syscall returned: %ld\n", res);
    return 0;
}
```
**Compile and Run:**
```bash
gcc test_mycall.c -o test_mycall
./test_mycall
```
**Check the Kernel Logs (`dmesg`):**
```bash
sudo dmesg | tail -10
```
* If everything worked, there should be:
```
Hello from my system call!
```

---
