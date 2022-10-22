#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static void gpio_ex_exit(void)
{
    printk(KERN_NOTICE "BBB GPIO TESTDRV EXIT\n");
}

static int gpio_ex_init(void)
{
    int ret = 0;
    printk(KERN_NOTICE "BBB GPIO TESTDRV INIT\n");

    return ret;
}


module_init(gpio_ex_init);
module_exit(gpio_ex_exit);