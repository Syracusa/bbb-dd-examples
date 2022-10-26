#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define CHAR_EX_MAJOR       142
#define CHAR_EX_MAX_MINORS  5

/* ================================================*/
struct char_ex_data
{
    struct cdev cdev;

    char buf[1024];
    int offset;
};

struct char_ex_data devs[CHAR_EX_MAX_MINORS];

/* ================================================*/
ssize_t char_ex_read(struct file* filp,
                     char __user* buf,
                     size_t count,
                     loff_t* fpos);

ssize_t char_ex_write(struct file* filp,
                      const char __user* buf,
                      size_t count,
                      loff_t* fpos);

/* ================================================*/

static void prt_drvinfo(struct char_ex_data *drv_data)
{

}

/* ================================================*/
int char_ex_open(struct inode* inodp,
                 struct file* filp)
{
    dev_t dev;
    struct char_ex_data *drv_data;

    drv_data = container_of(inodp->i_cdev, struct char_ex_data, cdev);
    filp->private_data = drv_data;

    dev = inodp->i_cdev->dev;
    printk(KERN_NOTICE "BBB charex driver open called(major:%u minor:%u)\n",
            MAJOR(dev), MINOR(dev));
    printk(KERN_NOTICE "Current offset : %d\n", drv_data->offset);
    return 0;
}

int char_ex_release(struct inode* inodp,
                 struct file* filp)
{
    dev_t dev;
    struct char_ex_data *drv_data;

    drv_data = container_of(inodp->i_cdev, struct char_ex_data, cdev);
    filp->private_data = drv_data;

    dev = inodp->i_cdev->dev;
    printk(KERN_NOTICE "BBB charex driver release called(major:%u minor:%u)\n",
            MAJOR(dev), MINOR(dev));
    printk(KERN_NOTICE "Current offset : %d\n", drv_data->offset);
    return 0;
}

ssize_t char_ex_write(struct file* filp,
                      const char __user* buf,
                      size_t count,
                      loff_t* fpos)
{
    struct char_ex_data *drv_data;
    drv_data = filp->private_data;

    printk(KERN_NOTICE "BBB charex driver write called(major:%u minor:%u)\n",
            MAJOR(drv_data->cdev.dev), MINOR(drv_data->cdev.dev));
    return 0;
}

ssize_t char_ex_read(struct file* filp,
                     char __user* ubuf,
                     size_t count,
                     loff_t* fpos)
{
    struct char_ex_data *drv_data;
    drv_data = filp->private_data;

    printk(KERN_NOTICE "BBB charex driver read called(major:%u minor:%u)\n",
            MAJOR(drv_data->cdev.dev), MINOR(drv_data->cdev.dev));
    return 0;
}


static void char_ex_exit(void)
{
    int i;
    printk(KERN_NOTICE "BBB CHARDRV EXIT\n");

    for (i = 0; i < CHAR_EX_MAX_MINORS; i++)
    {
        cdev_del(&devs[i].cdev);
    }
    unregister_chrdev_region(MKDEV(CHAR_EX_MAJOR, 0),
                             CHAR_EX_MAX_MINORS);
}

struct file_operations char_ex_fops = {
    .owner = THIS_MODULE,
    .open = char_ex_open,
    .release = char_ex_release,
    .read = char_ex_read,
    .write = char_ex_write
};

static int char_ex_init(void)
{
    int i, err;
    printk(KERN_NOTICE "BBB CHARDRV INIT\n");

    err = register_chrdev_region(MKDEV(CHAR_EX_MAJOR, 0),
                                 CHAR_EX_MAX_MINORS,
                                 "char_example_driver");
    if (err != 0)
    {
        return err;
    }

    for (i = 0; i < CHAR_EX_MAX_MINORS; i++)
    {
        devs[i].offset = 0;
        cdev_init(&devs[i].cdev, &char_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(CHAR_EX_MAJOR, i), 1);
    }

    return 0;
}

module_init(char_ex_init);
module_exit(char_ex_exit);