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

#define BBB_GPIO1_IN (BBB_GPIO1_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO1_OUT (BBB_GPIO1_BASE + OMAP4_GPIO_DATAOUT)

#define BBB_GPIO2_IN (BBB_GPIO2_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO2_OUT (BBB_GPIO2_BASE + OMAP4_GPIO_DATAOUT)

#define BBB_GPIO3_IN (BBB_GPIO3_BASE + OMAP4_GPIO_DATAIN)
#define BBB_GPIO3_OUT (BBB_GPIO3_BASE + OMAP4_GPIO_DATAOUT)

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



/* ================================================*/

struct file_operations gpio_ex_fops = {
    .owner = THIS_MODULE,
    .open = gpio_ex_open,
    .release = gpio_ex_release,
    .read = gpio_ex_read,
    .write = gpio_ex_write
};

/* ================================================*/

ssize_t gpio_ex_read(struct file* filp,
                     char __user* ubuf,
                     size_t count,
                     loff_t* fpos)
{
    u32 reg_state[4];

    printk(KERN_NOTICE "BBB GPIO TESTDRV READ CALL\n");

    readl(BBB_GPIO0_IN, &reg_state[0]);
    readl(BBB_GPIO1_IN, &reg_state[1]);
    readl(BBB_GPIO2_IN, &reg_state[2]);
    readl(BBB_GPIO3_IN, &reg_state[3]);
    
    if (copy_to_user(ubuf, reg_state, 4 * 4))
        return -EFAULT;

    return 0;
}

ssize_t gpio_ex_write(struct file* filp,
                      const char __user* ubuf,
                      size_t count,
                      loff_t* fpos)
{
    u32 reg_state[4];

    printk(KERN_NOTICE "BBB GPIO TESTDRV WRITE CALL\n");

    if (copy_from_user(reg_state, ubuf, 4 * 4)
        return -EFAULT;
        
    writel(BBB_GPIO0_OUT, &reg_state[0]);
    writel(BBB_GPIO1_OUT, &reg_state[1]);
    writel(BBB_GPIO2_OUT, &reg_state[2]);
    writel(BBB_GPIO3_OUT, &reg_state[3]);

    return 0;
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

    return 0;
}

module_init(gpio_ex_init);
module_exit(gpio_ex_exit);