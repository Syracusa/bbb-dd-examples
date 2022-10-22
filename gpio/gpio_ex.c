#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/cdev.h>

#include <arch/arm/mach-omap2/omap34xx.h>
#include <linux/platform_data/gpio-omap.h>

MODULE_LICENSE("GPL");

#define BBB_GPIO0_BASE 0x44E07000
#define BBB_GPIO1_BASE 0x4804C000
#define BBB_GPIO2_BASE 0x481AC000
#define BBB_GPIO3_BASE 0x481AE000

#define BBB_GPIO0_IN (BBB_GPIO0_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO0_OUT (BBB_GPIO0_BASE + OMAP4_GPIO_DATAOUT)

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