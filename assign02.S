#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"


.syntax unified                                                 @ Specify unified assembly syntax
.cpu    cortex-m0plus                                           @ Specify CPU type is Cortex M0+
.thumb                                                          @ Specify thumb assembly for RP2040
.global main_asm                                                @ Provide program starting address to the linker
.align 4                                                        @ Specify code alignment

.equ    GPIO_BTN,       21          @ Specify pin for the button
.equ    GPIO_LED_PIN,   25          @ Specify pin for the built in LED
.equ    GPIO_DIR_IN,   0            @ Specify input direction for a GPIO pin
.equ    GPIO_DIR_OUT,  1            @ Specify output direction for a GPIO pin


@ Entry point to the ASM portion of the program
main_asm:
    bl      init_btn                                           @ Branch to initialise buttons
    wfi
    b       main_asm                                           @ Infinite loop


init_btn:
    push    {lr}                        @ Push link register to stack
    movs    r0, #GPIO_BTN               @ This value is the button on the PI PICO
    bl      asm_gpio_init               @ Call the subroutine to initalise the pin
    movs    r0, #GPIO_BTN               @ This value is the button on the PI PICO
    movs    r1, GPIO_DIR_IN             @ Set button as input
    bl      asm_gpio_set_dir            @ Call subroutine to set it as an input pin
    movs    r0, #GPIO_BTN               @ Enable the interrupt
    bl      asm_gpio_set_irq            @ Call suburoutine to enable interrupt

@ Set data alignment
.data
    .align 4
