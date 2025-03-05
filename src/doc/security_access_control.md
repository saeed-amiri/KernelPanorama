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

