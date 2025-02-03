# Mychardev
The goal of this exercises is to explore Linux kernel programming by implementing a real word example. First, start with simple modules and going toward advanced topics like character devices, system calls, and kernel-space user interactions.

#### What:
> ##### Basic Kernel modules:
>> 1. Writing a minimal kernel module
>> 2. Working with ```c printk()``` 
>> 3. Loading (insmod) and unloading (rmmod) modules
>> 4. Checking kernel logs (dmesg, journalctl)
> ##### Charter Device Drivers:
>> 1. Registering a character device
>> 2. Implementing basic open, read, write and release functions
>> 3. Interacting with the device from user-space (`/dev/mychardev`)
> ##### File system interactions:
>> 1. Creating and managing `/proc` and `/sys` files
>> 2. Reading and writing from `/dev`
>> 3. Working with kernel buffers
> ##### System Calls & Kernel-User Communication:
>> 1. Extending the kernel with a custom system call
>> 2. Handling synchronization and concurrency
>> 3. Using `ioctl()` for advanced communication
> ##### Advanced Topics & Optimization:
>> 1. Working with kernel threads
>> 2. Memory allocation (`kmalloc`, `vmalloc`)
>> 3. Debugging (`gdb`, `kgdb`, `ftrace`)
