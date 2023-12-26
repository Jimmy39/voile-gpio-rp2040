# voile-gpio-rp2040
GPIO operation library for rp2040 in voile library

For more informations about voile library, see [voile-library](https://github.com/Jimmy39/voile-library).

## Dependence

This library use with:

**[voile-interface-iopin](https://github.com/Jimmy39/voile-interface-iopin)** : Define interface to use io.

**[voile_register_rp2040.h](https://github.com/Jimmy39/voile-register/blob/main/library/raspberrypi/voile_register_rp2040.h)** : Registers defination.

**[voile-library](https://github.com/Jimmy39/voile-library)** : Some common defination.

## Class voile_const_internal_ioPin_rp2040_t

This library defines struct ```voile_const_internal_ioPin_rp2040_t```:

```C
/**
 * @brief Single io for gpio in rp2040
 * 
 */
typedef const struct{

    // Operate ioPin
    voile_const_ioPin_Operate_t *Operate;

    // Get something from io
    voile_const_ioPin_Get_t *Get;
    
    // Pin number
    const uint8_t pin;

} voile_const_internal_ioPin_rp2040_t;
```
### Define

You should define and init struct at *devicelist.c*

```C
#include "voile_ioPin_rp2040.h"

voile_const_internal_ioPin_rp2040_t myIO = {
    FUNCINIT,
    .pin = 25       // you pin number
};

// some else voile_const_internal_ioPin_rp2040_t structs defination

#include "voile_ioPin_rp2040_end.h"
```

### Declare

And declare at *devicelist.h* as struct ```voile_const_ioPin_t```

```C
#include "voile_interface_iopin.h"

extern voile_const_ioPin_t myIO;
```

struct ```voile_const_ioPin_t``` is the public part of struct ```voile_const_internal_ioPin_rp2040_t```, defined in [voile-interface-iopin](https://github.com/Jimmy39/voile-interface-iopin).

### Usage

See [voile-interface-iopin](https://github.com/Jimmy39/voile-interface-iopin).
