# Process Management in the Kernel
The kernel creates, schedules, and manages processes, ensuring that multiple programs can run efficiently without interfering with each other.

## Key Responsibilities of the Kernel in Process Management are:
- Processes Creation (`fork()`, `execve()`)
- Process States & Transitions
- Scheduling (CFS, RT, FIFO)
- Process Termination  and signals
- Inter-Process Communication (IPC, Pipes, Sockets, Shared Memory)

## Process Structure in the Kernel:
Every running process in Linux is represented by a `task_struct` in the kernel:
```c
// Defined in the Linux kernel: include/linux/sched.h
struct task_struct {
    pid_t pid;                      // Process ID
    pid_t tgid;                     // Thread Group ID
    long state;                     // Process state (running, sleeping, zombie, etc.)
    struct mm_struct *mm;           // Memory management info
    struct list_head tasks;         // List of processes (for scheduling)
    struct task_struct *parent;     // Pointer to parent process
    struct files_struct *files;     // Open file descriptors
    struct signal_struct *signal;   // Signal handeling info
}
```
This structure stores everything about a process: its ID, memory, open files, parent process and scheduling state.

## Process States:
A process in Linux can be in different states, depending on what it's doing:
|state               | Description                                               |
|--------------------|-----------------------------------------------------------|
|TASK_RUNNING        | Actively running or ready to run                          |
|TASK_INTERRUPTIBLE  | Sleeping, but wakes on signals                            |
|TASK_UNINTERRUPTIBLE| Sleeping, but cannot be interrupted                       |
|TASK_STOPPED        | Process terminated (e.g., `SIGSTOP`)                      |
|TASK_ZOMBIE         | Process terminated, but parent hasn't collected its status|

Once can get the process by:
```bash
ps -eo pid,state, cmd | head

    PID S CMD
      1 S /sbin/init splash
      2 S [kthreadd]
      3 S [pool_workqueue_release]
      4 I [kworker/R-rcu_g]
      5 I [kworker/R-rcu_p]
      6 I [kworker/R-slub_]
      7 I [kworker/R-netns]
      9 I [kworker/0:0H-events_highpri]
     12 I [kworker/R-mm_pe]
```
or by checking a specific `<PID>`:
```bash
cat /proc/12/status
Name:   kworker/R-mm_pe
Umask:  0000
State:  I (idle)
Tgid:   12
Ngid:   0
Pid:    12
PPid:   2
TracerPid:      0
Uid:    0       0       0       0
Gid:    0       0       0       0
FDSize: 64
Groups:  
NStgid: 12
NSpid:  12
NSpgid: 0
NSsid:  0
Kthread:        1
Threads:        1
SigQ:   1/31128
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: ffffffffffffffff
SigCgt: 0000000000000000
CapInh: 0000000000000000
CapPrm: 000001ffffffffff
CapEff: 000001ffffffffff
CapBnd: 000001ffffffffff
CapAmb: 0000000000000000
NoNewPrivs:     0
Seccomp:        0
Seccomp_filters:        0
Speculation_Store_Bypass:       thread vulnerable
SpeculationIndirectBranch:      conditional enabled
Cpus_allowed:   ff
Cpus_allowed_list:      0-7
Mems_allowed:   00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001
Mems_allowed_list:      0
voluntary_ctxt_switches:        2
nonvoluntary_ctxt_switches:     0
x86_Thread_features:
x86_Thread_features_locked:
```

## Process Creation (`fork()`, `execev()`):
Linux creates new processes using:
* `fork()` which creates a copy of the current process,
* `execve()` which replaces the process image with a new program.

Example of process creation in user space:
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child Process: PID = %d\n", getpid());
    } else {
        printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), pid);
    }
    return 0;
}
```
Kernel-side Process Creation (`do_fork()`) internally, `fork()` calls:
- `do_fork()` (kernel function in `kernel/fork.c`)
- Allocates a new process context (memory, file descriptors)
- Returns new process ID (PID)

## Process Scheduling (How the Kernel Chooses Which Process Runs):

Linux schedules processes using the Completely Fair Scheduler (CFS)

|Scheduler Type          | Description                             |
|------------------------|-----------------------------------------|
|CFS (Default)           | First CPU time for all processes        |
|FIFO (Real-Time)        | First-in, first-out real-time scheduling|
|Round Robin (Real-Time) | Each process gets a time slice          |
|Nice values             | Lower `nice` values = Higher priority   |

For checking Process Scheduling Information:
```bash
chrt -p <PID>  # Check process priority
chrt -p  12


pid 12's current scheduling policy: SCHED_OTHER
pid 12's current scheduling priority: 0
```
- `SCHED_OTHER` is a default schedule and uses time-sharing (CFS) (others are `SCHED_FIFO` and `SCHED_RR`)
- `priority: 0` In SCHED_OTHER, priority is managed dynamically by the kernel and does not have a fixed numerical priority like real-time policies.

The process priority can be set by `nice`:
```bash
nice -n -10 ./program   # Run with high priority
```
negative number also can be use like `--10`, and it can be undone be `renice`

## Process termination and Zombie Processes
When a process ends, it moves to **TASK_ZOMBIE** until parent calls `waitpid()`

To create a zombie process:
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child Process: PID = %d\n", getpid());
        exit(0); // Child exits
    }
    sleep(10);  // Parent does not call wait(), so child becomes zombie
    return 0;
}
```
To check Zombie Process:
```bash
ps aux | grep Z

or

ps aux | awk '$8 ~ /Z/ {print $0}'
```

and to fix (kill) a Zombie process
```bash
kill -SIGCHD <Parent PID>
```

## Signals and Process Control (`kill`, `SIGKILL`, `SIGTERM`):
A signal is an event notification system used by the kernel:
|Signal     | Meaning                    | Default Action        |
|-----------|----------------------------|-----------------------|
|SIGKILL(9) | Immediately kill process    | Cannot be ignored     |
|SIGTERM(15)| Gracefully terminate       | Default for `kill`    |
|SIGSTOP(19)| Stop process execution     | Can be resumed        |
|SIGCONT    | Resume the paused execution|                       |

Send Signals:
```bash
kill -SIGTERM <PID>  # Ask process to terminate
kill -SIGKILL <PID>  # Force kill
kill -SIGSTOP <PID>  # Pause process
kill -SIGCONT <PID>  # Resume process
```
Trap a signal in code:
```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void handle_sigint(int sig) {
    printf("Caught SIGINT (%d), exiting...\n", sig);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    while (1) {
        printf("Running... Press Ctrl+C to exit.\n");
        sleep(1);
    }
    return 0;
}
```