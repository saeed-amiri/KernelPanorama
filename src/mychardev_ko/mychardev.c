/*
 * This is an experimental kernel code, based on a idea from ChatGPT.
 * It is a simple character device driver example, a classic "Hello, world of
 * kernel drivers".
 * It creates a pseudo-device (a "virtual" char device), which you can then
 * interact with at /dev/mycardev. When you write to it, it stores data in a
 * small buffer, and when you read from it, it returns that data back to user
 * space. this demonstrates how the kernel handle devices, I/O operations, and
 * user-kernel boundaries.
 * This code does not talk to any hardware, it is a pseudo0device, but it uses
 * the same mechanism real drivers use: register_chardev, file_operations, etc.
 * Saeed Amiri
 * 17/01/2025
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // register_chardev, struct file_operations
#include <linux/uaccess.h>   // copy_to_user, copy_from_user



