/**
 * @file voile_gpio_rp2040.c
 * @author JimmyWang
 * @brief 
 * @version	alpha20231015
 * @date 20231015
 */ 

#include "voile_gpio_rp2040.h"

const struct voile_ioPinOperations_t voile_ioPinOperations_gpio_rp2040 = {
    .Init = (enum voileFunctionReturn (*)(void *, enum voileIOmode, ...))ioPin_gpio_rp2040_Init,
    .Write = (enum voileFunctionReturn (*)(void *, bool))ioPin_gpio_rp2040_Write,
    .Read = (enum voileFunctionReturn (*)(void *, bool *))ioPin_gpio_rp2040_Read,
    .ReadToReturn = (bool (*)(void *))ioPin_gpio_rp2040_ReadToReturn,
    .Toggle = (enum voileFunctionReturn (*)(void *))ioPin_gpio_rp2040_Taggle
};

uint32_t IO_RP2040_IsOpenDrainMask = 0;

enum voileFunctionReturn ioPin_gpio_rp2040_Init(struct voile_hardware_ioPin_gpioRp2040_t *ioPin_p, enum voileIOmode mode, bool value){
    uint32_t mask = 1ul << ioPin_p->pin;
    if(ioPin_p->pin > 32){
        return hardwareUnsupportedError;
    }
    switch(mode){
        case IOmodeInput:
            gpio_rp2040_sio->gpio_oe_clr = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeInputWithPullUpResistor:
            gpio_rp2040_sio->gpio_oe_clr = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 3;     // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeInputWithPullDownResistor:
            gpio_rp2040_sio->gpio_oe_clr = mask;                        // disable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 2;     // enable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            break;
        case IOmodeQuasiBidirectional:
            gpio_rp2040_sio->gpio_out_clr = mask;                       // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                          // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 3;     // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            ioPin_gpio_rp2040_Write(ioPin_p, value);                    // write value
            break;
        case IOmodePushPull:
            IO_RP2040_IsOpenDrainMask &= ~mask;                         // write value to gpio_out
            gpio_rp2040_sio->gpio_oe_set = mask;                        // enable output
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            ioPin_gpio_rp2040_Write(ioPin_p, value);                    // write value
            break;
        case IOmodeOpenDrain:
            gpio_rp2040_sio->gpio_out_clr = mask;                       // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                          // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 3);  // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 2);  // disable pull down resistor
            ioPin_gpio_rp2040_Write(ioPin_p, value);                    // write value
            break;
        default:
            return hardwareUnsupportedError;
    }
    gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] |= 1ul << 6;     // input enable
    gpio_rp2040_pads_bank0->gpio[ioPin_p->pin] &= ~(1ul << 7);  // output enable
    gpio_rp2040_io_bank0->gpio[ioPin_p->pin].ctrl = 5;          // gpio mux sio
    return success;
}

