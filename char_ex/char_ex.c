#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define CHAR_EX_MAJOR       142
#define CHAR_EX_MAX_MINORS  5

#define DRV_BUFLEN  1024

/* ================================================*/
struct char_ex_data
{
    struct cdev cdev;

    char buf[BUFLEN];
    int datalen;
};

struct char_ex_data devs[CHAR_EX_MAX_MINORS];

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
    printk(KERN_NOTICE "Current datalen : %d\n", drv_data->datalen);
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
    printk(KERN_NOTICE "Current datalen : %d\n", drv_data->datalen);
    return 0;
}

ssize_t char_ex_write(struct file* filp,
                      const char __user* ubuf,
                      size_t count,
                      loff_t* fpos)
{
    struct char_ex_data *drv_data;
    int buf_remain;
    int write_len;
    int copy_res;
    
    drv_data = filp->private_data;

    printk(KERN_NOTICE "BBB charex driver write called(major:%u minor:%u)\n",
            MAJOR(drv_data->cdev.dev), MINOR(drv_data->cdev.dev));

    buf_remain = DRV_BUFLEN - drv_data->datalen;
    if (buf_remain > count)
        write_len = count;
    else
        write_len = buf_remain;
    
    copyres = copy_from_user(ubuf, &(drv_data->buf[drv_data->datalen]), write_len);
    if (copyres)
    {
        return -EFAULT;
    {
    else 
    {
        drv_data->datalen += write_len;
        return write_len;
    }
}

ssize_t char_ex_read(struct file* filp,
                     char __user* ubuf,
                     size_t count,
                     loff_t* fpos)
{
    struct char_ex_data *drv_data;
    int read_len;
    int copy_res;


    drv_data = filp->private_data;

    printk(KERN_NOTICE "BBB charex driver read called(major:%u minor:%u)\n",
            MAJOR(drv_data->cdev.dev), MINOR(drv_data->cdev.dev));
    
    if (drv_data->datalen > count)
        read_len = count;
    else 
        read_len = drv_data->datalen;
    
    copyres = copy_to_user(ubuf, drv_data->buf, readlen);
    if (copyres)
    {
        return -EFAULT;
    }
    else 
    {
        drv_data->datalen -= readlen;
        
        /* Move data to original point */    
        for (int i = 0; i < readlen; i++){
            drv_data->buf[i] = drv_data->buf[i+readlen];
        }
    }
    
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
        devs[i].datalen = 0;
        cdev_init(&devs[i].cdev, &char_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(CHAR_EX_MAJOR, i), 1);
    }

    return 0;
}

module_init(char_ex_init);
module_exit(char_ex_exit);