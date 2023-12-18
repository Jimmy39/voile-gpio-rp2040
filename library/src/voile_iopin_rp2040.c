/**
 * @file voile_gpio_rp2040.c
 * @author JimmyWang
 * @brief 
 */ 

#include "voile_iopin_rp2040.h"

voile_const_ioPin_Operate_t voile_const_ioPin_Operate_rp2040 = {
    .Init = (voile_status_t (*)(const void *, voile_io_mode_t, ...))voile_ioPin_Operate_Init,
    .Write = (voile_status_t (*)(const void *, bool))voile_ioPin_Operate_Write,
    .Read = (voile_status_t (*)(const void *, bool *))voile_ioPin_Operate_Read,
    .Toggle = (voile_status_t (*)(const void *))voile_ioPin_Operate_Taggle
};

voile_const_ioPin_Get_t voile_const_ioPin_Get_rp2040 = {
    .Read = (bool (*)(const void *))voile_ioPin_Get_Read
};


uint32_t IO_RP2040_IsOpenDrainMask = 0;

voile_status_t voile_ioPin_Operate_Init(voile_const_internal_ioPin_rp2040_t *ioPin_p, voile_io_mode_t mode, bool value) {
    uint32_t mask = 1ul << ioPin_p->pin;
    if(ioPin_p->pin > 32){
        return hardwareUnsupportedError;
    }
    switch(mode){
        case IOmodeInput:
            voile_register_rp2040_SIO->GPIO_OE_CLR = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeInputWithPullUpResistor:
            voile_register_rp2040_SIO->GPIO_OE_CLR = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 3;     // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeInputWithPullDownResistor:
            voile_register_rp2040_SIO->GPIO_OE_CLR = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 2;     // enable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeQuasiBidirectional:
            voile_register_rp2040_SIO->GPIO_OUT_CLR = mask;                       // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                          // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 3;     // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            voile_ioPin_Operate_Write(ioPin_p, value);                  // write value
            break;
        case IOmodePushPull:
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            voile_register_rp2040_SIO->GPIO_OE_SET = mask;                        // enable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            voile_ioPin_Operate_Write(ioPin_p, value);                  // write value
            break;
        case IOmodeOpenDrain:
            voile_register_rp2040_SIO->GPIO_OUT_CLR = mask;                       // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                          // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            voile_ioPin_Operate_Write(ioPin_p, value);                  // write value
            break;
        default:
            return hardwareUnsupportedError;
    }
    gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 6;     // input enable
    gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 7);  // output enable
    gpio_rp2040_io_bank0->gpio[ioPin_p->pin].ctrl = 5;          // gpio mux sio
    return success;
}

