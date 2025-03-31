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

## `dnf`

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
