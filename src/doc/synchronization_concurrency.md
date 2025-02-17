# Synchronization & Concurrency:
The Linux Kernel runs multiple processes and kernel threads at the same time. If two processes try to modify the same resource (memory, files, devices) the at the same time, race conditions, deadlocks, and data corruption can happen.

**Key Questions:**
- How does the kernel prevent race conditions?
    * Race condition: A race condition or race hazard is the condition of an electronics, software, or other system where the system's substantive behavior is dependent on the sequence or timing of other uncontrollable events, leading to unexpected or inconsistent results [wiki]. 

- How do multiple CPUs safely modify shared data?
- How does the kernel avoid deadlocks?
- What locking mechanisms does the Linux kernel use?
---


## **Lock**
A lock is a synchronization mechanisms used to prevent multiple processes or threads from modifying shared resources at the same time. Without lock, race conditions can occur, leading to data corruption, unpredictable behavior, or crashes.

Detect Deadlocks:
```bash
cat /proc/locks

or 

sudo dmesg | grep lock
```
---

## 1. What is Concurrency in the Kernel?
Concurrency happens when multiple processes or threads execute simultaneously. In the Linux kernel, concurrency occurs due to:
-   Preemptive scheduling -> The kernel can interrupt one process and switch to another.
-   Interrupts (IRQ) -> The kernel can pause execution to handle hardware events.
-   Multi-core CPUs -> multiple CPU cores run kernel code at the same time.

**Example of a Race Condition:**
When two processes modifying a global variable:
```c
int counter = 0;

void process_func(void) {
        counter++;  // Process 1 reads, modifies, and writes countor
        counter++;  // Process 2 does the same
}
```
If both processes run at the same time, `counter` might get incremented only once instead of twice!

---

## 2. Kernel Synchronization Mechanisms:
Linux uses several mechanisms to prevent concurrency issues [wiki, IBM, ScienceDirect]:

|Mechanism     | Uses                                           | Example API       |
|--------------|------------------------------------------------|-------------------|
| Spinlocks    | **Short critical sections:** a lock that causes a thread trying to acquire it to simply wait in a loop ("spin") while repeatedly checking whether the lock is available. | `spin_lock()` / `spin_unlock()`|
|Mutexes       | **Blocking locks for threads:** A mutex is a *mutual exclusion lock*. Only one thread can hold the lock. Mutexes are used to protect data or other resources from concurrent access. A mutex has attributes, which specify the characteristics of the mutex.| `mutex_lock()` / `mutex_unlock` |
|Semaphores    | **Multiple threads access control:** is merely a plain variable that is changed, indicating the status of the common resource. Semaphores can be seen as a resource-sharing mechanism, where you have a finite instance of a resource that you want to manage access for multiple threads. | `down()` / `up()` |
|Read-Write Locks| **Allow multiple readers, one writer:** RW lock allows concurrent access for read-only operations, whereas write operations require exclusive access. This means that multiple threads can read the data in parallel but an exclusive lock is needed for writing or modifying data. | `rwlock_t` |
|Atomic Operations | **Prevent race conditions without locks:** is a type of operation that runs without interruption, ensuring that it is completed as a single, indivisible unit. | `atomic_inc()` |
|Read-Copy-Update (RCU)| Efficient lockless synchronization | `rcu_read_lock()` |

---

## 3. Spinlocks (For Short Locks)
A spinlock is used when you expect the lock to be held for a very short time (microsecond):
```c
# include <linux/spinlock.h>

static spinlick_t my_lock;

void my_func(void) {
        spin_lock(&my_lock);
        // Critical section: Only one thread can enter
        spin_unlock(&my_lock);
}
```
- Used for quick access to shared resources
- But, wastes CPU if the lock is held for too long!

---

## 4. Mutexes (For Longer tasks)
A mutex allows one thread at a time to enter a critical section but puts waiting threads to sleep instead of spinning.
```c
#include <linux.mutex.h>

static DEFINE_MUTEX(my_mutex);

void my_func(void) {
        mutex_lock(&my_mutex);
        // Critical section
        mutex_unlock(&my_mutex);
}
```
- Better than spinlocks when waiting is long
- Cannot be used in interrupt context

---

## 5. Read-Write Locks (For Shared Reading)
```c
#include <linux/rwlock.h>

static rwlock_t my_rwlock;

void read_func(void) {
        read_lock(&my_rwlock);
        // Reading shared data
        read_unlock(&my_rwlock);
}

void write_func(void) {
        write_lock(&my_rwlock);
        // Writing shared data
        write_unlock(&my_rwlock);
}
```
- Efficient for many read, few writes
- Writers must wait for all readers to finish

---

## 6. Atomic Operations (For Lockless Synchronization)
Atomic operations are hardware-supported instructions that allow safe modification of shared variables without explicit locks.

```c
#include <linux/atomic.h>

static atomic_t counter = ATOMIC_INIT(0);

void my_func(void) {
        atomic_inc(&counter);  // Thread-safe increment
}
```
- Fast method which avoids locks
- Limited to simple operations (increment, compare-exchange, etc.)

---

## 7. Read-Copy-Update (RCU) - Lockless Synchronization
RCU allows fast reads while still allowing writes without blocking
```c
#include <linux/rcupdate.h>

void read_fun(void) {
        rcu_read_lock();
        // Read shared data (lock-free)
        rcu_read_unlock();
}
```
- Great for frequently read data
- Writing is complex

---

---

# Locks in Shell:
The shell (Bash, Zsh, etc.) uses **advisory file locking** via `flock()` or other mechanism to prevent concurrent writes.

## Locking Mechanism in Bash:
Example of locking used for `console history`:
|Bash Operation                          |                                           |
|----------------------------------------|-------------------------------------------|
|History Merging (`shopt -s histappend`) | Ensures each terminal appends to `~/.bash_history` instead of overwriting |
|History Synchronization (`PROMPT_COMMAND='history -a; history -r'`)| Writes history immediately after each command and reload it across terminals.|

---

## File Locking Mechanisms in User Space:
Unlike kernel locks, user-space applications use file locks to controls access:
| Lock Type | How it works                       | Common Uses Cases                |
|`flock()`  | Locks an open file descriptor      | Used in Bash history and scripts |
|`fcntl()`  | Advisory locking mechanism         | Used in databases (PostgreSQL, SQLite)|
|`lockf()`  | Simple wrapper around `fcntl()`    | Used in user-space programs      |
|dot-file locking| Creates a `.lock` file to prevent concurrent access | Used in package managers, editors (e.g., Vim) |

**Example of using `flock()` in shell:
```bash
exec 200>~/.bash_history.lock
flock -n 200 || exit 1
echo "Writing safely to history file" >> ~/.bash_history
flock -u 200  # Unlock

# to check:

tail ~/.bash_history

git add src/doc/process_management.md src/doc/mychardev.md
git commit -m 'Doc: fix typos'
git push origin main
Writing safely to history file
```
This prevents two terminals from writing history at the same time.

---

## How to See If a File is Locked

To check if `~/.bash_history` is locked:
```bash
lslocks | grep bash_history
```
or
```bash
losf ~/.bash_history
```
if another process is holding the lock, it will show up.