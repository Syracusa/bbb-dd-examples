# BBB-DD-EXAMPLES
 - Beaglebone black device driver examples.



## Character device driver example(./char_ex/)
- Simple Character device driver example that read/write to kernel memory.

## PINMUX/GPIO example(./pinmux_ex/)
- Beaglebone black pinmux register control example.
- Setting some pins to gpio mode(pinmux mode 7).
- Read/Write gpio direction/value by direct register access(No kernel gpio interface) 
- Userspace app to test functionality(userapp/pinmux)

## INTERRUPT/GPIO example(./interrupt_ex/)
- Beaglebone black interrupt request example.
- Request rising/falling edge interrupt.
- Read/Write gpio direction/value by kernel gpio interface(No direct register access)
- Userspace app to test functionality(userapp/interrupt)

## Technical Refernece(Memory-Mapped IO)
 + https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf?ts=1666670382560&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FAM3358

### ARM Cortex-A8 Memory Map - 177p
### ARM Cortex-A8 Interrupts - 543p 
### Contol Module - 1448p
 + CONTROL_MODULE Registers - 1458p
 + conf_<module>_<pin> Register (offset = 800h–A34h) - 1515p
### Timers - 4435p
### GPIO - 4976p
 + GPIO Registers - 4990p

## Resources
### Beaglebone black P8 table
+ https://github.com/derekmolloy/boneDeviceTree/blob/master/docs/BeagleboneBlackP8HeaderTable.pdf
### Beaglebone black P9 table
+ https://github.com/derekmolloy/boneDeviceTree/blob/master/docs/BeagleboneBlackP9HeaderTable.pdf
### Cape Expansion Header
+ https://vadl.github.io/beagleboneblack/2016/07/29/setting-up-bbb-gpio

### Device Tree Overlay
+ https://github.com/beagleboard/bb.org-overlays/

### Extending SD Card Stroage
+ https://community.element14.com/products/devtools/single-board-computers/next-genbeaglebone/b/blog/posts/extending-micro-sd-card-space-for-beaglebone-black-use---windows-users


### ETC
+ https://elinux.org/Beagleboard:BeagleBoneBlack_Debian#U-Boot_Overlays
+ https://github.com/cdsteinkuehler/beaglebone-universal-io



+ https://engineering.stackexchange.com/questions/3980/in-a-device-tree-for-am335x-how-do-i-know-what-pinctrl-values-to-use
+ https://www.kernel.org/doc/html/v4.14/driver-api/pinctl.html
+ https://github.com/selsinork/beaglebone-black-pinmux



## PINCTRL
```
config-pin -q P8.03 gpio
config-pin P8.03 in
config-pin -q P8.03

config-pin -q P8.04 gpio
config-pin P8.04 out
config-pin -q P8.04
```

## GPIO Sysfs test
```
echo 0 >/sys/class/gpio/gpio39/value
cat /sys/class/gpio/gpio38/value
echo 1 >/sys/class/gpio/gpio39/value
cat /sys/class/gpio/gpio38/value
```