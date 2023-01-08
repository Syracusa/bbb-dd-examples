# BBB-DD-EXAMPLES
 - Beaglebone black device driver examples.



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


## GPIO example(./gpio_ex, ongoing)
- Beaglebone black gpio control example(Character device driver).
- No kernel gpio interface used.

## Character device driver example(./char_ex/)
- Simple Character device driver example that read/write to kernel memory.
