# include <linux/init.h>
# include <linux/module.h>
# include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Holden");
MODULE_DESCRIPTION("A simple hello world kernel module");
MODULE_VERSION("0.1");

// Caled when the module is loaded

static int __init hello_init(void) {
    printk(KERN_INFO "Hello World: module loaded. \n");
    return 0;
}

// Called just before tje module is removed from the kernel
static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello World: module unloaded. \n");
}

module_init(hello_init);
module_exit(hello_exit);
