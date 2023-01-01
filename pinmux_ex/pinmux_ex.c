#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>

#include "bbb_pinmux.h"

MODULE_LICENSE("GPL");

#define READ_GPIO _IOWR('a', 'a', int32_t*)
#define ON_GPIO _IOW('a', 'b', int32_t*)
#define OFF_GPIO _IOW('a', 'c', int32_t*)


#define PINMUX_EX_MAJOR       42
#define PINMUX_EX_MAX_MINORS  5

char logbuffer[200];

struct file_operations gpio_ex_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = pinmux_ex_ioctl
    // .open = gpio_ex_open,
    // .release = gpio_ex_release,
    // .read = gpio_ex_read,
    // .write = gpio_ex_write
};

static long pinmux_ex_ioctl(struct file* file,
                            unsigned int cmd,
                            unsigned long arg)
{
    int32_t val;
    switch (cmd)
    {
        case READ_GPIO:
            /* Get GPIO ID to read */
            if (copy_from_user(&val, (int32_t*)arg, sizeof(value)))
            {
                printk(KERN_NOTICE, "GPIO READ IOCTL CALLED BUT CAN'T READ FROM USER\n");
            }
            else
            {
                printk(KERN_NOTICE, "GPIO READ IOCTL CALL. VAL:%d\n", val);
            }

            /* Read GPIO*/
            // TBD
            val = 1

            /**/
            if (copy_to_user(&arg, &val, sizeof(val)))
            {
                printk(KERN_NOTICE, "GPIO READ IOCTL CALLED BUT CAN'T READ FROM USER\n");
            }
            else
            {
                printk(KERN_NOTICE, "GPIO READ IOCTL CALL. VAL:%d\n", val);
            }
            break;
        case ON_GPIO:
            /* Get GPIO ID to on */
            if (copy_from_user(&val, (int32_t*)arg, sizeof(value)))
            {
                printk(KERN_NOTICE, "GPIO ON IOCTL CALLED BUT CAN'T READ FROM USER\n");
            }
            else
            {
                printk(KERN_NOTICE, "GPIO ON IOCTL CALL. VAL:%d\n", val);
            }
            
            /* Write GPIO to 1 */
            // TBD

            break;
        case OFF_GPIO:
            /* Get GPIO ID to off */
            if (copy_from_user(&val, (int32_t*)arg, sizeof(value)))
            {
                printk(KERN_NOTICE, "GPIO OFF IOCTL CALLED BUT CAN'T READ FROM USER\n");
            }
            else
            {
                printk(KERN_NOTICE, "GPIO OFF IOCTL CALL. VAL:%d\n", val);
            }

            /* Write GPIO to 0 */
            // TBD

            break;
        default:
            break;
    }

}

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

static void enable_interrupt()
{
    /* request_irq(gpio_isr) */
}

static void gpio_isr()
{
    /* Fill logbuffer interrupted time and information */
}

static int pinmux_ex_init(void)
{
    printk(KERN_NOTICE "BBB PINMUX Test driver init\n");

    /* Set pinmux mode 7(GPIO) */
    set_pinmux(PINCTRL_P8_03, 7);
    set_pinmux(PINCTRL_P8_04, 7);
    set_pinmux(PINCTRL_P8_05, 7);
    set_pinmux(PINCTRL_P8_06, 7);

    enable_interrupt();

    return 0;
}

static void pinmux_ex_exit(void)
{
    printk(KERN_NOTICE "BBB PINMUX Test driver exit\n");

}

module_init(pinmux_ex_init);
module_exit(pinmux_ex_exit);