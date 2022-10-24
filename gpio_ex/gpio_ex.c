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

#define GPIO_EX_MAJOR       42
#define GPIO_EX_MAX_MINORS  5

/* ================================================*/
struct gpio_ex_data
{
    struct cdev cdev;
    /* my data starts here */
    //...
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

/* ================================================*/

struct file_operations gpio_ex_fops = {
    .owner = THIS_MODULE,
    .read = gpio_ex_read,
    .write = gpio_ex_write
};

/* ================================================*/

ssize_t gpio_ex_read(struct file* filp,
                     char __user* ubuf,
                     size_t count,
                     loff_t* fpos)
{
    struct gpio_ex_data *drv_data; 
    u32 reg_state;

    if (count < sizeof(reg_state))
        return -EFAULT;

    drv_data = (struct gpio_ex_data *) filp->private_data;
    reg_state =  *((volatile u32 __force*)BBB_GPIO0_IN);
    
    if (copy_to_user(ubuf, &reg_state, sizeof(u32)))
        return -EFAULT;

    return sizeof(reg_state);
}

ssize_t gpio_ex_write(struct file* filp,
                      const char __user* buf,
                      size_t count,
                      loff_t* fpos)
{
#if 0
    int ret = 0;
    (volatile u32 __force*)BBB_GPIO0_OUT = 1;
    return ret;
#endif
    return 1;
}

int gpio_ex_open(struct inode* inodp,
                 struct file* filp)
{
    struct gpio_ex_data* drv_data;
    drv_data = container_of(inodp->i_cdev, struct gpio_ex_data, cdev);
    filp->private_data = drv_data;

    return 0;
}

static void gpio_ex_exit(void)
{
    int i;
    printk(KERN_NOTICE "BBB GPIO TESTDRV EXIT\n");

    for (i = 0; i < GPIO_EX_MAX_MINORS; i++)
    {
        /* release devs[i] fields */
        cdev_del(&devs[i].cdev);
    }
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
    {
        return err;
    }

    for (i = 0; i < GPIO_EX_MAX_MINORS; i++)
    {
        /* initialize devs[i] fields */
        cdev_init(&devs[i].cdev, &gpio_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(GPIO_EX_MAJOR, i), 1);
    }

    return 0;
}

module_init(gpio_ex_init);
module_exit(gpio_ex_exit);