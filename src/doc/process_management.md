# Process Management in the Kernel
The kernel creates, schedules, and manages processes,ensuring that multiple programs can run efficiently without interfering with each other.

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
    struct signal_struct * signal;  // Signal handeling info
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
or by checking a specific <PID>:
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