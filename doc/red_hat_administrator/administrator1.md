# Get Started with Red Hat Enterprise Linux (RHEL)
RH124 is about:

* Being able to manage a Red Hat system via CLI, 
* Performing essential admin tasks manually, 
* Prepping you for automation later with Ansible,

## 1. Check Shell, Terminal, and User
```bash
echo $SHELL
echo $TERM
whoami
```
output:
```bash
/bin/bash
xterm-256color
santiago
```

### Display System Info
```bash
hostnamectl
```
output:
```bash
Static hostname: (unset)                                 
Transient hostname: localhost
         Icon name: computer-vm
           Chassis: vm 🖴
        Machine ID: [REDACTED]
           Boot ID: [REDACTED]
    Virtualization: kvm
  Operating System: Red Hat Enterprise Linux 9.5 (Plow)     
       CPE OS Name: cpe:/o:redhat:enterprise_linux:9::baseos
            Kernel: Linux 5.14.0-503.33.1.el9_5.x86_64
      Architecture: x86-64
   Hardware Vendor: QEMU
    Hardware Model: Standard PC _Q35 + ICH9, 2009_
  Firmware Version: 1.16.3-debian-1.16.3-2
```
output:

- **Static hostname:** Permanent hostname set for the system. `(unset)` means no static hostname was configured.
- **Transient hostname:** Temporary hostname for the system. Here it's set as `localhost`.
- **Icon name & Chassis:** Indicates the type of hardware, here shown as `computer-vm`, meaning it's a virtual machine (VM).
- **Machine ID:** A unique identifier generated when the OS was installed (unique to each installation).
- **Boot ID:** Changes every time the machine is rebooted; not permanent.
- **Virtualization:** `kvm` indicates Kernel-based Virtual Machine virtualization.
- **Operating System:** Your OS version, here RHEL 9.5 (Plow).
- **Kernel:** Linux kernel version (`5.14.0-503.33.1.el9_5.x86_64`).
- **Architecture:** CPU architecture (`x86-64`).
- **Hardware Vendor:** Shows `QEMU`, indicating a virtualized hardware environment (typical for KVM/QEMU virtualization).
- **Hardware Model:** Virtual machine configuration model (`Standard PC Q35 + ICH9, 2009`).
- **Firmware Version:** Version of the virtual machine’s firmware (from Debian's QEMU package).

```bash
uname -r
uptime
```
output:
```bash
5.14.0-503.33.1.el9_5.x86_64
16:33:20 up 26 min,  2 users,  load average: 0.03, 0.13, 0.10
```

### Switch Between Consoles
On a GUI VM, try:
* `Ctrl` + `Alt` + `F2`: open virtual console

* `Ctrl` + `Alt` + `F1`: return to GUI

Use tty to see which terminal you're on:
```bash
tty
```

## Managing Files from the Command Line
Similar to Debian

###  Wildcards and Quoting

| Symbol | Meaning           | Example                  |
|--------|-------------------|--------------------------|
| `*`    | Zero/more chars   | `ls *.log`               |
| `?`    | Single char       | `ls file?.txt`           |
| `[]`   | Char class        | `ls file[1-3].txt`       |

```bash
touch file1.txt file2.txt fileA.txt
ls file*.txt
ls file[12].txt
```
### Inspecting Files
```bash
du -sh .
324K    .
```
output:
- `du -sh .`: Disk usage (`du`) summarized (`-s`) and human-readable (`-h`) for the current directory (`.`).
- `324K .`: Means your current directory (`KernelPanorama`) uses 324 kilobytes.

```bash
df -h
```
output:

```bash
df -h
Filesystem             Size  Used Avail Use% Mounted on
devtmpfs               4.0M     0  4.0M   0% /dev
tmpfs                  2.0G   27M  2.0G   2% /dev/shm
tmpfs                  817M  9.3M  808M   2% /run
/dev/mapper/rhel-root   17G  6.0G   11G  36% /
/dev/vda1              960M  350M  611M  37% /boot
tmpfs                  409M  104K  409M   1% /run/user/1000
```

This command shows disk space usage:

- **Filesystem**: Partitions and tmpfs mounted.
- **Size, Used, Avail, Use%**: Disk size, how much space is used and available, and usage percentage.
- **Mounted on**: Where these filesystems are mounted.

The key filesystem partitions here:
- `/dev/mapper/rhel-root`: Main root filesystem with total size (17 GB) and usage details.
- `/dev/vda1`: Boot partition (960 MB size).
- The rest (`tmpfs` and `devtmpfs`) are temporary file systems stored in RAM and used by the OS for temporary storage. They don’t contain sensitive information.

## 3. Managing Local Users and Groups
Create, manage, and configure users, groups, passwords, and permissions.

### 1. **Create a New User**

```bash
sudo useradd john
sudo passwd john
```

Check the home directory and shell:
```bash
grep john /etc/passwd
ls -l /home/john/
```

### 2. **User Details Breakdown**
These files are your admin playground:

| File               | Purpose                         |
|--------------------|---------------------------------|
| `/etc/passwd`      | Basic user info                 |
| `/etc/shadow`      | Encrypted passwords             |
| `/etc/group`       | Group definitions               |
| `/etc/gshadow`     | Group passwords (rarely used)   |

### 3. **Group Management**

```bash
sudo groupadd devs
sudo usermod -aG devs john     # Add user to group
groups john
```

- Make `devs` the **primary group**

```bash
sudo usermod -g devs john
```

- Difference:
- **Primary group**: shown in `id john` → 1 main group
- **Supplementary groups**: additional memberships

### 4. **Delete a User or Group**

```bash
sudo userdel john              # keep home dir
sudo userdel -r john           # remove home dir
sudo groupdel devs
```

### 5. **Password Management**

```bash
sudo passwd john               # set password
sudo chage -l john             # check aging info
sudo chage -M 60 john          # force password change every 60 days
```

### 6. **Force Password Reset at Next Login**

```bash
sudo chage -d 0 john
```

Next time `john` logs in, they must change their password.


### 7. `/etc/skel/` — Default Files for New Users

####  What it is:
When you create a new user (e.g., with `useradd`), files in `/etc/skel/` are **automatically copied** into the user’s home directory.

```bash
ls -a /etc/skel/
```

Typical contents:
```text
.bash_profile
.bashrc
.bash_logout
```

#### Why it matters:
- These files set up a **default shell environment** for every new user.
- You can add custom files (e.g., a welcome message, `.vimrc`, etc.) to `/etc/skel/` to give users consistent settings.

**Example**:
```bash
echo "Welcome to our server!" > /etc/skel/welcome.txt
sudo useradd testuser
ls /home/testuser/
```

You’ll see `welcome.txt` inside `testuser`’s home dir.


### Locking and Unlocking Users

#### Lock a user (disable login):
```bash
sudo passwd -l username
```

This **adds a `!`** in front of the password hash in `/etc/shadow`, disabling it.

####  Unlock the user:
```bash
sudo passwd -u username
```

This removes the `!`, allowing login again.

#### Use cases:
- Temporarily disable accounts (e.g., leaving employees)
- Quickly secure an account without deleting it


###  `/etc/passwd`, `/etc/shadow`, `/etc/group` — Formats

#### `/etc/passwd` — User Accounts

Each line = 1 user

Format:
```text
username:x:UID:GID:comment:home_directory:shell
```

Example:
```text
john:x:1001:1001:John Doe:/home/john:/bin/bash
```

| Field        | Meaning                     |
|--------------|-----------------------------|
| `john`       | Username                    |
| `x`          | Password is in `/etc/shadow`|
| `1001`       | User ID                     |
| `1001`       | Group ID                    |
| `John Doe`   | Optional comment (GECOS)    |
| `/home/john` | Home directory              |
| `/bin/bash`  | Login shell                 |


### `/etc/shadow` — Encrypted Passwords

Format:
```text
username:encrypted_password:last_change:min:max:warn:inactive:expire
```

Example:
```text
john:$6$Hd.28...:19514:0:99999:7:::
```

| Field             | Meaning                                |
|-------------------|----------------------------------------|
| `john`            | Username                               |
| `$6$...`          | Encrypted password                     |
| `19514`           | Last password change (days since 1970) |
| `0`               | Min days before password change        |
| `99999`           | Max days password valid                |
| `7`               | Warn user before expiration            |
| Others            | Rarely used                            |

### `/etc/group` — Group Info

Format:
```text
groupname:x:GID:members
```

Example:
```text
devs:x:1002:john,alice
```

| Field        | Meaning                                      |
|--------------|----------------------------------------------|
| `devs`       | Group name                                   |
| `x`          | Placeholder for group password (rarely used) |
| `1002`       | Group ID                                     |
| `john,alice` | Group members (optional)                     |


### Summary

| File              | Stores                     |
|-------------------|----------------------------|
| `/etc/passwd`     | All user account info      |
| `/etc/shadow`     | Encrypted passwords + aging|
| `/etc/group`      | Group info and members     |
| `/etc/skel/`      | Default files for new users|
| `passwd -l/-u`    | Lock/unlock user accounts  |

## 4. File Permissions and Ownership
*SEE [security access control](../general_review/security_access_control.md)*

Control *who* can read, write, and execute *what*, and understand how ownership affects access.


###  1. **Understanding File Permissions (ls -l)**

```bash
ls -l filename
```

Example output:
```bash
-rw-r--r--. 1 alice devs  9312 Mar 31 20:23 file.txt
```

Breakdown:

| Field         | Meaning                   |
|---------------|---------------------------|
| `-rw-r--r--`  | File type and permissions |
| `1`           | Hard link count           |
| `alice`       | File owner                |
| `devs`        | Group owner               |
| `9312`        | File size in bytes        |
| `Mar 31 20:23`| Timestamp                 |
| `file.txt`    | File name                 |

#### Permission symbols:

```text
[ file type ] [ user ] [ group ] [ others ]
    -           rw-      r--       r--
```

- `r` = read
- `w` = write
- `x` = execute
- `-` = permission not given

## 2. **Change Permissions – chmod**

### Symbolic mode:
```bash
chmod u+x script.sh       # add execute for user
chmod g-w file.txt        # remove write for group
chmod o=r myfile          # only read for others
```

#### Octal mode:

| Number | Permission |
|--------|------------|
| 7      | rwx        |
| 6      | rw-        |
| 5      | r-x        |
| 4      | r--        |
| 0      | ---        |

```bash
chmod 755 myscript.sh     # u=rwx, g=rx, o=rx
chmod 644 file.txt        # u=rw, g=r, o=r
```

### 3. **Change Ownership – chown & chgrp**

```bash
chown alice file.txt            # change user owner
chown alice:devs file.txt       # change user & group
chgrp devs file.txt             # change group only
```

### 4. **Default Permissions – umask**

Check your default file permission mask:

```bash
umask
```

- Default: `0022` → files get `644`, dirs get `755`

Calculate default permission:
```
Permissions = 666 (file) or 777 (dir) - umask
```

To temporarily change it:
```bash
umask 0077                    # very private: only user has access
```

### 5. **Special Permissions**

| Name           | Symbol        | Purpose           |
|----------------|---------------|-------------------|
| **SUID**       | `s` on user   | Run as file owner |
| **SGID**       | `s` on group  | Run with group privileges or retain group on new files in dirs |
| **Sticky Bit** | `t` on others | Restrict file deletion in shared dirs like `/tmp` |

#### Examples:

- **SUID**: `/usr/bin/passwd` → lets users change their password

```bash
ls -l /usr/bin/passwd
# -rwsr-xr-x
```

- **SGID on a dir**:
```bash
chmod g+s /project
```

- **Sticky bit on /tmp**:
```bash
chmod +t /tmp
```

## 5. Editing Text Files from the Command Line

Create, edit, save, and navigate text files using the `vim` editor — a vital skill for sysadmins configuring scripts, services, and system files.


### Step 1: Launch `vim`

```bash
vim test.txt
```

- If the file doesn't exist, it creates a new one.
- You're now in the **normal mode**.

### Step 2: Understand `vim` Modes

| Mode         | Purpose                                 |
|--------------|------------------------------------------|
| **Normal**   | Navigate, delete, copy, paste            |
| **Insert**   | Type/edit text                           |
| **Command**  | Save, quit, search, substitute, etc.     |

You start in **Normal mode**.


### Step 3: Enter Insert Mode (Start Writing)

In normal mode, press:
```text
i
```
Now you're in **insert mode**. Start typing anything.


### Step 4: Save and Quit

First, press:
```text
ESC    # (to leave insert mode)
```

Then, type one of the following in **command mode**:

| Command      | Action                      |
|--------------|-----------------------------|
| `:w`         | Save                        |
| `:q`         | Quit                        |
| `:wq`        | Save and quit               |
| `:q!`        | Quit without saving         |


### Step 5: Navigate Inside a File

In **normal mode**:
| Key     | Action                  |
|---------|-------------------------|
| `h` `l` | move left/right         |
| `j` `k` | move down/up            |
| `0`     | move to beginning of line |
| `$`     | move to end of line     |
| `G`     | move to end of file     |
| `gg`    | move to start of file   |

### Step 6: Delete, Copy, Paste

| Command | Action                    |
|---------|---------------------------|
| `dd`    | delete (cut) current line |
| `yy`    | copy current line         |
| `p`     | paste                     |
| `u`     | undo                      |

### Step 7: Search and Replace

#### Search:
```bash
/foo
```
Moves cursor to first match of “foo”.  
Use `n` for next match, `N` for previous.

#### Replace:
```bash
:%s/foo/bar/g
```
Replaces **all** “foo” with “bar” in the file.


## 6. Archiving and Compression (tar, gzip, xz)

**Archive**, **compress**, **extract**, and **inspect** files using CLI tools — essential for backups, transfers, and deployments.


###  1. **Archiving with `tar`** (The King of Archiving)

#### Create a `.tar` archive:
```bash
tar -cvf archive.tar file1 file2 dir/
```

| Option | Meaning         |
|--------|------------------|
| `-c`   | Create           |
| `-v`   | Verbose (show files) |
| `-f`   | Filename to create |

#### List contents (without extracting):
```bash
tar -tvf archive.tar
```

#### Extract archive:
```bash
tar -xvf archive.tar
```

| `-x` | e**X**tract |

**Extract to specific directory**:
```bash
tar -xvf archive.tar -C /path/to/extract/
```

###  2. **Compress with gzip and xz**

#### Compress an archive with `gzip`:
```bash
gzip archive.tar
```
* This creates `archive.tar.gz` (or `.tgz`)

To decompress:
```bash
gunzip archive.tar.gz
```

#### Compress with `xz`:
```bash
xz archive.tar        # becomes archive.tar.xz
unxz archive.tar.xz
```

More control:
```bash
xz -k -v archive.tar   # keep original, show progress
```


### 3. **Combined: Create & Compress in One Go**

| Command | Result |
|--------|--------|
| `tar -czvf archive.tar.gz file1 dir/` | tar + gzip |
| `tar -cjvf archive.tar.bz2 file1 dir/` | tar + bzip2 |
| `tar -cJvf archive.tar.xz file1 dir/`  | tar + xz |

- `z` = gzip
- `j` = bzip2
- `J` = xz


###  4. **Extract a Compressed Archive**

```bash
tar -xzvf archive.tar.gz   # gzip
tar -xjvf archive.tar.bz2  # bzip2
tar -xJvf archive.tar.xz   # xz
```

## 7. `dnf`

#### **DNF = Dandified YUM**

- A **modern package manager** for RHEL, Fedora, CentOS, Rocky, Alma
- Replaced **YUM** (`Yellowdog Updater Modified`) starting from **RHEL 8**
- Handles:
  - Package install/remove
  - Dependency resolution
  - Updates
  - Repos
  - GPG key verification
  - Metadata caching

#### How DNF Works (Behind the Scenes)

- It's written in **Python + C**
- Works on **RPM** files (binary packages)
- Parses repo metadata in `/etc/yum.repos.d/`
- Maintains a **local SQLite metadata cache**
- Uses **libsolv** for dependency resolution (faster, smarter than YUM)

#### You can inspect the config:
```bash
cat /etc/dnf/dnf.conf
```

### What is `rpm`?

#### **RPM = Red Hat Package Manager**

- Low-level tool that handles **.rpm files**
- Think of it like `dpkg` in Debian

```bash
sudo rpm -ivh somefile.rpm
```

It does **not** auto-resolve dependencies — it will fail if deps are missing.

#### Used Internally By:
- `dnf`
- Anaconda (during install)
- Kickstart
- Some container tools (for OS layering)

### DNF vs RPM: Who Does What?

| Task                        | Use `dnf`? | Use `rpm`? |
|----------------------------|------------|------------|
| Install a package          | ✅         | ⚠️ (no deps) |
| Remove a package           | ✅         | ✅         |
| Query installed packages   | ✅         | ✅         |
| Install `.rpm` from disk   | ✅ (smart) | ✅ (manual) |
| Query ownership of a file  | ❌         | ✅         |
| Manage repos               | ✅         | ❌         |


### Security: GPG Keys

- When installing from a repo, `dnf` checks **GPG signatures**
- Keys are stored in:
```bash
/etc/pki/rpm-gpg/
```

To view trusted keys:
```bash
rpm -q gpg-pubkey
```

### Repo Metadata

Stored in:
```bash
/var/cache/dnf/
```

Each repo has metadata like:
- `repodata/primary.xml.gz`
- `filelists.xml.gz`

You can clear it:
```bash
sudo dnf clean all
```

And rebuild it:
```bash
sudo dnf makecache
```

### `dnf history` — See & Undo Package Transactions

DNF keeps track of every action it performs: installs, removals, upgrades, etc.

#### View full history:
```bash
dnf history
```

You’ll get output like:
```
ID | Command line             | Date and time    | Action(s)  | Altered
---+--------------------------+------------------+------------+--------
 5 | install httpd            | 2025-03-31 18:00 | Install    |    10
 4 | remove nano              | 2025-03-31 17:30 | Remove     |     1
```

#### Undo a transaction:
```bash
sudo dnf history undo 5
```
This rolls back what you did in transaction ID 5.

> It doesn’t always fully reverse complex dependency trees — but it's incredibly useful.

#### Redo a transaction:
```bash
sudo dnf history redo 4
```

### `dnf mark install` vs `dnf mark remove`

This is about **package status tracking** — whether a package is considered “user-installed” or a “dependency”.

#### `dnf mark install <pkg>`
Manually mark a package as explicitly installed, even if it was pulled in as a dependency.

```bash
sudo dnf mark install python3
```

#### `dnf mark remove <pkg>`
Mark it as “removable” (i.e., no longer needed by anything).

```bash
sudo dnf mark remove httpd
```

#### Use with `dnf autoremove`
If you don’t mark packages you want to keep, `dnf autoremove` could remove them accidentally.

> Think of this like apt’s `apt-mark manual` vs `apt-mark auto`.


### `dnf config-manager` (from `dnf-plugins-core`)

This is an **extension** of DNF, provided by the package:

```bash
sudo dnf install dnf-plugins-core
```

#### What it does:
Allows you to **enable/disable repos**, **add repos**, **set persistent config options**.

#### Examples:

Enable a repo:
```bash
sudo dnf config-manager --set-enabled codeready-builder-for-rhel-8-x86_64-rpms
```

Disable a repo:
```bash
sudo dnf config-manager --set-disabled <repo-id>
```

Add a repo:
```bash
sudo dnf config-manager --add-repo=http://my.repo.url/repo.repo
```

You can see enabled repos with:
```bash
dnf repolist
```

### `rpm --verify` — Check File Integrity

This checks **if a package's files have changed** from when it was first installed (e.g., manually edited config files, modified binaries, etc.)

#### Basic usage:
```bash
rpm -V bash
```

#### Output:
```
S.5....T.  c /etc/bashrc
```

Each letter means something changed:

| Char | Meaning                  |
|------|--------------------------|
| S    | File size                |
| M    | Mode (permissions)       |
| 5    | MD5 checksum             |
| D    | Device                   |
| L    | Symlink                  |
| U    | User ownership           |
| G    | Group ownership          |
| T    | Modification time        |
| c    | Config file              |

If no output → package files are unchanged.

### `rpmdev-extract` — Extract `.src.rpm` or `.rpm` Files

This comes from the **`rpmdevtools`** package:

```bash
sudo dnf install rpmdevtools
```

#### Extract contents of a source RPM:

```bash
rpmdev-extract mypackage.src.rpm
```

It will unpack the `.spec` file and source tarballs.

#### Also works on binary `.rpm` files:

```bash
rpmdev-extract some.rpm
```

You get:
```
mypackage/
├── mypackage.spec
├── source.tar.gz
```

Useful for:
- Analyzing how a package is built
- Rebuilding or modifying RPMs
- Inspecting licensing, dependencies, install scripts

### Summary Cheat Sheet

| Command                       | Purpose                                      |
|-------------------------------|----------------------------------------------|
| `dnf history`                 | View/undo/redo past installs & removals      |
| `dnf mark install/remove`     | Control autoremove behavior                  |
| `dnf config-manager`          | Enable/disable/add repos                     |
| `rpm --verify`                | Check integrity of installed files           |
| `rpmdev-extract`              | Unpack `.src.rpm` and `.rpm` for inspection  |

## 8. Managing System Services & Logs

#### Goal: Control services and analyze system logs with **systemctl** and **journalctl**.


### 1. **What is systemd?**

- It’s the **init system** used by RHEL (and most modern distros).
- systemd **manages system services**, startup processes, and dependencies.
- It replaces old-school init scripts (`/etc/init.d/`).

systemctl is the main tool to interact with systemd.


### 2. **Control Services with `systemctl`**

#### Check if a service is active:
```bash
systemctl status sshd
```
output:
```bash
● sshd.service - OpenSSH server daemon
     Loaded: loaded (/usr/lib/systemd/system/sshd.service; enabled; preset: enabled)
     Active: active (running) since [REDACTED]; 52min ago
       Docs: man:sshd(8)
             man:sshd_config(5)
   Main PID: [REDACTED] (sshd)
      Tasks: 1 (limit: 25744)
     Memory: 2.9M
        CPU: 10ms
     CGroup: /system.slice/sshd.service
             └─[REDACTED] "sshd: /usr/sbin/sshd -D [listener] 0 of 10-100 startups"

[REDACTED]: systemd[1]: Starting OpenSSH server daemon...
[REDACTED]: systemd[1]: Started OpenSSH server daemon.
[REDACTED]: sshd[PID]: Server listening on [REDACTED].
```

- **Service Name:** `sshd.service`  
  The SSH Daemon (sshd) allows secure remote login and file transfers.

- **Loaded:**  
  The SSH service is installed, enabled to start automatically, and configured as per system presets.

- **Active:** `active (running)`  
  Currently running normally. The SSH daemon started successfully and has been up for 52 minutes.

- **Main PID:** `941 (sshd)`  
  Process ID (PID) of the main SSH daemon process.

- **Tasks:** `1 (limit: 25744)`  
  SSHD is running with one active task (normal for minimal load).

- **Memory:** `2.9M`  
  Currently using ~3 MB of RAM—standard and lightweight.

- **CPU:** `10ms`  
  Minimal CPU usage (just 10 milliseconds since starting).

- **CGroup (Control Group):**  
  Shows the active SSH process, listening for connections.

- **Logs:**
  ```plaintext
  Server listening on 0.0.0.0 port XX.
  Server listening on :: port XX.
  ```
  Indicates SSH daemon is listening on:
  - IPv4 (`0.0.0.0`)
  - IPv6 (`::`)
  at default port **XX**.


#### Start a service **now**:
```bash
sudo systemctl start sshd
```

#### Stop a service:
```bash
sudo systemctl stop sshd
```

#### Enable service at boot:
```bash
sudo systemctl enable sshd
```

#### Disable service at boot:
```bash
sudo systemctl disable sshd
```

#### Restart a service:
```bash
sudo systemctl restart sshd
```

#### Reload config (without restarting):
```bash
sudo systemctl reload sshd
```

#### Check if enabled at boot:
```bash
systemctl is-enabled sshd
```

> **EX200 test tip:**  
They *love* to ask:  
> "Make sure `sshd` is running and starts on boot."


### 3. **Service Status Summary**

Show running services:
```bash
systemctl list-units --type=service --state=running
```

List all services:
```bash
systemctl list-unit-files --type=service
```


### 4. **System Targets (Runlevels in systemd)**

| Old Runlevel | systemd Target   |
|--------------|------------------|
| 0            | poweroff.target  |
| 1 (single)   | rescue.target    |
| 3            | multi-user.target|
| 5 (GUI)      | graphical.target |
| 6            | reboot.target    |

Check current target:
```bash
systemctl get-default
```

Set boot target:
```bash
sudo systemctl set-default multi-user.target
```

Switch target immediately:
```bash
sudo systemctl isolate rescue.target
```

> Warning: isolate will switch immediately — test carefully!


### 5. **Viewing Logs with `journalctl`**

#### Show all logs:
```bash
journalctl
```

#### Show logs for a service:
```bash
journalctl -u sshd
```

#### Follow live logs (like `tail -f`):
```bash
journalctl -f
```

#### Filter by time:
```bash
journalctl --since today
journalctl --since "2025-03-30 10:00" --until "2025-03-30 12:00"
```

#### Show boot logs:
```bash
journalctl -b
```

### Advanced Pro Stuff

- Persistent logs stored in `/var/log/journal/`
- Filter logs by priority:
```bash
journalctl -p err
journalctl -p warning
journalctl -p crit
```
- View logs for previous boots:
```bash
journalctl -b -1
```

## 9. Networking with `nmcli`, `ip`, and Network Troubleshooting

#### Goal: Configure and manage network interfaces, verify connectivity, and troubleshoot.


#### 1. Network Manager Overview

- **NetworkManager** is the service managing interfaces in RHEL.
- We control it with:
  - `nmcli` (command line tool)
  - `nmtui` (text user interface)
  - config files: `/etc/NetworkManager/system-connections/`

Check if NetworkManager is running:
```bash
systemctl status NetworkManager
```


#### 2. Configure Network Interfaces with `nmcli`

##### Show all connections:
```bash
nmcli connection show
```

##### Show active devices:
```bash
nmcli device status
```

##### Show connection details:
```bash
nmcli connection show <connection-name>
```

##### Add a new connection (static IP):
```bash
sudo nmcli con add con-name my-ethernet ifname eth0 type ethernet ip4 1XX.1XX.1.1XX/24 gw4 1XX.1XX.1XX.1
```

##### Set DNS:
```bash
sudo nmcli con mod my-ethernet ipv4.dns "8.8.8.8 8.8.4.4"
```

##### Bring connection up/down:
```bash
sudo nmcli con up my-ethernet
sudo nmcli con down my-ethernet
```

#### 3. `nmtui` — Easy Visual TUI

If you like menus, try:
```bash
sudo nmtui
```

From here you can:
- Edit connections
- Set IP addresses
- Set DNS
- Activate/deactivate interfaces


#### 4. Verify & Troubleshoot with `ip` and friends

##### Show interfaces:
```bash
ip a
```

##### Show routing table:
```bash
ip route
```

##### Test connectivity:
```bash
ping -c 4 8.8.8.8
```

##### Test name resolution:
```bash
ping www.redhat.com
```

If IP works but name fails, it's **DNS problem**.


#### 5. Troubleshoot:

| Problem               | Command to Diagnose                          |
|-----------------------|----------------------------------------------|
| No IP Address         | `ip a`                                       |
| Interface down        | `nmcli device status`                        |
| Wrong gateway         | `ip route`                                   |
| DNS fails             | Check `/etc/resolv.conf` or `nmcli con show` |
| Service not listening | `ss -tuln` or `netstat -tuln`                |
| Firewall blocking     | `firewall-cmd --list-all`                    |



#### Pro Notes

- Network configs are persistent **if you use `nmcli con mod` or `nmtui`**.
- Manual `ip a` settings are *temporary* (until reboot).
- DNS config is saved by NetworkManager, but old tools (like editing `/etc/resolv.conf`) are overwritten.


## 10. Managing Time and Date

####  Goal: Configure system time, timezone, NTP time sync, and check accuracy.


### 1. Check Current Time and Settings

```bash
timedatectl status
```
output:
```bash
               Local time: Fri 2025-04-04 17:32:40 CEST
           Universal time: Fri 2025-04-04 15:32:40 UTC
                 RTC time: Fri 2025-04-04 15:32:40
                Time zone: Europe/Berlin (CEST, +0200)
System clock synchronized: yes
              NTP service: active
          RTC in local TZ: no
```

You’ll see:
- Local time
- Universal time (UTC)
- RTC time (hardware clock)
- Time zone
- NTP status


### 2. Set Time Zone

List available time zones:
```bash
timedatectl list-timezones
```

Set time zone:
```bash
sudo timedatectl set-timezone Europe/Berlin
```

Check:
```bash
timedatectl status
```

> Set UTC for servers, local time for desktops. UTC prevents time drift in global systems.


### 3. Manually Set the Time (Rare, but good to know)

```bash
sudo timedatectl set-time "2025-03-31 22:00:00"
```

> Manual time setting disables automatic NTP sync!


### 4. Enable/Disable NTP Synchronization

Check if NTP is active:
```bash
timedatectl status
```

Enable NTP:
```bash
sudo timedatectl set-ntp true
```

Disable NTP:
```bash
sudo timedatectl set-ntp false
```

### 5. Check NTP Sync Source

See which NTP servers you’re syncing with:
```bash
chronyc sources
```

If `chronyc` is missing:
```bash
sudo dnf install chrony
sudo systemctl enable --now chronyd
```

### 6. Persistent Time Configs

- Time zone config: `/etc/localtime` (symlink to zoneinfo)
- NTP config: `/etc/chrony.conf`

View `chrony.conf`:
```bash
cat /etc/chrony.conf
```


### Summary

| Task              | Command                               |
|-------------------|---------------------------------------|
| Show current time | `timedatectl status`                  |
| List time zones   | `timedatectl list-timezones`          |
| Set time zone     | `sudo timedatectl set-timezone <zone>`|
| Enable NTP        | `sudo timedatectl set-ntp true`       |
| Check NTP sources | `chronyc sources`                     |


## 11. Storage Management (Partitions, Filesystems, Mounting)

#### Understand devices, partitions, filesystems, and how to mount them (temporarily and permanently).


### 1. Understand Devices & Partitions

#### List all storage devices:
```bash
lsblk
```

Example output:
```bash
NAME   MAJ:MIN RM SIZE RO TYPE MOUNTPOINT
sda      8:0    0  20G  0 disk 
├─sda1   8:1    0   1G  0 part /boot
└─sda2   8:2    0  19G  0 part /
```

- `sda` → whole disk
- `sda1`, `sda2` → partitions
- `TYPE` can be `disk`, `part`, `lvm`, etc.


#### Detailed info:
```bash
sudo fdisk -l
```

#### Use `blkid` for UUID and filesystem info:
```bash
sudo blkid
```

### 2. Partitioning Disks (with `fdisk`)

> Warning: **Do this only on empty or test disks in your VM.**

```bash
sudo fdisk /dev/sdb
```

Inside fdisk:
- `n` → new partition
- `p` → primary
- `w` → write changes
- `q` → quit without saving


### 3. Create Filesystem

After creating the partition (example: `/dev/sdb1`):

#### Make ext4 filesystem:
```bash
sudo mkfs.ext4 /dev/sdb1
```

Other types:
```bash
sudo mkfs.xfs /dev/sdb1    # Default RHEL fs
sudo mkfs.vfat /dev/sdb1   # FAT32 (USB drives)
```

#### Check filesystem:
```bash
sudo file -s /dev/sdb1
```


### 4. Mount Storage

#### Create a mount point:
```bash
sudo mkdir /mnt/data
```

#### Mount the partition:
```bash
sudo mount /dev/sdb1 /mnt/data
```

#### Check mounted filesystems:
```bash
mount | grep /mnt/data
df -h
```

### 5. Make Mount Persistent (Edit `/etc/fstab`)

Get the UUID:
```bash
sudo blkid /dev/sdb1
```

Edit `/etc/fstab`:
```bash
sudo nano /etc/fstab
```

Add line:
```
UUID=xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx /mnt/data ext4 defaults 0 0
```

> Test:
```bash
sudo mount -a
```

If no errors, it’s good.

## Pro Notes

- Red Hat prefers **XFS** filesystem in RHEL 8+ (supports journaling, good performance).
- Always back up `/etc/fstab` before editing:
```bash
sudo cp /etc/fstab /etc/fstab.bak
```
- Use `mount -a` to test **before** reboot.
- If you mess up `fstab`, use **rescue mode** to fix it.

## 12. SELinux & Firewalld


### Part 1: SELinux — Security-Enhanced Linux

#### What is SELinux?

SELinux controls **access to files, processes, and ports** at a deeper level than normal file permissions.

Even if file permissions are open, **SELinux policy can block access.**

It uses:
- **Labels** (contexts) on files and processes
- **Policies** to control access

#### Check SELinux Status:
```bash
sestatus
```

Output:
```bash
SELinux status:                 enabled
SELinuxfs mount:                /sys/fs/selinux
SELinux root directory:         /etc/selinux
Loaded policy name:             targeted
Current mode:                   enforcing
Mode from config file:          enforcing
Policy MLS status:              enabled
Policy deny_unknown status:     allowed
Memory protection checking:     actual (secure)
Max kernel policy version:      33
```

#### Modes of SELinux:

| Mode           | What it does                                      |
|----------------|---------------------------------------------------|
| **Enforcing**  | SELinux enforces policies (blocks access)         |
| **Permissive** | SELinux logs violations but allows access         |
| **Disabled**   | SELinux is turned off (needs reboot to re-enable) |

#### Change mode temporarily:
```bash
sudo setenforce 0   # permissive
sudo setenforce 1   # enforcing
```

**Note**: This does not survive reboot.

#### Change mode permanently:
Edit config:
```bash
sudo nano /etc/selinux/config
```
Set:
```bash
SELINUX=enforcing
```


#### Check File Context:
```bash
ls -Z /var/www/html/
```

#### Restore default contexts:
If SELinux is blocking access because of wrong context:
```bash
sudo restorecon -Rv /var/www/html/
```

#### Manage SELinux Ports:
List ports:
```bash
semanage port -l | grep http
```

Allow Apache on port 8080:
```bash
sudo semanage port -a -t http_port_t -p tcp 8080
```

>  `semanage` is part of **policycoreutils-python-utils**, install it if needed:
```bash
sudo dnf install policycoreutils-python-utils
```

#### Pro Note:
**Audit logs** for SELinux denials:
```bash
sudo cat /var/log/audit/audit.log | grep AVC
```

### Part 2: Firewalld — Firewall Management

#### What is firewalld?

Firewalld uses **zones** to manage access to services and ports dynamically.

#### Check firewalld status:
```bash
sudo systemctl status firewalld
```

#### List current zone:
```bash
sudo firewall-cmd --get-default-zone
```

#### List all zones:
```bash
sudo firewall-cmd --list-all-zones
```

#### List open ports/services:
```bash
sudo firewall-cmd --list-all
```

#### Add service (temporary):
```bash
sudo firewall-cmd --add-service=http
```

#### Add service (permanent):
```bash
sudo firewall-cmd --add-service=http --permanent
sudo firewall-cmd --reload
```

#### Add custom port:
```bash
sudo firewall-cmd --add-port=8080/tcp --permanent
sudo firewall-cmd --reload
```

#### Remove service:
```bash
sudo firewall-cmd --remove-service=http --permanent
sudo firewall-cmd --reload
```

### Pro Notes

> *Allow web server to run on custom port with SELinux and firewalld configured.*

- Test your work:
```bash
sudo curl -I http://localhost:8080
```

- Permanent changes need **`--permanent`** and **reload**:
```bash
sudo firewall-cmd --reload
```

## 13. Boot Process & Troubleshooting (GRUB, Targets, Recovery)

### Goal: Understand the Linux boot process, manage bootloaders, system targets, and recover systems when they fail to boot.


### 1. Understand the Boot Process

| Step          | Description                                  |
|---------------|----------------------------------------------|
| **BIOS/UEFI** | Hardware initializes, locates bootloader     |
| **GRUB2**     | Bootloader — lets you choose kernel/initrd   |
| **Kernel**    | Loads system hardware and mounts rootfs      |
| **initramfs** | Initial temporary root filesystem            |
| **systemd**   | PID 1 process starts user space and services |

Check kernel version:
```bash
uname -r
```

View boot logs:
```bash
journalctl -b
```

### 2. GRUB2 — Bootloader

GRUB2 config lives at:
```bash
/etc/default/grub
```

View default boot target:
```bash
sudo grubby --default-kernel
```

#### Edit kernel boot parameters **temporarily**:
At boot:
- Press `e` in GRUB menu
- Modify kernel line (e.g., add `single`, or `rd.break` for rescue shell)
- Boot with `Ctrl + X`

#### Regenerate GRUB config:
```bash
sudo grub2-mkconfig -o /boot/grub2/grub.cfg
```

### 3. Rescue & Emergency Targets

| Target               | Purpose                                               |
|----------------------|-------------------------------------------------------|
| **rescue.target**    | Single-user mode with basic services                  |
| **emergency.target** | Minimal shell, no services (useful for fsck, repairs) |

Boot into rescue mode:
```bash
sudo systemctl isolate rescue.target
```

Check default target:
```bash
systemctl get-default
```

Change default boot target:
```bash
sudo systemctl set-default multi-user.target
sudo systemctl set-default graphical.target
```

### 4. Recover Lost Root Password (EX200 loves this!)

Steps:
1. Reboot system.
2. At GRUB menu, press **e**.
3. Find the kernel line (`linux16 ...`)
4. Add at end:
```bash
rd.break
```
In RHEL 9
```bash
rd.break enforcing=0
```

5. Press **Ctrl + X** to boot.  
>It will drop into:
```bash
switch_root:/#
```
6. Remount `/sysroot` as writable:
```bash
mount -o remount,rw /sysroot
```
7. Switch to chroot:
```bash
chroot /sysroot
```
8. Set new root password:
```bash
passwd root
```
9. Re-label SELinux (important!):
```bash
touch /.autorelabel
```
10. Exit and reboot:
```bash
exit
exit
```

> System reboots and you’re back!

### Pro Tips for RHCSA

- RHCSA exams *love* testing:
  - Rescue mode
  - Boot targets
  - Forgot password recovery
  - View boot logs
- Make sure you **practice `rd.break`**!
- `/etc/default/grub` changes need **regeneration** with `grub2-mkconfig`.
- Always check SELinux relabeling with `touch /.autorelabel` when fixing root password.

