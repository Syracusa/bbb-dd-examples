#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

#define BBB_CTRL_BASE   0x0

static int pinmux_ex_init(void)
{
    printk(KERN_NOTICE "BBB PINMUX Test driver init\n");

    return 0;
}

static void pinmux_ex_exit(void)
{
   printk(KERN_NOTICE "BBB PINMUX Test driver exit\n");
    
}

module_init(pinmux_ex_init);
module_exit(pinmux_ex_exit);