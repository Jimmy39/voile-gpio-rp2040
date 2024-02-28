/**
 * @file voile_iopin_rp2040.c
 * @author JimmyWang
 * @brief Define class ioPin for rp2040
 * @version alpha-240228
 * 
 */

#include "voile_pin_rp2040.h"

static voile_io_mode_t voile_static_pin_getMode(
    voile_const_internal_pin_rp2040_t *this);

const struct voile_pin_interface_struct voile_const_pin_rp2040 = {
    .init    = (void (*)(voile_const_pin_t *))voile_pin_rp2040_init,
    .setMode = (voile_io_mode_t(*)(
        voile_const_pin_t *, voile_io_mode_t))voile_pin_rp2040_setMode,
    .write   = (void (*)(voile_const_pin_t *, bool))voile_pin_rp2040_write,
    .read    = (bool (*)(voile_const_pin_t *))voile_pin_rp2040_read,
    .toggle  = (void (*)(voile_const_pin_t *))voile_pin_rp2040_taggle,
    .readRegister =
        (bool (*)(voile_const_pin_t *))voile_pin_rp2040_readRegister};


uint32_t IO_RP2040_IsOpenDrainMask = 0;

void voile_pin_rp2040_init(voile_const_internal_pin_rp2040_t *this) {

    // Defination cheak
    if (this->pin > 32) {
        microVerrorno_error(configError, "message");
        return;
    }

    voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.IE         = 1;
    voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.OD         = 0;
    voile_rp2040_IO_BANK0->GPIO[this->pin].CTRL.selectBit.FUNCSEL = 5;
}

voile_io_mode_t voile_pin_rp2040_setMode(
    voile_const_internal_pin_rp2040_t *pin, voile_io_mode_t mode) {

    uint32_t mask = 1ul << pin->pin;

    microVerrorno_cheakInput(
        (uint8_t)mode < (uint8_t)IOmodeError, message,
        voile_static_pin_getMode(pin));
    if (mode == IOmodeHold) {
        return voile_static_pin_getMode(pin);
    }

    // Pull-up and pull-down resistor set
    switch (mode) {

        // No pull
        case IOuninit:
        case IOmodeInput:
        case IOmodePushPull:
        case IOmodeOpenDrain:
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PDE = 0;
            break;

        // Pull-up
        case IOmodeInputWithPullUpResistor:
        case IOmodeQuasiBidirectional:
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PUE = 1;
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PDE = 0;
            break;

        // Pull-down
        case IOmodeInputWithPullDownResistor:
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[pin->pin].selectBit.PDE = 1;
            break;

        default:
            microVerrorno_error(hardwareUnsupportedError, message);
            return voile_static_pin_getMode(pin);
    }


    // Switch gpio_out and gpio_oe
    switch (mode) {

        // Write output value to gpio_out
        case IOmodePushPull:
            if (IO_RP2040_IsOpenDrainMask & mask) {
                if (voile_rp2040_SIO->GPIO_OE & mask) {
                    voile_rp2040_SIO->GPIO_OUT_SET = mask;
                } else {
                    voile_rp2040_SIO->GPIO_OUT_CLR = mask;
                }
                IO_RP2040_IsOpenDrainMask &= ~mask;
            }
            voile_rp2040_SIO->GPIO_OE_SET = mask;
            break;

        // Input mode is also write output value to gpio_out
        case IOmodeInput:
        case IOmodeInputWithPullUpResistor:
        case IOmodeInputWithPullDownResistor:
            if (IO_RP2040_IsOpenDrainMask & mask) {
                if (voile_rp2040_SIO->GPIO_OE & mask) {
                    voile_rp2040_SIO->GPIO_OUT_CLR = mask;
                } else {
                    voile_rp2040_SIO->GPIO_OUT_SET = mask;
                }
                IO_RP2040_IsOpenDrainMask &= ~mask;
            }
            voile_rp2040_SIO->GPIO_OE_CLR = mask;
            break;

        // Write output value to gpio_oe
        case IOmodeOpenDrain:
        case IOmodeQuasiBidirectional:
            if (!(IO_RP2040_IsOpenDrainMask & mask)) {
                if (voile_rp2040_SIO->GPIO_OUT & mask) {
                    voile_rp2040_SIO->GPIO_OE_CLR = mask;
                } else {
                    voile_rp2040_SIO->GPIO_OE_SET = mask;
                }
                IO_RP2040_IsOpenDrainMask |= mask;
            }
            voile_rp2040_SIO->GPIO_OUT_CLR = mask;
            break;

        default:
            break;
    }

    return IOmodeError;
}

static voile_io_mode_t voile_static_pin_getMode(
    voile_const_internal_pin_rp2040_t *this) {
    if (voile_rp2040_IO_BANK0->GPIO[this->pin].CTRL.selectBit.FUNCSEL != 5) {
        return IOuninit;
    }
    if (IO_RP2040_IsOpenDrainMask & (1ul << this->pin)) {
        if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE) {
            return IOmodeQuasiBidirectional;
        } else {
            return IOmodeOpenDrain;
        }
    } else {
        if (voile_rp2040_SIO->GPIO_OE & (1ul << this->pin)) {
            return IOmodePushPull;
        } else {
            if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE) {
                return IOmodeInputWithPullUpResistor;
            } else if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PDE) {
                return IOmodeInputWithPullDownResistor;
            } else {
                return IOmodeInput;
            }
        }
    }
}
