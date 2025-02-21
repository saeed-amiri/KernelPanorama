# Process Scheduling:
The Linux kernel manages multiple processes and threads running simultaneously. The scheduler decides:
- Which process gets CPU time?
- When a process should be paused (preempted)?
- How to fairly allocate CPU resources among multiple tasks?
- How to prioritize real-time and time-sharing process?

---

## What is the Linux Process Scheduler?
- The Linux scheduler is responsible for choosing which process runs next on the CPU.
- It balances:
    * **Fairness:** every process gets CPU time,
    * **Efficiency:** keeps the system responsive,
    * **Priorities:** higher-priority tasks should not starve others.

---

## Linux Scheduling Policies:
The Linux scheduling supports multiple scheduling policies, as:
1. **Normal Scheduling (Time-Sharing)** Used for regular processes (background tasks, applications):
    - `SCHED_OTHER`: Default scheduling policy (Completely Fair Scheduler, CFS),
    - `SCHED_BATCH`: Optimized for batch jobs (low priority, background tasks),
    - `SCHED_IDLE`: For externally low priority background tasks.
2. **Real-Time Scheduling (For Critical Tasks)** used for real-time applications that require strict timing:
    - `SCHED_FIFO`: First-In, First-Out runs until preempted by a higher-priority task,
    - `SCHED_RR`: Round-Robin, gives time slices and rotates,
    - `SCHED_DEADLINE`: Guarantee a process runs before a given deadline.

**How to check the scheduling policy:**
```bash
chrt -p <PID>
```
for example if do:
```bash
top
```
the output is:
```bash
   3151 holden    20   0 1160.1g 365092 129620 S   3.9   4.5   4:50.94 code     
```
so if we get the policies:
```bash
chrt -p 3151

pid 3151's current scheduling policy: SCHED_OTHER
pid 3151's current scheduling priority: 0
```
---

## How the Completely Fair Scheduler (CFS) Works?
#### Virtual Runtime (VRuntime):
   VRuntime is a per-process counter that tracks how much CPU time a process has used:
   - Every process gets a virtual runtime.
   - The lower the VRuntime, the higher the priority.
   - The scheduler always picks the process with the smallest VRuntime to run next.

Instead of using priority based queues, CFS assigns every process a "virtual runtime" and tries to distribute CPU time as fairly as possible:
* Uses a Read-Black Tree (RBT) for fast process selection,
    - "*In computer science, a red–black tree is a self-balancing binary search tree data structure noted for fast storage and retrieval of ordered information*" [wiki].
* Prioritizes interactive tasks over CPU0heavy tasks,
* Maintains system responsiveness.
For checking it:
```bash
cat /proc/<PID>/sched
```
which for pid `3151` gives:
```bash
code (3151, #threads: 16)
-------------------------------------------------------------------
se.exec_start                                :       4616477.960378
se.vruntime                                  :        130686.676730
se.sum_exec_runtime                          :        214153.153922
se.nr_migrations                             :                 3746
nr_switches                                  :               169732
nr_voluntary_switches                        :               156139
nr_involuntary_switches                      :                13593
se.load.weight                               :              1048576
se.avg.load_sum                              :                 9237
se.avg.runnable_sum                          :              9466958
se.avg.util_sum                              :              9223216
se.avg.load_avg                              :                  200
se.avg.runnable_avg                          :                  200
se.avg.util_avg                              :                  195
se.avg.last_update_time                      :        4616477960192
se.avg.util_est                              :                  181
uclamp.min                                   :                    0
uclamp.max                                   :                 1024
effective uclamp.min                         :                    0
effective uclamp.max                         :                 1024
policy                                       :                    0
prio                                         :                  120
clock-delta                                  :                  110
mm->numa_scan_seq                            :                    0
numa_pages_migrated                          :                    0
numa_preferred_nid                           :                   -1
total_numa_faults                            :                    0
current_node=0, numa_group_id=0
numa_faults node=0 task_private=0 task_shared=0 group_private=0 group_shared=0
```

## Process Priorities (Nice Values):
Each process has a `nice` value (-20 to +19) that affects its CPU priority:
    - Lower `nice` value means higher priority, and visa versa.

one can change the process priority:
```bash
nice -n -10 command
renice -n 5 -p <PID>
```

---

## Preemption: How the Kernel Interrupts Processes:
Preemption means the kernel can pause a running process to switch to another:
- Involuntary preemption: When a process exceeds its time slice,
- Voluntary preemption: When a process block on I/O or a system call.

---

## Mutlicore Scheduling: How Linux Uses Multiple CPUS:
The Linux scheduler balances across CPU core:
- **Load balancing** which tries to evenly distribute work.
- **Processes Affinity** which can bind a process to a specific CPU.

To check which CPU a process is ruining on:
```bash
taskset -p <PID>
```
for 3151 it is:
```bash
pid 3151's current affinity mask: ff
```
which means:
- `current affinity mask: ff`: This represents the CPU affinity mask in hexadecimal.
- `ff` is a hexadecimal number, converting it to binary: `ff (hex) = 11111111 (binary)`
    * Each `1` represents an allowed CPU core.
    * This means the process can run on all 8 CPU cores (from `CPU 0` to `CPU7`),
    * For a 16-core system, the `mask` would likely be `fff`.
- `affinity`:
    * CPU `affinity` controls which CPU cores a process allowed to run in,
    * One can restrict a process to a specific CPUs using `taskset`,
    * For example, to make process `3151` run only on CPUs 0 and 1:
    ```bash
    taskset -p 3 3151
    ```
    * Where `3` is hex `11` in binary, meaning CPU 0 and CPU1.

---
