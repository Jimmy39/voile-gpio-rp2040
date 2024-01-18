/**
 * @file voile_iopin_rp2040.c
 * @author JimmyWang
 * @brief Define class ioPin for rp2040
 * @version alpha-240118
 * 
 */

#include "voile_iopin_rp2040.h"

voile_const_ioPin_Operate_t voile_const_ioPin_Operate_rp2040 = {
    .Init = (voile_status_t (*)(voile_const_ioPin_t *))voile_ioPin_Operate_Init,
    .SetMode = (voile_status_t (*)(voile_const_ioPin_t *, voile_io_mode_t))voile_ioPin_Operate_SetMode,
    .Write = (voile_status_t (*)(voile_const_ioPin_t *, bool))voile_ioPin_Operate_Write,
    .Read = (voile_status_t (*)(voile_const_ioPin_t *, bool *))voile_ioPin_Operate_Read,
    .Toggle = (voile_status_t (*)(voile_const_ioPin_t *))voile_ioPin_Operate_Taggle,
    .ReadRegister = (voile_status_t (*)(voile_const_ioPin_t *, bool *))voile_ioPin_Operate_ReadRegister
};

voile_const_ioPin_Get_t voile_const_ioPin_Get_rp2040 = {
    .Mode = (voile_io_mode_t (*)(voile_const_ioPin_t *))voile_ioPin_Get_Mode,
    .Read = (bool (*)(voile_const_ioPin_t *))voile_ioPin_Get_Read,
    .ReadRegister = (bool (*)(voile_const_ioPin_t *))voile_ioPin_Get_ReadRegister
};


uint32_t IO_RP2040_IsOpenDrainMask = 0;

voile_status_t voile_ioPin_Operate_Init(voile_const_internal_ioPin_rp2040_t *this) {

    // Defination cheak
    if (this->pin > 32) {
        return hardwareUnsupportedError;
    }

    voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.IE = 1;
    voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.OD = 0;
    voile_rp2040_IO_BANK0->GPIO[this->pin].CTRL.selectBit.FUNCSEL = 5;

    return success;
}


voile_status_t voile_ioPin_Operate_SetMode(voile_const_internal_ioPin_rp2040_t *this, voile_io_mode_t mode) {

    uint32_t mask = 1ul << this->pin;
    
    // Input cheak
    if ((uint8_t)mode >= (uint8_t)IOmodeError) {
        return inputRangeError;
    }

    // Pull-up and pull-down resistor set
    switch(mode) {

        // No pull
        case IOmodeInput:
        case IOmodePushPull:
        case IOmodeOpenDrain:
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PDE = 0;
        break;

        // Pull-up
        case IOmodeInputWithPullUpResistor:
        case IOmodeQuasiBidirectional:
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE = 1;
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PDE = 0;
        break;

        // Pull-down
        case IOmodeInputWithPullDownResistor:
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE = 0;
            voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PDE = 1;
        break;
        
        // Other unsupport
        default:
            return hardwareUnsupportedError;
    }

    
    // Switch gpio_out and gpio_oe
    switch(mode){

        // Write output value to gpio_out
        case IOmodePushPull:
            if (IO_RP2040_IsOpenDrainMask & mask) {
                if (voile_rp2040_SIO->GPIO_OE & mask) {
                    voile_rp2040_SIO->GPIO_OUT_SET = mask;
                }
                else {
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
                }
                else {
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
                }
                else {
                    voile_rp2040_SIO->GPIO_OE_SET = mask;
                }
                IO_RP2040_IsOpenDrainMask |= mask;
            }
            voile_rp2040_SIO->GPIO_OUT_CLR = mask;
        break;

        default:
    }

    return success;
}


voile_io_mode_t voile_ioPin_Get_Mode(voile_const_internal_ioPin_rp2040_t *this) {
    if (voile_rp2040_IO_BANK0->GPIO[this->pin].CTRL.selectBit.FUNCSEL != 5) {
        return IOuninit;
    }
    if (IO_RP2040_IsOpenDrainMask & (1ul << this->pin)) {
        if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE) {
            return IOmodeQuasiBidirectional;
        }
        else {
            return IOmodeOpenDrain;
        }
    }
    else {
        if (voile_rp2040_SIO->GPIO_OE & (1ul << this->pin)) {
            return IOmodePushPull;
        }
        else {
            if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PUE) {
                return IOmodeInputWithPullUpResistor;
            }
            else if (voile_rp2040_PADS_BANK0->GPIO[this->pin].selectBit.PDE) {
                return IOmodeInputWithPullDownResistor;
            }
            else {
                return IOmodeInput;
            }
        }
    }
}
