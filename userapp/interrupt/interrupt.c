#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include <string.h>

#define READ_GPIO _IOWR('a', 'a', uint32_t*)
#define ON_GPIO _IOW('a', 'b', uint32_t*)
#define OFF_GPIO _IOW('a', 'c', uint32_t*)
#define GPIO_DIR_OUT _IOW('a', 'd', uint32_t*)
#define GPIO_DIR_IN _IOW('a', 'e', uint32_t*)

#define GPIO_ID_PIN_P8_03   38
#define GPIO_ID_PIN_P8_04   39

int main(){
    int fd = open("/dev/iex", O_RDWR);
    if (fd < 0){
        fprintf(stderr, "Can't open driver\n");
        return 0;
    }

    uint32_t val;

    /* Set input mode to p8-03 gpio pin */
    val = GPIO_ID_PIN_P8_03;
    ioctl(fd, GPIO_DIR_IN, &val);
    printf("Set GPIO P8_03 Direction IN\n");

    /* Set output mode to p8-04 gpio pin */
    val = GPIO_ID_PIN_P8_04;
    ioctl(fd, GPIO_DIR_OUT, &val);
    printf("Set GPIO P8_04 Direction OUT\n");

    /* Write 1 to p8-04 */
    val = GPIO_ID_PIN_P8_04;
    ioctl(fd, ON_GPIO, &val);
    printf("GPIO P8_04 output value to 1\n");

    /* Read from p8-03 */
    val = GPIO_ID_PIN_P8_03;
    ioctl(fd, READ_GPIO, &val);
    printf("Read GPIO P8_03 Result : %u\n", val);

    /* Write 0 to p8-04 */
    val = GPIO_ID_PIN_P8_04;
    ioctl(fd, OFF_GPIO, &val);
    printf("GPIO P8_04 output value to 0\n");

    /* Read from p8-03 */
    val = GPIO_ID_PIN_P8_03;
    ioctl(fd, READ_GPIO, &val);
    printf("Read GPIO P8_03 Result : %u\n", val);

    return 0;
}