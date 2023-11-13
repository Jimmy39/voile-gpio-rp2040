/**
 * @file voile_gpio_rp2040.c
 * @author JimmyWang
 * @brief 
 * @version	V0.0.0
 * @date 202310--
 * @par Dependence
 * voile_device_iopin.h
 * @par History
 * Version|Auther|Date|Describe
 * ------|----|------|-------- 
 * V0.0.0|JimmyWang|202310--|First update.
 */ 

#include "voile_gpio_rp2040.h"

uint32_t IO_RP2040_IsOpenDrainMask = 0;

enum voileFunctionReturn IOpin_gpio_rp2040_Init(IOpin_pin_t pin, enum voileIOmode mode, bool value){
    uint32_t mask = 1ul << pin;
    if(pin > 32){
        return hardwareUnsupportedError;
    }
    switch(mode){
        case IOmodeInput:
            gpio_rp2040_sio->gpio_oe_clr = mask;                // disable output
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 3);   // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 2);   // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                 // write value to gpio_out
            break;
        case IOmodeInputWithPullUpResistor:
            gpio_rp2040_sio->gpio_oe_clr = mask;                // disable output
            gpio_rp2040_pads_bank0->gpio[pin] |= 1ul << 3;      // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 2);   // disable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                 // write value to gpio_out
            break;
        case IOmodeInputWithPullDownResistor:
            gpio_rp2040_sio->gpio_oe_clr = mask;                // disable output
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 3);   // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] |= 1ul << 2;      // enable pull down resistor
            IO_RP2040_IsOpenDrainMask &= ~mask;                 // write value to gpio_out
            break;
        case IOmodeQuasiBidirectional:
            gpio_rp2040_sio->gpio_out_clr = mask;               // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                  // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[pin] |= 1ul << 3;      // enable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 2);   // disable pull down resistor
            break;
        case IOmodePushPull:
            IO_RP2040_IsOpenDrainMask &= ~mask;                 // write value to gpio_out
            gpio_rp2040_sio->gpio_oe_set = mask;                // enable output
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 3);   // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 2);   // disable pull down resistor
            break;
        case IOmodeOpenDrain:
            gpio_rp2040_sio->gpio_out_clr = mask;               // set gpio output low
            IO_RP2040_IsOpenDrainMask |= mask;                  // write value to gpio_oe
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 3);   // disable pull up resistor
            gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 2);   // disable pull down resistor
            break;
        default:
            return hardwareUnsupportedError;
    }
    IOpin_gpio_rp2040_Write(pin, value);                // write value
    gpio_rp2040_pads_bank0->gpio[pin] |= 1ul << 6;      // input enable
    gpio_rp2040_pads_bank0->gpio[pin] &= ~(1ul << 7);   // output enable
    gpio_rp2040_io_bank0->gpio[pin].ctrl = 5;           // gpio mux sio
    return success;
}

