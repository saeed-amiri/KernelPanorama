/*
 * This is an experimental kernel code, based on a idea from ChatGPT.
 * It is a simple character device driver example, a classic "Hello, world of
 * kernel drivers".
 * It creates a pseudo-device (a "virtual" char device), which you can then
 * interact with at /dev/mycardev. When you write to it, it stores data in a
 * small buffer, and when you read from it, it returns that data back to user
 * space. this demonstrates how the kernel handle devices, I/O operations, and
 * user-kernel boundaries.
 * This code does not talk to any hardware, it is a pseudo-device, but it uses
 * the same mechanism real drivers use: register_chardev, file_operations, etc.
 * Saeed Amiri
 * 17/01/2025
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // register_chardev, struct file_operations
#include <linux/uaccess.h>   // copy_to_user, copy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SD")
MODULE_DESCRIPTION("A simple character device driver example");
MODULE_VERSION("0.1")

/*
 * We'll store up to 128 bytes from user writes.
 * In a real driver, the allocation should be handle dynamically and with care
*/
#define BUF_SIZE 128
static chat device_buffer[BUF_SIZE];
static int buffer_len;

// Store the major number that the kernel gives us
static int mychardev_major;

/*
 * Prototypes for our device functions
 * open, release, read, write
*/

static int     mychardev_open(struct inode *inode, struct file *file);
static int     mychardev_release(struct inode *inode, struct file *file);
static ssize_t mychardev_read(
    struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t mychardev_write(
    struct file *file, const char __user *buf, size_t len, loff_t *off)
