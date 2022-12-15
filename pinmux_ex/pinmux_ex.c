#include <linux/init.h>
#include <linux/module.h>

#include "bbb_pinmux.h"

MODULE_LICENSE("GPL");

static void set_pinmux(volatile unsigned int addr, int mmode)
{
    struct am335x_conf_regval val;
    val.slewctrl = 0;
    val.rxactive = 1;
    val.pulltypesel = 1;
    val.puden = 0;
    val.mmode = mmode;

    writel(addr, val);
}

static int pinmux_ex_init(void)
{
    printk(KERN_NOTICE "BBB PINMUX Test driver init\n");

    /* Set pinmux mode 7(GPIO) */
    set_pinmux(PINCTRL_P8_03, 7);
    set_pinmux(PINCTRL_P8_04, 7);
    set_pinmux(PINCTRL_P8_05, 7);
    set_pinmux(PINCTRL_P8_06, 7);

    return 0;
}

static void pinmux_ex_exit(void)
{
   printk(KERN_NOTICE "BBB PINMUX Test driver exit\n");
    
}

module_init(pinmux_ex_init);
module_exit(pinmux_ex_exit);