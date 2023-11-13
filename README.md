# voile-gpio-rp2040
GPIO operation library for rp2040 in voile library

For more informations about voile library, see [voile-library](https://github.com/Jimmy39/voile-library).

## Dependence

This library use with:

[voile-class-iopin](https://github.com/Jimmy39/voile-class-iopin)

[voile-library](https://github.com/Jimmy39/voile-library)

## Class voile_hardware_ioPin_gpioRp2040_t

This library defines struct voile_hardware_ioPin_gpioRp2040_t:

```C
/**
 * @brief Single io for gpio in rp2040
 * 
 */
struct voile_hardware_ioPin_gpioRp2040_t{
    const struct voile_ioPinOperations_t *Operation;    ///< Functions of ioPin
    const uint8_t pin;                                  ///< Pin number
};
```
### Define

You should define and init struct at _devicelist.c_

```C
#include "voile_gpio_rp2040.h"

struct voile_hardware_ioPin_gpioRp2040_t myIO = {
    .Operation = &voile_ioPinOperations_gpio_rp2040,
    .pin = 25       // you pin number
};
```

### Declare

And declare at _devicelist.h_ as __struct voile_ioPin_t__

```C
#include "voile_class_iopin.h"

extern struct voile_ioPin_t myIO;
```

struct voile_ioPin_t is the public part of struct voile_hardware_ioPin_gpioRp2040_t, defined in voile_class_iopin.h.

### Usage

See [voile-class-iopin](https://github.com/Jimmy39/voile-class-iopin)
