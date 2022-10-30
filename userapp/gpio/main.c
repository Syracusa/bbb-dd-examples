#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#define GPIO_DEV "/dev/cex0"
#define GPIO_RW_SZ 16

#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

static int read_gpio(void* buf)
{
    int res = 0;
    int f = open(GPIO_DEV, 0);
    if (f)
    {
        res = read(f, buf, GPIO_RW_SZ);
        close(f);
    }   
    else
    {
        DEBUG("Fail to open %s\n", GPIO_DEV);
        exit(2);
    }
    return 0;
}

static int write_gpio(void* data)
{
    int res = 0;
    int f = open(GPIO_DEV, 0);
    if (f)
    {
        res = read(f, data, GPIO_RW_SZ);
        close(f);
    }   
    else
    {
        DEBUG("Fail to open %s\n", GPIO_DEV);
        exit(2);
    }
    return 0;
}

int main()
{
    uint32_t on[4] = {0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF};
    uint32_t off[4] = {0, 0, 0, 0};
    
    uint32_t read[4];

    while (1)
    {
        if (rand() % 2 == 1)
        {
            DEBUG("Write to 1\n");
            write_gpio(on);
        }
        else
        {
            DEBUG("Write to 0\n");
            write_gpio(off);
        }

        read_gpio(read);
        for (int i = 0; i < 4; i++)
        {
            DEBUG("Register %d : %x\n", i, read[i]);
        }
        sleep(1);
    }

    return 0;
}