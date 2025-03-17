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
