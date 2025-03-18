#  Kernel Debugging and Profiling in Linux

Kernel debugging and profiling help identify performance bottlenecks, crashes, and unexpected behavior.

## 1. Kernel Debugging: Finding and Fixing Issues
### 1.1 Understanding Kennel Logs (`dmesg`)
When the kernel encounters an issue, it logs messages in the ring buffer, which you can access with:
```bash
dmesg | less
```
it usually need `sudo` to run the command.
some example of the output:
```bash
[[    0.000000] Linux version 6.8.0-52-generic (Ubuntu 6.8.0-52.53-generic 6.8.12)
[    0.000000] Command line: BOOT_IMAGE=/boot/vmlinuz-6.8.0-52-generic root=UUID=REDACTED ro quiet splash vt.handoff=7
[    0.000000] KERNEL supported cpus:
[    0.000000]   Intel GenuineIntel
[    0.000000]   AMD AuthenticAMD
[    0.000000]   Hygon HygonGenuine
[    0.000000]   Centaur CentaurHauls
[    0.000000]   zhaoxin   Shanghai
[    0.000000] BIOS-provided physical RAM map:
[    0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable
[    0.000000] BIOS-e820: [mem 0x000000000009fc00-0x000000000009ffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000000f0000-0x00000000000fffff] reserved
[    0.000000] BIOS-e820: [mem 0x0000000000100000-0x000000007ffdafff] usable
```
#### **Short Explanation of `dmesg` Output**

```bash
[    0.000000] Linux version 6.8.0-52-generic (Ubuntu 6.8.0-52.53-generic 6.8.12)
```
- **Kernel version** running on the system.

```bash
[    0.000000] Command line: BOOT_IMAGE=/boot/vmlinuz-6.8.0-52-generic root=UUID=REDACTED ro quiet splash vt.handoff=7
```
- **Kernel boot parameters**:
  - `BOOT_IMAGE=...` → Kernel image loaded at boot,
  - `root=UUID=REDACTED` → Root filesystem identifier,
  - `ro` → Mounts root filesystem as **read-only** initially,
  - `quiet splash` → Hides detailed boot logs, shows a splash screen.

```bash
[    0.000000] KERNEL supported cpus:
[    0.000000]   Intel GenuineIntel
[    0.000000]   AMD AuthenticAMD
[    0.000000]   Hygon HygonGenuine
[    0.000000]   Centaur CentaurHauls
[    0.000000]   zhaoxin   Shanghai
```
- Lists **supported CPU architectures**.

```bash
[    0.000000] BIOS-provided physical RAM map:
[    0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable
```
- **Memory map from BIOS**:
  - **Usable** → Available to the OS,
  - **Reserved** → Used by BIOS or hardware.

**TL;DR:**
- Kernel version & boot settings.
- Supported CPUs.
- Memory layout (usable vs. reserved).

- Useful for checking **boot logs, hardware issues**, and **drivers errors**.
---
- To filter for errors:
```bash
dmesg --level=err,warn
```
an output:
```bash
[    0.601295] device-mapper: core: CONFIG_IMA_DISABLE_HTABLE is disabled. Duplicate IMA measurements will not be recorded in the IMA log.
[    0.601546] platform eisa.0: Cannot allocate resource for mainboard
[    0.601548] platform eisa.0: Cannot allocate resource for EISA slot 1
[    0.601549] platform eisa.0: Cannot allocate resource for EISA slot 2
[    0.601557] platform eisa.0: Cannot allocate resource for EISA slot 3
[    0.601558] platform eisa.0: Cannot allocate resource for EISA slot 4
[    0.601559] platform eisa.0: Cannot allocate resource for EISA slot 5
[    0.601559] platform eisa.0: Cannot allocate resource for EISA slot 6
[    0.601560] platform eisa.0: Cannot allocate resource for EISA slot 7
[    0.601561] platform eisa.0: Cannot allocate resource for EISA slot 8
[  123.846749] kauditd_printk_skb: Callbacks suppressed
```
#### **Short Explanation**
1. **IMA Log Warning**
   ```bash
   [    0.601295] device-mapper: core: CONFIG_IMA_DISABLE_HTABLE is disabled.
   ```
   - **Integrity Measurement Architecture (IMA)** logs duplicate measurements.
   - This is a **security feature**, and the warning means that duplicate entries **won’t be ignored**.

2. **EISA Resource Allocation Errors**
   ```bash
   [    0.601546] platform eisa.0: Cannot allocate resource for mainboard
   [    0.601548] platform eisa.0: Cannot allocate resource for EISA slot 1
   ```
   - **EISA (Old Expansion Slot System)**:
     - These messages indicate that the **system doesn’t have EISA hardware**, but the kernel **still checks for it**.
     - **Usually safe to ignore** unless dealing with legacy hardware.

3. **Audit Log Suppression**
   ```bash
   [  123.846749] kauditd_printk_skb: Callbacks suppressed
   ```
   - The **Linux Audit System (`kauditd`)** logs security events.
   - This message means **multiple audit logs were combined to avoid flooding the system**.

---

and to monitor logs **in real time**:
```bash
dmesg -w
```

**Example of Checking for a Kernel Crash (Oops/Panic):**
```bash
dmesg | grep -i "oops"
```
- A **kernel oops** means a process crashed due to an invalid memory access,
- A **kernel panic** is more severe - requiring a system reboot.

### 1.2 Using `sysrq` for Emergency Debugging
if the system becomes **unresponsive**, Linux provides a **magic SysRq key** (System Request) for emergency debugging.

**Enable SysRq**

Check if SysRq is enabled:
```bash
cat /proc/sys/kernel/sysrq
```
if `0`, enable it:
```bash
echo ` | sudo tee /proc/sys/kernel/sysrq
```
**Useful SysRq Commands:**
*This command will reboot the system!*
```bash
echo b > /proc/sysrq-trigger
```
Sync files and remount disks read-only before rebooting:
```bash
echo s > /proc/sysrq-trigger
echo u > /proc/sysrq-trigger
echo b > /proc/sysrq-trigger
```
- `s` -> Sync discs,
- `u` -> Unmount filesystem safely,
- `b` -> Reboot immediately.

### 1.3 Debugging a Running Kernel with `kdb` and `kgdb`
*GDB = GNU Debugger*

For **live kernel debugging**, Linux provides:
- `kdb` -> A Built-in kernel debugger,
- `kgdb` -> Remote debugging with **GDB**.

#### **Enable Kernel Debugging (Boot Parameter)**
To enable Kernel Debugging features, add this to the kernel boot options:
```bash
kgdboc=ttys0,115200 kgdbwait
```
- This enables debugging over a **serial port**,
- The system will wait for **GDB** to attach before booting.

#### **Attach GDB to the Kernel (`kgdb`)**
On a **remote system**:
```bash
gdb vmlinux
```
Then connect to the target system:
```gdb
target remote /dev/ttys0
```
Now we can:
- Set breakpoints (`b function_name`),
- Step through execution (`step`, `next`),
- Print kernel variables (`p variable_name`).

#### **Analyzing Kernel Crashes with `kdump`**
`kdump` captures a **memory dump** when the kernel crashed, useful for **post-mortem analysis**.

#####  What is postmortem?
*A postmortem is a written record of an incident that describes:*
- *The incident's impact,*
- *The actions taken to mitigate or resolve the incident,*
- *The incident's root cause,*
- *Follow-up actions taken to prevent the incident from happening again [[atlassian](https://www.atlassian.com/incident-management/handbook/postmortems#why-do-we-do-postmortems)].*

#### **Enable `kdupm`:**
1. Install `kdump` (A reboot is required afterward):
```bash
sudo apt install kdump-tools
```
2. Enable it:
```bash
sudo systemctl enable kdump-tools
```
3. Trigger a test crash (need to run it as sudoer, it will reboot the system):
```bash
echo c > /proc/sysqr-trigger
```
4. Find the crash dump in `/var/crash/` and analyze with `crash` (*Did not work on the first try!*):
```bash
sudo crash /usr/lib/debug/boot/vmlinux-$(uname -r) /varcrash/vmcore
```

## 2. Kernel Profiling: Measuring Performance
Once the kernel is running, **profiling** tools help analyze **CPU, Memory, and I/O performance**.

### 2.1 Using `perf` for Performance Analysis
`perf` is the **standard Linux profiling tool** for kernel and user-space performance.

**Install** `perf`
```bash
sudo apt install linux-tools-common linux-tools-$(uname -r)
```
**Monitor System-wide Performance**
```bash
sudo perf top
```
Example of the output:
```bash
Samples: 65K of event 'cycles:P', 4000 Hz, Event count (approx.): 36587273962 lost: 0/0 drop: 0/0
Overhead  Shared Object                          Symbol
   6.10%  libvk_swiftshader.so                   [.] 0x00000000000f65a4
   4.51%  libvk_swiftshader.so                   [.] 0x00000000000f659f
   2.61%  libvk_swiftshader.so                   [.] 0x00000000000f6577
   2.55%  libvk_swiftshader.so                   [.] 0x00000000000f6587
   2.52%  libvk_swiftshader.so                   [.] 0x00000000000f658f
   2.50%  libvk_swiftshader.so                   [.] 0x00000000000f6583
   2.41%  libvk_swiftshader.so                   [.] 0x00000000000f658b
   2.33%  libvk_swiftshader.so                   [.] 0x00000000000f6593
   2.33%  libvk_swiftshader.so                   [.] 0x00000000000f659b
   2.28%  libvk_swiftshader.so                   [.] 0x00000000000f6597
   1.99%  libc.so.6                              [.] pthread_mutex_unlock@@GLIBC_2.2.5
   1.98%  libc.so.6                              [.] pthread_mutex_lock@@GLIBC_2.2.5
   1.24%  [kernel]                               [k] clear_bhb_loop
   0.92%  [kernel]                               [k] __calc_delta.constprop.0
   0.90%  [kernel]                               [k] pick_next_task_fair
   0.85%  libvk_swiftshader.so                   [.] 0x00000000000f6595
   0.78%  [kernel]                               [k] entry_SYSCALL_64
   0.73%  [kernel]                               [k] update_curr
   0.70%  [kernel]                               [k] pick_eevdf
   0.67%  [kernel]                               [k] pvclock_clocksource_read_nowd
For a higher level overview, try: perf top --sort comm,dso
```

#### Understanding the `perf top` output
1. **Samples**:  
   - `65K of event 'cycles:P'` means perf has collected 65,000 samples of the “CPU cycles” performance event (i.e., how many CPU cycles code is consuming).
   - `4000 Hz` is the sampling frequency, i.e., perf tries to sample about 4000 times per second.
   - `Event count (approx.): 36587273962` is an estimate of the total number of CPU cycles recorded in those samples.

2. **Overhead**:  
   - The “Overhead” column shows the percentage of samples for which the program counter (instruction pointer) was in that function/symbol. In simpler terms, it’s an approximation of how much CPU time is spent in that particular function relative to the total sampled time.

3. **Shared Object / DSO (Dynamic Shared Object)**:  
   - This column tells you which library or binary the function/symbol belongs to. For example, `libvk_swiftshader.so` is a Vulkan software rendering library (SwiftShader), `libc.so.6` is the C standard library, and `[kernel]` refers to the Linux kernel.

4. **Symbol**:  
   - This is the function or symbol name. Sometimes, especially in stripped binaries or JIT-generated code, you only see numeric addresses (`0x00000000000f65a4`) rather than a more user-friendly symbol name (e.g., `vkSomeFunction`).

#### What the output means

- **`libvk_swiftshader.so`** is at the top of the list, meaning a significant portion of CPU cycles is spent in this library. If you’re running a 3D application or something that uses Vulkan with SwiftShader, this is expected—SwiftShader is a CPU-based implementation of the Vulkan (or OpenGL) APIs, and it’s known to be quite CPU-intensive compared to hardware GPU acceleration.
  
- **`pthread_mutex_lock` / `pthread_mutex_unlock`** from `libc.so.6`: This indicates that your program(s) are frequently acquiring and releasing mutexes. It’s not unusual in multi-threaded applications.

- **`[kernel]`** symbols like `pick_next_task_fair`, `update_curr`, etc. are part of the CPU scheduler’s logic. Seeing some overhead in these functions is normal as the kernel decides which thread gets the CPU next.

- **`clear_bhb_loop`, `pvclock_clocksource_read_nowd`,** etc. are lower-level kernel or hardware-related routines. A small percentage there is usually normal overhead for kernel functionality and housekeeping.

---

### Profile a Specific Command
```bash
sudo perf record -g ./my_program
```
- Runs `my_programs` and records execution data.

### View Detailed Performance Report
```bash
sudo perf report
```
- Analyzes collected data

### Trace System Calss (`perf trace`)
```bash
sudo perf trace -p $(pidof firefox)
```
or for only one PID
```bash
# Use 'pidof -s' (short for '--single-shot') to return only one PID
sudo perf trace -p $(pidof -s firefox)
```
- Trace **syscalls** made by **Firefox**.

### 2.2 Using `ftrace` for Kernel Function Tracing
`ftrace` us the kernel's built-in function tracer.

**Enable Function Tracing**
```bash
echo function > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on
```
**View Tracing Output**
```bash
cat /sys/kernel/debug/tracing/trace
```
- Shows function calls executed by the kernel.

**Stop Tracing**
```bash
echo 0 > /sys/kernel/debug/tracing/tracing_on
```

### 2.3 Using `eBPF` for Advanced Observably
`eBPF` (**Extended Berkeley Packet Filter**) provides a low-overhead way to analyze kernel activity.

#### List Available `bpftrace` Programs
```bash
bpftrace -l
```
[You may need to install it, and run it as sudoer.]

#### Trace Process Scheduling
```bash
sudo bpftrace -e 'tracepoint:sched:sched_switch { printf("%s -> %s\n", args.prev_comm, args.next_comm); }'
```

## 3. Debugging Kernel Modules
In developing **kernel modules**, debugging can be difficult since `printf` does not work in kernel space. Instead, use:

### 3.1 Debug with `printk` (Kernel Logs)
```c
prink(KERN_INFO "This is a debug message\n");
```
check the logs:
```bash
dmesg | tail -50
```

### 3.2 Debug with `debugfs`
Create a debug entry:
```c
debugfs_create_file("my_debug", 0644, NULL, NULL, &fops);
```
Read the output:
```bash
cat /sys/kernel/debug/my_debug
```

## 4. Summary: Key Debugging and Profiling Commands
|Task                    |	Command                        |
|------------------------|---------------------------------|
|View kernel logs	       |`dmesg`                          |
|Live system debugging   |	`sysrq`                        |
|Enable kernel debugging | Add `kgdboc` in boot options    |
|Debug kernel with GDB   | `gdb vmlinux` + `target remote` |
|Capture a kernel crash  | `kdump`                         |
|Profile CPU usage       | `perf top`                      |
|Profile a program       | `perf record -g ./program`      |
|Trace function calls	   | `ftrace`                        |
|Use eBPF tracing        | `bpftrace -e '...'`             |
