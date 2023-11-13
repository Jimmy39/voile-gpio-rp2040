#ifndef __VOILE_GPIO_RP2040_H__
#define __VOILE_GPIO_RP2040_H__

#include "voile_class_iopin.h"

typedef uint8_t ioPin_pin_t;
typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;


/**
 * @brief Single io for gpio in rp2040
 * 
 */
struct voile_hardware_ioPin_gpioRp2040_t{
    const struct voile_ioPinOperations_t *Operation;    ///< Functions of ioPin
    const uint8_t pin;                                  ///< Pin number
};

extern const struct voile_ioPinOperations_t voile_ioPinOperations_gpio_rp2040;

/*********Register definition*********/

// Reference to datasheet: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf#tab-registerlist_sio
struct gpio_rp2040_sio_t{
    uint32_t unused0; // CPUID
    io_ro_32 gpio_in;
    uint32_t unused1; // GPIO_HI_IN
    uint32_t unused2;
    io_rw_32 gpio_out;
    io_wo_32 gpio_out_set;
    io_wo_32 gpio_out_clr;
    io_wo_32 gpio_out_xor;
    io_rw_32 gpio_oe;
    io_wo_32 gpio_oe_set;
    io_wo_32 gpio_oe_clr;
    io_wo_32 gpio_oe_xor;
    //other unused
};

#define gpio_rp2040_sio ((struct gpio_rp2040_sio_t *)0xd0000000u)

// Reference to datasheet: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf#tab-registerlist_pads_bank0
struct gpio_rp2040_pads_bank0_t{
    io_rw_32 voltage_select;
    io_rw_32 gpio[30u];
};

#define gpio_rp2040_pads_bank0 ((struct gpio_rp2040_pads_bank0_t *)0x4001c000u)

// Reference to datasheet: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf#tab-registerlist_io_bank0
struct gpio_rp2040_io_bank0_gpio_t{
    io_ro_32 status;
    io_rw_32 ctrl;
};
struct gpio_rp2040_io_bank0_t{
    struct gpio_rp2040_io_bank0_gpio_t gpio[30];
    //other unused
};

#define gpio_rp2040_io_bank0 ((struct gpio_rp2040_io_bank0_t *)0x40014000u)

/*********Variable definition*********/


extern uint32_t IO_RP2040_IsOpenDrainMask;  // if 0 write output value to gpio_out, 1 write output value to gpio_oe


/**
 * @brief Initialise or reinitialise a IO and set IO mode and default output value
 * @ingroup ioPin_gpio_rp2040
 * 
 * @param[in] pin :[struct voile_hardware_ioPin_gpioRp2040 *]ioPin object
 * @param[in] mode :[enum voileIOmode]Which mode select to use @ref voileIOmode
 * @param[in] value :(option)[uint8_t]If 0 clear the IO, 1 set it. Input mode will ignore this param.
 * @return voileFunctionReturn
 *
 * @par Sample
 * @code
 * ioPin_gpio_rp2040_Init(3, IOmodeOpenDrain, 1);
 * @endcode
 * 
 * @details 
 * RP2040 supported: \n 
 * - The input mode \n 
 * - The input mode with pull up resistor \n 
 * - The input mode with pull down resistor \n 
 * - The quasi-bidirectional mode \n
 * - The open-drain mode  \n 
 * - The push-pull mode
 * 
 */
enum voileFunctionReturn ioPin_gpio_rp2040_Init(struct voile_hardware_ioPin_gpioRp2040_t *, enum voileIOmode, bool value);


/**
 * @brief Drive a single IO high/low
 * @ingroup ioPin_gpio_rp2040
 * 
 * @param[in] pin :[struct voile_hardware_ioPin_gpioRp2040 *]ioPin object
 * @param[in] value :[bool]If 0 clear the IO, 1 set it.
 * @return voileFunctionReturn
 *
 * @par Sample
 * @code
 * ioPin_gpio_rp2040_Write(2, 1);
 * @endcode
 *  
 */
static inline enum voileFunctionReturn ioPin_gpio_rp2040_Write(struct voile_hardware_ioPin_gpioRp2040_t *ioPin_p, bool value){
    if (value){
        gpio_rp2040_sio->gpio_out_set = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
        gpio_rp2040_sio->gpio_oe_clr = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
    }else{
        gpio_rp2040_sio->gpio_out_clr = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
        gpio_rp2040_sio->gpio_oe_set = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
    }
    return success;
}


/**
 * @brief Get state of a single specified IO
 * @ingroup ioPin_gpio_rp2040
 * 
 * @param[in] pin :[struct voile_hardware_ioPin_gpioRp2040 *]ioPin object
 * @param[out] value :[bool *]Current state of the GPIO. 0 for low, 1 for high
 * @return voileFunctionReturn
 *
 * @par Sample
 * @code
 * ioPin_Read(1, &value);
 * @endcode
 *  
 */
static inline enum voileFunctionReturn ioPin_gpio_rp2040_Read(struct voile_hardware_ioPin_gpioRp2040_t *ioPin_p, bool *value){
    *value = !!((1ul << ioPin_p->pin) & gpio_rp2040_sio->gpio_in);
    return success;
}


/**
 * @brief Get state of a single specified IO
 * @ingroup ioPin_gpio_rp2040
 * 
 * @param[in] name :[micro]ioPin device
 * @return [bool]Current state of the GPIO. 0 for low, 1 for high
 *
 * @par Sample
 * @code
 * value = ioPin_ReadToReturn(0);
 * @endcode
 *  
 */
static inline bool ioPin_gpio_rp2040_ReadToReturn(struct voile_hardware_ioPin_gpioRp2040_t *ioPin_p){
    return !!((1ul << ioPin_p->pin) & gpio_rp2040_sio->gpio_in);
}


/**
 * @brief Toggle a single io
 * 
 * @param[in] name :[struct voile_hardware_ioPin_gpioRp2040 *]ioPin object
 * @return voileFunctionReturn
 *
 * @par Sample
 * @code
 * led.Func->toggle(&myIo);
 * @endcode
 *  
 */
static inline enum voileFunctionReturn ioPin_gpio_rp2040_Taggle(struct voile_hardware_ioPin_gpioRp2040_t *ioPin_p){
    gpio_rp2040_sio->gpio_out_xor = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
    gpio_rp2040_sio->gpio_oe_xor = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
    return success;
}


#endif // !__VOILE_GPIO_RP2040_H__
