#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include <linux/platform_data/gpio-omap.h>

MODULE_LICENSE("GPL");

/* Technical reference 180P */
#define CONTORL_MODULE_BASE  0x44E10000 

/* Technical reference 1459P */
#define PINCTRL_P8_03 (CONTORL_MODULE_BASE + 0x818) /* gpmc_ad6 - 38 - GPIO1_6 */
#define PINCTRL_P8_04 (CONTORL_MODULE_BASE + 0x81c) /* gpmc_ad7 - 39 - GPIO1_7 */
#define PINCTRL_P8_05 (CONTORL_MODULE_BASE + 0x808) /* gpmc_ad2 - 34 - GPIO1_2 */
#define PINCTRL_P8_06 (CONTORL_MODULE_BASE + 0x80c) /* gpmc_ad3 - 35 - GPIO1_3 */

/* Technical reference 1515P */
struct am335x_conf_regval
{
    /* LSB should be first */
    u32 mmode : 3;
    u32 puden : 1;
    u32 pulltypesel : 1;
    u32 rxactive : 1;
    u32 slewctrl : 1;
    u32 resv2 : 13;
    u32 resv1 : 12;
}__attribute__((packed));

#define PINMUX_EX_MAJOR       42
#define PINMUX_EX_MAX_MINORS  5

/* Magic number, Command number, Data type */
#define READ_GPIO _IOWR('a', 'a', u32*)
#define ON_GPIO _IOW('a', 'b', u32*)
#define OFF_GPIO _IOW('a', 'c', u32*)
#define GPIO_DIR_OUT _IOW('a', 'd', u32*)
#define GPIO_DIR_IN _IOW('a', 'e', u32*)

#define BBB_GPIO0_BASE 0x44E07000
#define BBB_GPIO1_BASE 0x4804C000
#define BBB_GPIO2_BASE 0x481AC000
#define BBB_GPIO3_BASE 0x481AE000

/* Interrupts - Technical reference 543P */
#define BBB_GPIOINT0A   96
#define BBB_GPIOINT0B   97
#define BBB_GPIOINT1A   98
#define BBB_GPIOINT1B   99
#define BBB_GPIOINT2A   32
#define BBB_GPIOINT2B   33
#define BBB_GPIOINT3A   62
#define BBB_GPIOINT3B   63

struct pinmux_ex_data
{
    struct cdev cdev;
};

struct pinmux_ex_data devs[PINMUX_EX_MAX_MINORS];

char logbuffer[200];


/* Function declaration */
static void request_gpio_interrupt(int gpio_id, struct file* filp);

/* ===== */

static unsigned long get_gpio_base_addr(int gpio_id)
{
    int gpio_regidx = gpio_id / 32;
    switch (gpio_regidx)
    {
        case 0:
            return BBB_GPIO0_BASE;
        case 1:
            return BBB_GPIO1_BASE;
        case 2:
            return BBB_GPIO2_BASE;
        case 3:
            return BBB_GPIO3_BASE;
        default:
            break;
    }
    return 0;
}

static void gpio_direction_set(int gpio_id, u32 is_read)
{
    u32 __iomem* outaddr = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_OE, 4);
    int offset = gpio_id - ((gpio_id / 32) * 32);

    u32 val = readl(outaddr);
    if (is_read)
    {
        printk(KERN_NOTICE "SET %d DIRECTION IN\n", gpio_id);
        val |= 1UL << offset; /* Set bit */
    }
    else
    {
        printk(KERN_NOTICE "SET %d DIRECTION OUT\n", gpio_id);
        val &= ~(1 << offset); /* Clear bit */
    }
    writel(val, outaddr);
}

static void write_gpio(int gpio_id, u32 val)
{
    int offset = gpio_id - ((gpio_id / 32) * 32);
    u32 __iomem* outaddr;
    u32 mask = 0UL;
    mask |= 1UL << offset;

    if (val == 0)
    {
        outaddr = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_CLEARDATAOUT, 4);
        printk(KERN_NOTICE "CLEAR GPIO %d\n", gpio_id);
    }
    else if (val == 1)
    {
        outaddr = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_SETDATAOUT, 4);
        printk(KERN_NOTICE "SET GPIO %d\n", gpio_id);
    }
    else
    {
        printk(KERN_NOTICE "UNABLE TO SET VAL %d\n", val);
        return;
    }

    writel(mask, outaddr);
}

static void read_gpio(int gpio_id, u32* buf)
{
    u32 __iomem* inaddr = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_DATAIN, 4);
    int offset = gpio_id - ((gpio_id / 32) * 32);
    u32 regval = readl(inaddr);
    printk(KERN_NOTICE "REG %p VAL %x\n", inaddr, regval);
    *buf = ((regval >> offset) & 1UL);
}

static long pinmux_ex_ioctl(struct file* file,
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

static void set_pinmux(unsigned long addr, int mmode)
{
    struct am335x_conf_regval val;
    u32 aggr;
    u32 __iomem* remapped = ioremap(addr, 4);

    u32 regval = ioread32(remapped);
    printk(KERN_NOTICE "BBB PINMUX PYSICAL ADDR : %lx\n", addr);
    printk(KERN_NOTICE "BBB PINMUX PRIV : %p %x\n", remapped, regval);

    val.resv1 = 0;
    val.resv2 = 0;
    val.slewctrl = 0;
    val.rxactive = 1;
    val.pulltypesel = 1;
    val.puden = 0;
    val.mmode = mmode;

    memcpy(&aggr, &val, sizeof(u32));
    printk(KERN_NOTICE "BBB PINMUX TRY REWRITE REG : %p %x\n",
           remapped, aggr);
    iowrite32(aggr, remapped);

    regval = ioread32(remapped);
    printk(KERN_NOTICE "BBB PINMUX AFTER : %p %x\n", remapped, regval);
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
    return IRQ_HANDLED;
}

static void setval_rising_detect(int gpio_id, int val)
{
    /* Rising detect enable register */
    u32 __iomem* rdttreg
        = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_RISINGDETECT, 4);

    int offset = gpio_id - ((gpio_id / 32) * 32);
    u32 rdval = ioread32(rdttreg);

    if (val == 0)
    {
        printk(KERN_NOTICE "CLEAR GPIO RISING DETECT %d\n", gpio_id);
        rdval &= ~(1UL << offset);
    }
    else if (val == 1)
    {
        printk(KERN_NOTICE "SET GPIO RISING DETECT %d\n", gpio_id);
        rdval |= 1UL << offset;
    }
    else
    {
        printk(KERN_NOTICE "UNABLE TO SET VAL %d TO RISING DETECT REG\n", val);
        return;
    }

    iowrite32(rdval, rdttreg);
}

static void setval_falling_detect(int gpio_id, int val)
{
    /* Falling detect enable register */
    u32 __iomem* fdttreg
        = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_FALLINGDETECT, 4);

    int offset = gpio_id - ((gpio_id / 32) * 32);
    u32 fdval = ioread32(fdttreg);

    if (val == 0)
    {
        printk(KERN_NOTICE "CLEAR GPIO FALLING DETECT %d\n", gpio_id);
        fdval &= ~(1UL << offset);
    }
    else if (val == 1)
    {
        printk(KERN_NOTICE "SET GPIO FALLING DETECT %d\n", gpio_id);
        fdval |= 1UL << offset;
    }
    else
    {
        printk(KERN_NOTICE "UNABLE TO SET VAL %d TO FALLING DETECT REG\n", val);
        return;
    }

    iowrite32(fdval, fdttreg);
}

static void setval_irqstatus_set(int gpio_id, int val)
{
    u32 __iomem* target_reg;
    int offset = gpio_id - ((gpio_id / 32) * 32);
    u32 mask = 1UL << offset;

    if (val == 0)
    {
        /* GPIO_IRQSTATUS_CLR_0 register */
        target_reg = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_IRQSTATUSCLR0, 4);
        printk(KERN_NOTICE "SET GPIO IRQSTATUSCLR0 %d\n", gpio_id);
    }
    else if (val == 1)
    {
        /* GPIO_IRQSTATUS_SET_0 register */
        target_reg = ioremap(get_gpio_base_addr(gpio_id) + OMAP4_GPIO_IRQSTATUSSET0, 4);
        printk(KERN_NOTICE "SET GPIO IRQSTATUSSET0 %d\n", gpio_id);
    }
    else
    {
        printk(KERN_NOTICE "UNABLE TO SET VAL %d TO IRQSTATUSSET\n", val);
        return;
    }

    iowrite32(mask, target_reg);
}

static void enable_detect(int gpio_id)
{
    setval_rising_detect(gpio_id, 1);
    setval_falling_detect(gpio_id, 1);
}

static void disable_detect(int gpio_id)
{
    setval_rising_detect(gpio_id, 0);
    setval_falling_detect(gpio_id, 0);
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
                               NULL,
                               IRQF_SHARED,
                               "pinmux_ex",
                               filp);

    if (res == 0)
    {
        printk(KERN_NOTICE "Request IRQ %d SUCCESS\n", irq);
    } 
    else {
        printk(KERN_NOTICE "Request IRQ %d FAIL\n", irq);
    }

    /* Enable irq generation */
    setval_irqstatus_set(gpio_id, 1);
    enable_detect(gpio_id);

    // res = request_threaded_irq(BBB_GPIOINT1B,
    //                            gpio_interrupt_handler,
    //                            gpio_threaded_interrupt_handler,
    //                            IRQF_SHARED,
    //                            "pinmux_ex",
    //                            filp);

    // if (res == 0)
    // {
    //     printk(KERN_NOTICE "Request IRQ BBB_GPIOINT1B FAIL\n");
    // }
    // else {
    //     printk(KERN_NOTICE "Request IRQ BBB_GPIOINT1B SUCCESS\n");
    // }

}

static void free_gpio_interrupt(int gpio_id, struct file* filp)
{
    /* Disable irq generation */
    setval_irqstatus_set(gpio_id, 0);
    disable_detect(gpio_id);

    free_irq(BBB_GPIOINT1A, filp);
}

ssize_t pinmux_ex_read(struct file* filp,
                       char __user* ubuf,
                       size_t count,
                       loff_t* fpos)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV READ CALL\n");
    return count;
}

ssize_t pinmux_ex_write(struct file* filp,
                        const char __user* ubuf,
                        size_t count,
                        loff_t* fpos)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV WRITE CALL\n");
    return count;
}

int pinmux_ex_open(struct inode* inodp,
                   struct file* filp)
{
    struct pinmux_ex_data* drv_data;

    printk(KERN_NOTICE "BBB PINMUX TESTDRV OPEN CALL\n");

    drv_data = container_of(inodp->i_cdev, struct pinmux_ex_data, cdev);
    filp->private_data = drv_data;

    return 0;
}

int pinmux_ex_release(struct inode* inodp,
                      struct file* filp)
{
    printk(KERN_NOTICE "BBB PINMUX TESTDRV RELEASE CALL\n");
    return 0;
}

struct file_operations pinmux_ex_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = pinmux_ex_ioctl,
    .open = pinmux_ex_open,
    .release = pinmux_ex_release,
    .read = pinmux_ex_read,
    .write = pinmux_ex_write
};

static int pinmux_ex_init(void)
{
    int err, i;
    printk(KERN_NOTICE "BBB PINMUX Test driver init\n");

    err = register_chrdev_region(MKDEV(PINMUX_EX_MAJOR, 0),
                                 PINMUX_EX_MAX_MINORS,
                                 "gpio_example_driver");
    if (err != 0)
        return err;
    printk(KERN_NOTICE "TRY CDEV INIT\n");

    for (i = 0; i < PINMUX_EX_MAX_MINORS; i++)
    {
        cdev_init(&devs[i].cdev, &pinmux_ex_fops);
        cdev_add(&devs[i].cdev, MKDEV(PINMUX_EX_MAJOR, i), 1);
    }
    printk(KERN_NOTICE "CDEV INIT DONE\n");

    /* Set pinmux mode 7(GPIO) */
    set_pinmux((unsigned long)PINCTRL_P8_03, 7);
    printk(KERN_NOTICE "PINMUX P8_03 INIT DONE\n");
    set_pinmux((unsigned long)PINCTRL_P8_04, 7);
    printk(KERN_NOTICE "PINMUX P8_04 INIT DONE\n");
    set_pinmux((unsigned long)PINCTRL_P8_05, 7);
    printk(KERN_NOTICE "PINMUX P8_05 INIT DONE\n");
    set_pinmux((unsigned long)PINCTRL_P8_06, 7);
    printk(KERN_NOTICE "PINMUX P8_06 INIT DONE\n");

    return 0;
}

static void pinmux_ex_exit(void)
{
    printk(KERN_NOTICE "BBB PINMUX Test driver exit\n");
}

module_init(pinmux_ex_init);
module_exit(pinmux_ex_exit);