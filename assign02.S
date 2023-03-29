.syntax unified                                                 @ Specify unified assembly syntax
.cpu    cortex-m0plus                                           @ Specify CPU type is Cortex M0+
.thumb                                                          @ Specify thumb assembly for RP2040
.global main_asm                                                @ Provide program starting address to the linker
.align 4                                                        @ Specify code alignment


@ Entry point to the ASM portion of the program
main_asm:
    b       main_asm                                           @ Infinite loop


@ Set data alignment
.data
    .align 4
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"

.syntax unified
.cpu    cortex-m0plus
.thumb
.global main_asm
.align  4


.equ    DFLT_STATE_STRT, 1            @ Specify the value to start flashing
.equ    DFLT_STATE_STOP, 0            @ Specify the value to stop flashing
.equ    DFLT_ALARM_TIME, 1000000      @ Specify the default ALARM timeout


.equ    GPIO_BTN_FALL_MSK, 0x00400000   @ Bit-22 for falling-edge event on GP21
.equ    GPIO_BTN_RISE_MSK, 0x00800000   @ Bit-26 for falling-edge event on GP22

.equ    GPIO_BTN_EN,  21              @ Specify pin for the "enter" button
.equ    GPIO_LED_PIN, 25              @ Specify pin for the built-in LED
.equ    GPIO_DIR_IN,   0              @ Specify input direction for a GPIO pin
.equ    GPIO_DIR_OUT,  1              @ Specify output direction for a GPIO pin

.equ    LED_VAL_ON,    1              @ Specify value that turns the LED "on"
.equ    LED_VAL_OFF,   0              @ Specify value that turns the LED "off"

.equ    DOT_TIME,    1000000              @ Specify value that turns the LED "on"
.equ    DASH_TIME,   3000000            @ Specify value that turns the LED "off"
.equ    EMPTY_TIME,  1000000              @ Specify value that turns the LED "on"
.equ    ENTER_TIME,  1000000              @ Specify value that turns the LED "on"


.equ    GPIO_ISR_OFFSET, 0x74         @ GPIO is int #13 (vector table entry 29)
.equ    ALRM_ISR_OFFSET, 0x40         @ ALARM0 is int #0 (vector table entry 16)

@ Entry point to the ASM portion of the program
main_asm:
    bl      init_gpio_led                               @Initialise the GPIO LED pin                            @ Install each of the GPIO buttons
    bl      init_gpio_en                         @Install the SVC interrupt service routine
    bl      install_gpio_isr
main_loop:
    wfi                                                 @waiting for an interupt 
    b       main_loop                                   @return to start of loop





@ Subroutine used to initialise the PI Pico built-in LED
init_gpio_led:
    push    {lr}                                        @initialising GPIO LED by moving pin no to r0 and setting direction as output
    movs    r0, #GPIO_LED_PIN                           @ moving the pin number into register 0
    bl      asm_gpio_init                               @calling function to  initialising the pin
    movs    r0, #GPIO_LED_PIN                   
    movs    r1, #GPIO_DIR_OUT                           @moving the direction we want into register 1
    bl      asm_gpio_set_dir                            @ calling function to setting the direction       
    pop     {pc}                                        
                                                        @ similar operation for all buttons

init_gpio_en:
    push    {lr}                                        @initialising GPIO BTN by moving pin no to r0 and setting direction as input
    movs    r0, #GPIO_BTN_EN
    bl      asm_gpio_init
    movs    r0, #GPIO_BTN_EN
    movs    r1, #GPIO_DIR_IN
    bl      asm_gpio_set_dir                            
    movs    r0, #GPIO_BTN_EN
    bl      asm_gpio_set_irq
    bl      asm_gpio_set_irq2
    pop     {pc}

@ Subroutine used to install the gpio interrupt service handler
install_gpio_isr:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)        @Similar to the alarm_isr, it initialises the interupt disable and enables for GPIO buttons
    ldr     r1, [r2]                                    
    movs    r2, #GPIO_ISR_OFFSET                        
    adds    r1, r2                                      
    ldr     r0, =gpio_isr                               
    str     r0, [r1]                                    
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)   
    ldr     r1, =#0x2000                                
    str     r1, [r2]                                    
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)   
    ldr     r1, =#0x2000                                
    str     r1, [r2]                                    
    bx      lr                                          

@ gpio interrupt service handler routine
.thumb_func                     
gpio_isr:
    push    {lr}                                                    @enters the routine
    ldr     r2, =(IO_BANK0_BASE + IO_BANK0_PROC0_INTS2_OFFSET)      @enters interupt register to determine interupts
    ldr     r0, [r2]                                                @stores correct value to init
    ldr     r1, =0x2000
    str     r1, [r2]                                                
    ldr     r2, = (TIMER_BASE + TIMER_INTE_OFFSET)                  @ stores correct value to enable the interupt
    str     r1, [r2]            
    ldr     r1, =(GPIO_BTN_FALL_MSK)                                  @ loads value of falling edge for dwn btn and compares to interupt
    cmp     r0, r1
    beq     gpiofall                                                @if its equal enter the down routine
    ldr     r1, =(GPIO_BTN_RISE_MSK)                                  @loads value of falling edge for up btn and compares to interupt
    cmp     r0, r1
    beq     gpiorise                                                 @if its equal enter the up routine                                             @if its equal enter the engage routine
    b       done                                                    @ else exit

gpiofall:
    ldr     r2, =(TIMER_BASE + TIMELR)
    ldr     r0, [r2]
    bl      done                                                    @goes to done

gpiorise:
    ldr     r2, =(TIMER_BASE + TIMELR) 
    ldr     r1, [r2]
    sub     r1, r0
    cmp     r1, DOT_TIME
    blt     dot     
    bl      done      
dot: 
    bl      printdot 
    bl      done     

dash: 
    bl      printdash 
    bl      done   

space: 
    bl      printspace
    bl      done     
                                    @ <TODO – add comment>
done:                                                @proc the alarm routine 
    pop     {pc}                                                    @exit the routine


.align 4
msgdot:    .asciz "."
msgdash:   .asciz "-"
msgsapce:   .asciz " "



.data
lstate: .word   DFLT_STATE_STRT
ltimer: .word   DFLT_ALARM_TIME
