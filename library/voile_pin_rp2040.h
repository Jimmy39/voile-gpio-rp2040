/**
 * @file voile_iopin_rp2040.h
 * @author JimmyWang
 * @brief Define class ioPin for rp2040
 * @version alpha-240228
 * 
 */
#ifndef __VOILE_IOPIN_RP2040_H__
#define __VOILE_IOPIN_RP2040_H__

#include "voile_errorno.h"
#include "voile_interface_pin.h"
#include "voile_register_rp2040.h"

/**
 * @brief Single io for gpio in rp2040
 * 
 */
typedef const struct {

    voile_const_pin_t interface;

    // Pin number
    uint8_t pin;

} voile_const_internal_pin_rp2040_t;

// The functions operate the pin
extern const struct voile_pin_interface_struct voile_const_pin_rp2040;

// if 0 write output value to gpio_out, 1 write output value to gpio_oe
extern uint32_t IO_RP2040_IsOpenDrainMask;

void voile_pin_rp2040_init(voile_const_internal_pin_rp2040_t *);

voile_io_mode_t voile_pin_rp2040_setMode(
    voile_const_internal_pin_rp2040_t *, voile_io_mode_t);

static inline void voile_pin_rp2040_write(
    voile_const_internal_pin_rp2040_t *pin_p, bool value) {
    if (value) {
        voile_rp2040_SIO->GPIO_OUT_SET =
            (1ul << pin_p->pin) & (~IO_RP2040_IsOpenDrainMask);
        voile_rp2040_SIO->GPIO_OE_CLR =
            (1ul << pin_p->pin) & (IO_RP2040_IsOpenDrainMask);
    } else {
        voile_rp2040_SIO->GPIO_OUT_CLR =
            (1ul << pin_p->pin) & (~IO_RP2040_IsOpenDrainMask);
        voile_rp2040_SIO->GPIO_OE_SET =
            (1ul << pin_p->pin) & (IO_RP2040_IsOpenDrainMask);
    }
}

static inline bool voile_pin_rp2040_read(
    voile_const_internal_pin_rp2040_t *ioPin_p) {
    return !!((1ul << ioPin_p->pin) & voile_rp2040_SIO->GPIO_IN);
}

static inline void voile_pin_rp2040_taggle(
    voile_const_internal_pin_rp2040_t *ioPin_p) {
    voile_rp2040_SIO->GPIO_OUT_XOR =
        (1ul << ioPin_p->pin) & (~IO_RP2040_IsOpenDrainMask);
    voile_rp2040_SIO->GPIO_OE_XOR =
        (1ul << ioPin_p->pin) & (IO_RP2040_IsOpenDrainMask);
}

static inline bool voile_pin_rp2040_readRegister(
    voile_const_internal_pin_rp2040_t *this) {
    if (IO_RP2040_IsOpenDrainMask & (1ul << this->pin)) {
        return !(voile_rp2040_SIO->GPIO_OE & (1ul << this->pin));
    } else {
        return !!(voile_rp2040_SIO->GPIO_OUT & (1ul << this->pin));
    }
}

#endif  // !__VOILE_IOPIN_RP2040_H__
