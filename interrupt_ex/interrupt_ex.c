#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include <linux/platform_data/gpio-omap.h>

MODULE_LICENSE("GPL");

#define INTERRUPT_EX_MAJOR       42
#define INTERRUPT_EX_MAX_MINORS  5

/* Magic number, Command number, Data type */
#define READ_GPIO _IOWR('a', 'a', u32*)
#define ON_GPIO _IOW('a', 'b', u32*)
#define OFF_GPIO _IOW('a', 'c', u32*)
#define GPIO_DIR_OUT _IOW('a', 'd', u32*)
#define GPIO_DIR_IN _IOW('a', 'e', u32*)

struct interrupt_ex_data
{
    struct cdev cdev;
};

struct interrupt_ex_data devs[INTERRUPT_EX_MAX_MINORS];

char logbuffer[200];

/* Function declaration */
static void request_gpio_interrupt(int gpio_id, struct file* filp);
/* ===== */

static void gpio_direction_set(int gpio_id, u32 is_read)
{
    struct gpio_desc *gpiod = gpio_to_desc(gpio_id);
    int res;

    if (is_read)
    {
        printk(KERN_NOTICE "SET %d DIRECTION IN\n", gpio_id);
        res = gpiod_direction_input(gpiod);
        if (res == 0){
            printk(KERN_NOTICE "GPIO %d SET DIR INPUT SUCCESS\n", gpio_id);
        } else {
            printk(KERN_NOTICE "GPIO %d SET DIR INPUT FAIL\n", gpio_id);
        }
    }
    else
    {
        printk(KERN_NOTICE "SET %d DIRECTION OUT\n", gpio_id);
        res = gpiod_direction_output(gpiod, 0/* INITIAL VAL */);
        if (res == 0){
            printk(KERN_NOTICE "GPIO %d SET DIR OUTPUT SUCCESS\n", gpio_id);
        } else {
            printk(KERN_NOTICE "GPIO %d SET DIR OUTPUT FAIL\n", gpio_id);
        }
    }
}

static void write_gpio(int gpio_id, u32 val)
{
    struct gpio_desc *gpiod = gpio_to_desc(gpio_id);

    if (val == 0)
    {
        printk(KERN_NOTICE "CLEAR GPIO %d\n", gpio_id);
        gpiod_set_value(gpiod, val);
    }
    else if (val == 1)
    {
        printk(KERN_NOTICE "SET GPIO %d\n", gpio_id);
        gpiod_set_value(gpiod, val);
    }
    else
    {
        printk(KERN_NOTICE "UNABLE TO SET VAL %d\n", val);
        return;
    }
}

static void read_gpio(int gpio_id, u32* buf)
{
    struct gpio_desc *gpiod = gpio_to_desc(gpio_id);
    int val = gpiod_get_value(gpiod);
    printk(KERN_NOTICE "READ_GPIO %d val %d\n", gpio_id, val);
    *buf = val;
}

static long interrupt_ex_ioctl(struct file* file,
                            unsigned int cmd,
                            unsigned long arg)
{
    u32 gpio_id;
    u32 res;

    if (copy_from_user(&gpio_id, (u32*)arg, sizeof(gpio_id)))
    {
        printk(KERN_NOTICE "GPIO IOCTL CALLED BUT CAN'T READ FROM USER\n");
    }
    else
    {
        printk(KERN_NOTICE "GPIO IOCTL CALL. ID:%d\n", gpio_id);
    }

    switch (cmd)
    {
        case READ_GPIO:
            /* Read GPIO */
            read_gpio(gpio_id, &res);

            /* Send to user */
            if (copy_to_user((u32*)arg, &res, sizeof(res)))
            {
                printk(KERN_NOTICE "GPIO READ IOCTL CALLED BUT CAN'T COPY TO USER\n");
            }
            else
            {
                printk(KERN_NOTICE "GPIO READ IOCTL CALL. VAL:%u\n", res);
            }
            break;
        case ON_GPIO:
            /* Write GPIO to 1 */
            write_gpio(gpio_id, 1);
            break;
        case OFF_GPIO:
            /* Write GPIO to 0 */
            write_gpio(gpio_id, 0);
            break;
        case GPIO_DIR_OUT:
            gpio_direction_set(gpio_id, 0);
            break;
        case GPIO_DIR_IN:
            gpio_direction_set(gpio_id, 1);
            request_gpio_interrupt(gpio_id, file);
            break;
        default:
            break;
    }

    return 0;
}

static irqreturn_t gpio_threaded_interrupt_handler(int irq, void* dev_id)
{
    printk(KERN_NOTICE "GPIO THREADED INTERRUPT HANDLED. irq:%d dev_id:%p\n",
           irq, dev_id);
    return IRQ_HANDLED;
}

static irqreturn_t gpio_interrupt_handler(int irq, void* dev_id)
{
    /* You can't use printk in ISR. This is only test purpose... */
    printk(KERN_NOTICE "GPIO INTERRUPT HANDLED. irq:%d dev_id:%p\n",
           irq, dev_id);
    return IRQ_WAKE_THREAD;
}


static void request_gpio_interrupt(int gpio_id, struct file* filp)
{
    int res, irq;
    struct gpio_desc *irq_gpio = gpio_to_desc(gpio_id);

    if (!irq_gpio){
        printk("gpio_to_desc failed(%d)\n", gpio_id);
        return;
    }
 
    irq = gpiod_to_irq(irq_gpio);
    if (irq < 0){
        printk("gpiod_to_irq failed(%d)\n", irq);
        return;
    } else {
        printk("gpiod_to_irq success(%d)\n", irq);
    }

    res = request_threaded_irq(irq,
                               gpio_interrupt_handler,
                               gpio_threaded_interrupt_handler,
                               IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                               "interrupt_ex",
                               filp);

    if (res == 0)
    {
        printk(KERN_NOTICE "Request IRQ %d SUCCESS\n", irq);
    } 
    else {
        printk(KERN_NOTICE "Request IRQ %d FAIL\n", irq);
    }
}

ssize_t interrupt_ex_read(struct file* filp,
                       char __user* ubuf,
                       size_t count,
                       loff_t* fpos)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV READ CALL\n");
    return count;
}

ssize_t interrupt_ex_write(struct file* filp,
                        const char __user* ubuf,
                        size_t count,
                        loff_t* fpos)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV WRITE CALL\n");
    return count;
}

int interrupt_ex_open(struct inode* inodp,
                   struct file* filp)
{
    struct interrupt_ex_data* drv_data;

    printk(KERN_NOTICE "BBB PINMUX TESTDRV OPEN CALL\n");

    drv_data = container_of(inodp->i_cdev, struct interrupt_ex_data, cdev);
    filp->private_data = drv_data;

    return 0;
}

int interrupt_ex_release(struct inode* inodp,
                      struct file* filp)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV RELEASE CALL\n");
    return 0;
}

struct file_operations interrupt_ex_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = interrupt_ex_ioctl,
    .open = interrupt_ex_open,
    .release = interrupt_ex_release,
    .read = interrupt_ex_read,
    .write = interrupt_ex_write
};

static int interrupt_ex_init(void)
{
    int err, i;
    printk(KERN_NOTICE "BBB PINMUX Test driver init\n");

    err = register_chrdev_region(MKDEV(INTERRUPT_EX_MAJOR, 0),
                                 INTERRUPT_EX_MAX_MINORS,
                                 "interrupt_example_driver");
    if (err != 0)
        return err;
    printk(KERN_NOTICE "TRY CDEV INIT\n");

    for (i = 0; i < INTERRUPT_EX_MAX_MINORS; i++)
    {
        cdev_init(&devs[i].cdev, &interrupt_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(INTERRUPT_EX_MAJOR, i), 1);
    }
    printk(KERN_NOTICE "CDEV INIT DONE\n");


    return 0;
}

static void interrupt_ex_exit(void)
{
    printk(KERN_NOTICE "BBB INTERRUPT Test driver exit\n");
}

module_init(interrupt_ex_init);
module_exit(interrupt_ex_exit);