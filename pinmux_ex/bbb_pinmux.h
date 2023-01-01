#ifndef BBB_PINMUX_H
#define BBB_PINMUX_H

#include <linux/types.h>

#define CONTORL_MODULE_BASE  0x44E12000

#define PINCTRL_P8_03 (CONTORL_MODULE_BASE + 0x818) /* gpmc_ad6 - 38 - GPIO1_6 */
#define PINCTRL_P8_04 (CONTORL_MODULE_BASE + 0x81c) /* gpmc_ad7 - 39 - GPIO1_7 */
#define PINCTRL_P8_05 (CONTORL_MODULE_BASE + 0x808) /* gpmc_ad2 - 34 - GPIO1_2 */
#define PINCTRL_P8_06 (CONTORL_MODULE_BASE + 0x80c) /* gpmc_ad3 - 35 - GPIO1_3 */

struct am335x_conf_regval{
    u32 resv1 : 12; 
    u32 resv2 : 13;   
    u32 slewctrl : 1;
    u32 rxactive : 1;
    u32 pulltypesel : 1;
    u32 puden : 1;
    u32 mmode : 3;
}__attribute__((packed));

#endif