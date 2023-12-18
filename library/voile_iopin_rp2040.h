#ifndef __VOILE_IOPIN_RP2040_H__
#define __VOILE_IOPIN_RP2040_H__

#include "voile_interface_iopin.h"
#include "voile_register_rp2040.h"


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
        voile_register_rp2040_SIO->GPIO_OUT_SET = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
        voile_register_rp2040_SIO->GPIO_OE_CLR = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
    }else{
        voile_register_rp2040_SIO->GPIO_OUT_CLR = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
        voile_register_rp2040_SIO->GPIO_OE_SET = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
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
    *value = !!((1ul << ioPin_p->pin) & voile_register_rp2040_SIO->GPIO_IN);
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
    voile_register_rp2040_SIO->GPIO_OUT_XOR = (1ul << ioPin_p->pin)&(~IO_RP2040_IsOpenDrainMask);
    voile_register_rp2040_SIO->GPIO_OE_XOR = (1ul << ioPin_p->pin)&(IO_RP2040_IsOpenDrainMask);
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
    return !!((1ul << ioPin_p->pin) & voile_register_rp2040_SIO->GPIO_IN);
}


#endif // !__VOILE_IOPIN_RP2040_H__
