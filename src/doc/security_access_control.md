# Security and Access Control in Linux

Understanding how Linux manages **security** is crucial for **controlling user permissions**, **restricting access**, and enforcing **security policies**. This document covers:
- Linux capabilities,
- Access control mechanisms,
- Security features,
- Practical tools for securing the system.

## 1. Understanding Linux Capabilities
Traditionally, the `root` user has unlimited system access. Linux **capabilities** break down root privileges into smaller, manageable units, allowing processes to have only the permissions they need.

### Checking the Capability:
To see the capabilities of a binary:
```bash
getcap /bin/ping
```
Examples output:
```bash
/bin/ping = cap_net_raw+ep
```
This means `ping` has `cap_net_raw` enabled, allowing it to create raw **network packets**.

or
```bash
/bin/ping = cap_net_raw+ep
```
The difference between `=` and `+` in `getcap` output is:
- `=` (Assignment Operator): Shows the exact capabilities assigned to the binary,
- `+` (Modification Indicator): Denotes that the capabilities include the **effective** (`e`) set, meaning they are **immediately usable** without requiring `exec()`.

|Flag   |	Meaning                                                   |
|-------|-------------------------------------------------------------|
|`e`    | Effective - Process can use the capability immediately.     |
|`p`    | Permitted - Process may gain this capability if needed.     |
|`i`    | Inheritable - Capability can be passed to child processes.  |

**Some definitions [[wiki](https://en.wikipedia.org/wiki/Network_packet#Examples)]:**
- In telecommunications and computer networking, a **network packet** is a formatted unit of data carried by a **packet-switched** network. A packet consists of control information and user data.
- In telecommunications, **packet switching** is a method of grouping data into short messages in fixed format, i.e. packets, that are transmitted over a digital network. Packets are made of a **header** and a **payload**.
- In information technology, **header** is supplemental data placed at the beginning of a block of data being stored or transmitted.
- In computing and telecommunications, the **payload** is the part of transmitted data that is the actual intended message. Headers and metadata are sent only to enable payload delivery and are considered overhead.
In the context of a computer virus or worm, the payload is the portion of the malware which performs malicious action.  The term is borrowed from transportation, where payload refers to the part of the load that pays for transportation.

#### Assigning Capabilities to Binaries
Instead of running a program as root, we can grant it specific capabilities:
```bash
sudo setcap cap_net_bind_service=+ep /usr/bin/nc
```
`netcat` (`nc`) normally requires root privileges to bind to ports below `1024`, but assigning `cap_net_bind_service` allows it to do so without full root access.

**Privileged ports** (`0-1023`) require administrative rights because they are used by critical network services. The system enforces this restriction to prevent unauthorized applications from impersonating essential services like `SSH` (port `22`) or HTTPS (port `443`). By contrast, **non-privileged ports** (`1024-65535`) are available for general use by regular users and applications.

**Some definitions [[wiki](https://en.wikipedia.org/wiki/Netcat)]:**
- `netcat` (often abbreviated to `nc`) is a computer networking utility for reading from and writing to network connections using **TCP** or **UDP**.

- The **Transmission Control Protocol (TCP)** is one of the main protocols of the Internet protocol suite.

- In computer networking, the **User Datagram Protocol (UDP)** is one of the core communication protocols of the Internet protocol suite used to send messages (transported as datagrams in packets) to other hosts on an **Internet Protocol (IP)** network.

- The **Internet Protocol (IP)** is the network layer communications protocol in the Internet protocol suite for relaying datagrams across network boundaries.

- **Privileged ports [[W3](https://www.w3.org/Daemon/User/Installation/PrivilegedPorts.html)]:**
    * In **TCP/IP networking**, port numbers below **1024** are classified as **privileged ports**, meaning that only users with administrative or root privileges can operate servers on these ports. This restriction enhances security by ensuring that services running on these ports are legitimate and not unauthorized replicas set up by malicious actors.

    * **Common Ports and Their Usage:**
        - The standard port for **WWW (World Wide Web) servers** is **port 80**, a designation assigned by the **Internet Assigned Numbers Authority (IANA)**.
        - When testing a server without elevated privileges, alternative ports such as **2784**, **5000**, **8001**, or **8080** are often used.
    * **Port Access Across Operating Systems:**
        - **Unix Systems**
            * On **Unix**, the **inetd daemon** (which runs as `root`) can monitor incoming requests on **port 80** and forward them to a server process with a lower-privileged user ID for security reasons.
            * More recent versions of **httpd (2.14 and later)** can safely operate with root permissions initially but later transition to a non-privileged user, such as `nobody`, for security.
        - **VMS Systems:**
            * In **VMS environments**, especially when using **UCX**, a server process needs **BYPASS** privileges to access ports below **1024**. In some cases, installation of the server may be necessary to grant these permissions.
            * Other **TCP/IP** implementations on **VMS** require similar elevated privileges for operating on restricted ports.

To remove a capability:
```bash
sudo setcap -r /usr/bin/nc
```
## 2. Access Control Mechanisms:
Linux provides various access control mechanisms beyond **standard UNIX file permissions**.

### **Standard UNIX File Permissions**

UNIX file permissions control how users and groups can **read**, **write**, and **execute** files and directories. Each file has three permission categories:

| Permission Type | Symbol | Numeric Value |
|---------------|--------|---------------|
| **Read**      | `r`    | `4` |
| **Write**     | `w`    | `2` |
| **Execute**   | `x`    | `1` |

Each file and directory in UNIX has three sets of these permissions:

| Category   | Description |
|------------|-------------|
| **Owner**  | The user who owns the file    |
| **Group**  | Users in the file's group     |
| **Others** | All other users on the system |

#### **Viewing File Permissions**
To check the permissions of a file or directory:
```bash
ls -l myfile.txt
```
Example output:
```bash
-rwxr-xr--  1 user group 1234 Mar 5 12:34 myfile.txt
```
**Breaking it down:**
- `-` → Regular file (`d` would indicate a directory).
- `rwx` → **Owner** has **read, write, execute** (`r=4`, `w=2`, `x=1`, total = `7`).
- `r-x` → **Group** has **read and execute** (`r=4`, `x=1`, total = `5`).
- `r--` → **Others** have **read-only** (`r=4`, total = `4`).

---

### **Changing File Permissions (`chmod`)**
You can modify file permissions using `chmod` with:
1. **Symbolic Mode**  
2. **Octal (Numeric) Mode**

#### **Symbolic Mode**
To **add execute** permission for the owner:
```bash
chmod u+x myfile.txt
```
To **remove write** permission for the group:
```bash
chmod g-w myfile.txt
```
To **give everyone execute** permission:
```bash
chmod a+x myfile.txt
```

#### **Octal (Numeric) Mode**
Each permission type corresponds to a number:
- **Read (r) = 4**
- **Write (w) = 2**
- **Execute (x) = 1**

Combine these to set permissions:
```bash
chmod 754 myfile.txt
```
- **7 (Owner) → rwx** (4+2+1)
- **5 (Group) → r-x** (4+0+1)
- **4 (Others) → r--** (4+0+0)

---

### **Changing File Ownership (`chown` & `chgrp`)**
#### **Changing Owner**
```bash
sudo chown newuser myfile.txt
```
#### **Changing Group**
```bash
sudo chgrp newgroup myfile.txt
```
#### **Changing Both Owner & Group**
```bash
sudo chown newuser:newgroup myfile.txt
```

---

### **Special Permissions**
#### **Setuid (`s`)**
Allows users to execute a file **as the owner**:
```bash
chmod u+s myscript.sh
```
Now, when a user runs `myscript.sh`, it executes as the file's owner.

#### **Setgid (`s`)**
For directories, new files inherit the **group ownership**:
```bash
chmod g+s mydir/
```

#### **Sticky Bit (`t`)**
Prevents users from deleting files they don’t own (common for `/tmp`):
```bash
chmod +t /tmp
```
### **Difference Between `chmod`, `chown`, and `chgrp`**

| Command  | Purpose                                            | Example                                   |
|----------|----------------------------------------------------|-------------------------------------------|
| **`chmod`**  | Changes file **permissions** (read, write, execute) | `chmod 755 myfile.txt` (Owner: rwx, Group: r-x, Others: r-x) |
| **`chown`**  | Changes **file ownership** (who owns the file) | `sudo chown user1 myfile.txt` (Now `user1` owns the file) |
| **`chgrp`**  | Changes the **group ownership** of a file | `sudo chgrp group1 myfile.txt` (Now the group is `group1`) |

### **Key Differences**
- **`chmod`** modifies access rights (**who can read/write/execute**).
- **`chown`** assigns a new **owner**.
- **`chgrp`** assigns a new **group** (without changing the owner).

### **Combined Example**
```bash
sudo chown user1:group1 myfile.txt  # Change owner to `user1` and group to `group1`
chmod 750 myfile.txt                # Owner: rwx, Group: r-x, Others: no access
```
Now:
- **user1** can **read/write/execute**.
- **group1 members** can **read/execute**.
- **Others** have **no access**.

---

### POSIX Access Control Lists (ACLs):

ACLs allow more granular permission settings than the traditional `chmod` system.

**Checking ACLs:**
```bash
getfacl myfile.text
```
Example output:
```bash
# file: myfile.txt
# owner: user
# group: group
user::rw-
group::r--
other::---
user:john:rwx
```

This file allows an additional user (john) to have *read-write-execute* (`rwx`) permissions.

**Seting ACLs:**
```bash
setfacl -m u:john:r myfile.txt
```
`setfacl`: set file ACLs

**Remove ACL entry:**
```bash
setfacl -x u:john:r myfile.text
```
* `u`: Specifies that the ACL is being set for a user (john in this case).
* `r`: Grants read permission to john.

#### AppArmor and SELinux
These are **Mandatory Access Control (MAC)** frameworks that enforce security policies beyond traditional UNIX permissions, ensuring strict access control and limiting potential exploits:
- *The **Security-Enhanced Linux (SELinux)** is a security architecture for Linux® systems that allows administrators to have more control over who can access the system. **SELinux** defines access controls for the applications, processes, and files on a system [[RedHat](https://www.redhat.com/en/topics/linux/what-is-selinux)].*
- *The **AppArmor** [as an Linux kernel security module] is an effective and easy-to-use Linux application security system. AppArmor proactively protects the operating system and applications from external or internal threats, even zero-day attacks, by enforcing good behavior and preventing both known and unknown application flaws from being exploited [[apparmor](https://apparmor.net/)].*


**Checking AppArmor Status:**
```bash
sudo aa-status
```
Example output:
```bash
apparmor module is loaded.
5 profiles are in enforce mode.
```

To temporarily switch **SELinux** to permissive mode:
```bash
sudo setenforce 0
```
To permanently disable it:
```bash
sudo nano /etc/selinux/config
# Change 'SELINUX=enforcing' to 'SELINUX=disabled'
```
