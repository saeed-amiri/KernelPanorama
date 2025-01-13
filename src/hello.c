# include <linux/init.h>
# include <linux/module.h>
# include <linux/kernel.h>

# define DRIVER_AUTHOR "Holden"
# define DRIVER_DESC "A sample driver"
# define DRIVER_LICENSE "GPL"
# define DRIVER_VERSION "0.1"

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);

// Caled when the module is loaded

static int __init hello_init(void) {
    printk(KERN_INFO "Hello World: module loaded. \n");
    pr_info("\tAuthor: %s\n", DRIVER_AUTHOR);
    pr_info("\tDescription: %s\n", DRIVER_DESC);
    pr_info("\tVersion: %s\n", DRIVER_VERSION);
    pr_info("\tLicense: %s\n", DRIVER_LICENSE);
    return 0;
}

// Called just before tje module is removed from the kernel
static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello World: module unloaded. \n");
}

module_init(hello_init);
module_exit(hello_exit);
