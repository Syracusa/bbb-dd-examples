#ifndef BBB_PINMUX_H
#define BBB_PINMUX_H

#define CONTORL_MODULE_BASE  0x44E12000

#define PINCTRL_P8_03 (CONTORL_MODULE_BASE + 0x818) /* gpmc_ad6 */
#define PINCTRL_P8_04 (CONTORL_MODULE_BASE + 0x81c) /* gpmc_ad7 */
#define PINCTRL_P8_05 (CONTORL_MODULE_BASE + 0x808) /* gpmc_ad2 */
#define PINCTRL_P8_06 (CONTORL_MODULE_BASE + 0x80c) /* gpmc_ad3 */

#endif