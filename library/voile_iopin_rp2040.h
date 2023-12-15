#ifndef __VOILE_IOPIN_RP2040_H__
#define __VOILE_IOPIN_RP2040_H__

#include "voile_interface_iopin.h"

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;


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


// The functions operate the pin
extern voile_const_ioPin_Operate_t voile_const_ioPin_Operate_rp2040;

// The functions get date or status from pin
extern voile_const_ioPin_Get_t voile_const_ioPin_Get_rp2040;

// if 0 write output value to gpio_out, 1 write output value to gpio_oe
extern uint32_t IO_RP2040_IsOpenDrainMask;

// This micro use to init all function 
#define VOILE_IOPIN_RP2040_FUNCINIT  \
    .Operate = &voile_const_ioPin_Operate_rp2040,    \
    .Get = &voile_const_ioPin_Get_rp2040

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




/**
 * @brief Initialise or reinitialise a IO and set IO mode and default output value
 * 
 * @param[in] this :[voile_const_internal_ioPin_rp2040_t *]This ioPin object.
 * @param[in] mode :[voile_io_mode_t]Which mode select to use @ref voile_io_mode_t.
 * @param[in] value :[bool](option when input mode)If 0 clear the IO, 1 set it. Input mode will ignore this param.
 * @return voile_status_t.
 *
 * @par Sample
 * @code {.C}
 * voile_ioPin_Operate_Init(&myIO, IOmodeOpenDrain, 1);
 * @endcode
 * 
 * @details 
 * RP2040 supported: \n 
 * - The input mode, \n 
 * - The input mode with pull up resistor, \n 
 * - The input mode with pull down resistor, \n 
 * - The quasi-bidirectional mode, \n
 * - The open-drain mode,  \n 
 * - The push-pull mode.
 * 
 */
voile_status_t voile_ioPin_Operate_Init(voile_const_internal_ioPin_rp2040_t *, voile_io_mode_t, bool value);


/**
 * @brief Drive a single IO high/low
 * 
 * @param[in] this :[voile_const_internal_ioPin_rp2040_t *]This ioPin object.
 * @param[in] value :[bool]If 0 clear the IO, 1 set it.
 * @return voile_status_t.
 *
 * @par Sample
 * @code {.C}
 * voile_ioPin_Operate_Write(&myIO, 1);
 * @endcode
 *  
 */
static inline voile_status_t voile_ioPin_Operate_Write(voile_const_internal_ioPin_rp2040_t *ioPin_p, bool value){
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
 * 
 * @param[in] this :[voile_const_internal_ioPin_rp2040_t *]This ioPin object.
 * @param[out] value :[bool *]Current state of the GPIO. 0 for low, 1 for high.
 * @return voile_status_t.
 *
 * @par Sample
 * @code {.C}
 * voile_ioPin_Operate_Read(&myIO, &value);
 * @endcode
 *  
 */
static inline voile_status_t voile_ioPin_Operate_Read(voile_const_internal_ioPin_rp2040_t *ioPin_p, bool *value){
    *value = !!((1ul << ioPin_p->pin) & gpio_rp2040_sio->gpio_in);
    return success;
}


/**
 * @brief Toggle a single io
 * 
 * @param[in] this :[voile_const_internal_ioPin_rp2040_t *]This ioPin object.
 * @return voile_status_t.
 *
 * @par Sample
 * @code {.C}
 * voile_ioPin_Operate_Taggle(&myIo);
 * @endcode
 *  
 */
static inline voile_status_t voile_ioPin_Operate_Taggle(voile_const_internal_ioPin_rp2040_t *ioPin_p){
    gpio_rp2040_sio->gpio_out_xor = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
    gpio_rp2040_sio->gpio_oe_xor = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
    return success;
}


/**
 * @brief Get state of a single specified IO
 * 
 * @param[in] this :[voile_const_internal_ioPin_rp2040_t *]This ioPin object.
 * @return [bool]Current state of the GPIO. 0 for low, 1 for high.
 *
 * @par Sample
 * @code {.C}
 * value = voile_ioPin_Get_Read(&myIO);
 * @endcode
 *  
 */
static inline bool voile_ioPin_Get_Read(voile_const_internal_ioPin_rp2040_t *ioPin_p){
    return !!((1ul << ioPin_p->pin) & gpio_rp2040_sio->gpio_in);
}


#endif // !__VOILE_IOPIN_RP2040_H__
