/*
 * This is an experimental kernel code, based on a idea fron ChatGPT.
 * It is a simple character device driver example, a classic "Hello, world of
 * kerenl wdrivers".
 * It creates a pseudo-device (a "virtual" char device), which you can then
 * interact with at /dev/mycardev. When you write to it, it stores data in a
 * small buffrt, and when you read from it, it returns that data back to user
 * space. this demonestrates how the kernel handel devices, I/O operations, and
 * user-kernel boundaries.
 * This code does not talk to any hardware, it is a pseudo0device, but it uses
 * the same mechanis real drivers use: register_chardev, file_operations, etc.
 * Saeed Amiri
 * 17/01/2025
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // register_chardev, struct file_operations
#include <linux/uaccess.h>   // compy_to_user, copy_from_user



