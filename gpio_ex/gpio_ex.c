#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include <linux/platform_data/gpio-omap.h>

MODULE_LICENSE("GPL");

#define BBB_GPIO0_BASE 0x44E07000
#define BBB_GPIO1_BASE 0x4804C000
#define BBB_GPIO2_BASE 0x481AC000
#define BBB_GPIO3_BASE 0x481AE000

#define BBB_GPIO0_IN (BBB_GPIO0_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO0_OUT (BBB_GPIO0_BASE + OMAP4_GPIO_DATAOUT)
#define BBB_GPIO0_OE (BBB_GPIO0_BASE + OMAP4_GPIO_OE)

#define BBB_GPIO1_IN (BBB_GPIO1_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO1_OUT (BBB_GPIO1_BASE + OMAP4_GPIO_DATAOUT)
#define BBB_GPIO1_OE (BBB_GPIO1_BASE + OMAP4_GPIO_OE)

#define BBB_GPIO2_IN (BBB_GPIO2_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO2_OUT (BBB_GPIO2_BASE + OMAP4_GPIO_DATAOUT)
#define BBB_GPIO2_OE (BBB_GPIO2_BASE + OMAP4_GPIO_OE)

#define BBB_GPIO3_IN (BBB_GPIO3_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO3_OUT (BBB_GPIO3_BASE + OMAP4_GPIO_DATAOUT)
#define BBB_GPIO3_OE (BBB_GPIO3_BASE + OMAP4_GPIO_OE)

#define GPIO_EX_MAJOR       42
#define GPIO_EX_MAX_MINORS  5

/* ================================================*/
struct gpio_ex_data
{
    struct cdev cdev;
};

struct gpio_ex_data devs[GPIO_EX_MAX_MINORS];

/* ================================================*/
ssize_t gpio_ex_read(struct file* filp,
                     char __user* buf,
                     size_t count,
                     loff_t* fpos);

ssize_t gpio_ex_write(struct file* filp,
                      const char __user* buf,
                      size_t count,
                      loff_t* fpos);

int gpio_ex_open(struct inode* inodp,
                 struct file* filp);

int gpio_ex_release(struct inode* inodp,
                    struct file* filp);

/* ================================================*/

struct file_operations gpio_ex_fops = {
    .owner = THIS_MODULE,
    .open = gpio_ex_open,
    .release = gpio_ex_release,
    .read = gpio_ex_read,
    .write = gpio_ex_write
};

/* ================================================*/

static void set_output_enable(u32 val)
{
    /* TODO void --> u32 */
    void __iomem * gpio0_oe = ioremap(BBB_GPIO0_OE, 4);
    void __iomem * gpio1_oe = ioremap(BBB_GPIO1_OE, 4);
    void __iomem * gpio2_oe = ioremap(BBB_GPIO2_OE, 4);
    void __iomem * gpio3_oe = ioremap(BBB_GPIO3_OE, 4);

    printk(KERN_NOTICE "Set all gpio oe register to %u %p %p %p %p\n",
           val, gpio0_oe, gpio1_oe, gpio2_oe, gpio3_oe);

    *(u32*)gpio0_oe = val;
    *(u32*)gpio1_oe = val;
    *(u32*)gpio2_oe = val;
    *(u32*)gpio3_oe = val;
}

static void prt_register(u32 val)
{
    int i;
    char vals[33];
    u32 pos;
    vals[32] = '\0';

    pos = 1; 
    for (i = 0; i < 32; i++){
        if ((val & pos) != 0){
            vals[i] = '1';
        } else {
            vals[i] = '0';
        }
        pos = pos << 1;
    }
    
    printk(KERN_NOTICE "%s\n", vals);
}


/*
TODO : Fixit
This function generates an error below.

Unhandled fault: external abort on non-linefetch (0x1028) at 0xfa1ac138
*/
ssize_t gpio_ex_read(struct file* filp,
                     char __user* ubuf,
                     size_t count,
                     loff_t* fpos)
{
    void __iomem * gpio0_in = ioremap(BBB_GPIO0_IN, 4);
    void __iomem * gpio1_in = ioremap(BBB_GPIO1_IN, 4);
    void __iomem * gpio2_in = ioremap(BBB_GPIO2_IN, 4);
    void __iomem * gpio3_in = ioremap(BBB_GPIO3_IN, 4);

    printk(KERN_NOTICE "BBB GPIO TESTDRV READ CALL\n");

    set_output_enable(0xFFFFFFFF); /* Disable output */

    prt_register(*(u32*)gpio0_in);
    prt_register(*(u32*)gpio1_in);
    prt_register(*(u32*)gpio2_in);
    prt_register(*(u32*)gpio3_in);
    
    return count;
}

ssize_t gpio_ex_write(struct file* filp,
                      const char __user* ubuf,
                      size_t count,
                      loff_t* fpos)
{
    u32 val, setall;
    u8 input;

    void __iomem * gpio0_out = ioremap(BBB_GPIO0_OUT, 4);
    void __iomem * gpio1_out = ioremap(BBB_GPIO1_OUT, 4);
    void __iomem * gpio2_out = ioremap(BBB_GPIO2_OUT, 4);
    void __iomem * gpio3_out = ioremap(BBB_GPIO3_OUT, 4);

    printk(KERN_NOTICE "BBB GPIO TESTDRV WRITE CALL\n");
    setall = 0xFFFFFFFF;

    if (copy_from_user(&input, ubuf, 1))
        return -EFAULT;

    if (input == '1')
    {
        val = 0xFFFFFFFF;
        printk(KERN_NOTICE "BBB GPIO SET ALL 1\n");
    }
    else
    {
        val = 0x0;
        printk(KERN_NOTICE "BBB GPIO SET ALL 0\n");
    }

    set_output_enable(0x0); /* Enable output */

    *(u32*)gpio0_out = val;
    *(u32*)gpio1_out = val;
    *(u32*)gpio2_out = val;
    *(u32*)gpio3_out = val;

    printk(KERN_NOTICE "BBB GPIO SETDATAOUT DONE\n");
    return count;
}

int gpio_ex_open(struct inode* inodp,
                 struct file* filp)
{
    struct gpio_ex_data* drv_data;

    printk(KERN_NOTICE "BBB GPIO TESTDRV OPEN CALL\n");

    drv_data = container_of(inodp->i_cdev, struct gpio_ex_data, cdev);
    filp->private_data = drv_data;

    return 0;
}

int gpio_ex_release(struct inode* inodp,
                    struct file* filp)
{
    printk(KERN_NOTICE "BBB GPIO TESTDRV RELEASE CALL\n");
    return 0;
}

static void gpio_ex_exit(void)
{
    int i;
    printk(KERN_NOTICE "BBB GPIO TESTDRV EXIT\n");

    for (i = 0; i < GPIO_EX_MAX_MINORS; i++)
        cdev_del(&devs[i].cdev);

    unregister_chrdev_region(MKDEV(GPIO_EX_MAJOR, 0),
                             GPIO_EX_MAX_MINORS);
}

static int gpio_ex_init(void)
{
    int i, err;

    printk(KERN_NOTICE "BBB GPIO TESTDRV INIT\n");

    err = register_chrdev_region(MKDEV(GPIO_EX_MAJOR, 0),
                                 GPIO_EX_MAX_MINORS,
                                 "gpio_example_driver");
    if (err != 0)
        return err;

    for (i = 0; i < GPIO_EX_MAX_MINORS; i++)
    {
        cdev_init(&devs[i].cdev, &gpio_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(GPIO_EX_MAJOR, i), 1);
    }

    set_output_enable(0x0);
    return 0;
}

module_init(gpio_ex_init);
module_exit(gpio_ex_exit);