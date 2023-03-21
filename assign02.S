#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"
.syntax unified                                                 @ Specify unified assembly syntax
.cpu    cortex-m0plus                                           @ Specify CPU type is Cortex M0+
.thumb                                                          @ Specify thumb assembly for RP2040
.global main_asm                                                @ Provide program starting address to the linker
.align 4          


.equ    DFLT_STATE_STRT, 1            @ Specify the value to start flashing
.equ    DFLT_STATE_STOP, 0            @ Specify the value to stop flashing
.equ    DFLT_ALARM_TIME, 1000000      @ Specify the default alarm timeout
.equ    GPIO_LOC, 8192
.equ    GPIO_EVNT_MSK, 0x20000

.equ    GPIO_BTN_DN_MSK, 0x00040000   @ Bit-18 for falling-edge event on GP20 (if started, halve time, else set to default time)
.equ    GPIO_BTN_EN_MSK, 0x00400000   @ Bit-22 for falling-edge event on GP21 (start/stop)
.equ    GPIO_BTN_UP_MSK, 0x04000000   @ Bit-26 for falling-edge event on GP22 (if started, double time, else set to default time)

.equ    GPIO_BTN_DN,  20              @ Specify pin for the "down" button
.equ    GPIO_BTN_EN,  21              @ Specify pin for the "enter" button
.equ    GPIO_BTN_UP,  22              @ Specify pin for the "up" button
.equ    GPIO_LED_PIN, 25              @ Specify pin for the built-in LED
.equ    GPIO_DIR_IN,   0              @ Specify input direction for a GPIO pin
.equ    GPIO_DIR_OUT,  1              @ Specify output direction for a GPIO pin

.equ    LED_VAL_ON,    1              @ Specify value that turns the LED "on"
.equ    LED_VAL_OFF,   0              @ Specify value that turns the LED "off"

.equ    GPIO_ISR_OFFSET, 0x74         @ GPIO is int #13 (vector table entry 29)
.equ    ALRM_ISR_OFFSET, 0x40         @ ALARM0 is int #0 (vector table entry 16)



@ Entry point to the ASM portion of the program
main_asm:
    ldr     r0, =msg
    bl      printf
    bl      initialise_gpio           @ Initialise GPIO (Init, Direct, IRQ)                                                    
    bl      install_alarm             @ Install ALARM ISR 
    bl      install_gpio              @ Install GPIO ISR
    bl      default_alarm             @ Reset alarm to default time

    @ Start Alarm:
    ldr     r2, = (TIMER_BASE + TIMER_INTE_OFFSET)                         
    movs    r1, #1                                               
    str     r1, [r2]                          
loop:
    wfi                                                                     @ wait for interrupt
    b       loop                                                            


	

@ Call the GPIO initialization function for all the required buttons and set up the falling edge
initialise_gpio:
    push    {lr}

    movs    r0, #GPIO_LED_PIN           @ This value is the GPIO LED pin on the PI PICO board
    bl      asm_gpio_init               @ Call the subroutine to initialise the GPIO pin specified by r0
    movs    r0, #GPIO_LED_PIN           @ This value is the GPIO LED pin on the PI PICO board
    movs    r1, #GPIO_DIR_OUT           @ We want this GPIO pin to be setup as an output pin
    bl      asm_gpio_set_dir            @ Call the subroutine to set the GPIO pin specified by r0 to state specified by r1

    movs    r0, #GPIO_BTN_EN           @ This value is the GPIO EN BTN pin on the PI PICO board
    bl      asm_gpio_init               @ Call the subroutine to initialise the GPIO pin specified by r0
    movs    r0, #GPIO_BTN_EN           @ This value is the GPIO EN BTN pin on the PI PICO board
    movs    r1, #GPIO_DIR_IN           @ We want this GPIO pin to be setup as an input pin
    bl      asm_gpio_set_dir            @ Call the subroutine to set the GPIO pin specified by r0 to state specified by r1
    movs r0, #GPIO_BTN_EN
    bl asm_gpio_set_irq                 @Call the subroutine to enable falling-edge interrupt for GPIO pin specified by r0

        movs    r0, #GPIO_BTN_UP           @ This value is the GPIO UP BTN pin on the PI PICO board
    bl      asm_gpio_init               @ Call the subroutine to initialise the GPIO pin specified by r0
    movs    r0, #GPIO_BTN_UP           @ This value is the GPIO UP BTN pin on the PI PICO board
    movs    r1, #GPIO_DIR_IN           @ We want this GPIO pin to be setup as an input pin
    bl      asm_gpio_set_dir            @ Call the subroutine to set the GPIO pin specified by r0 to state specified by r1
    movs r0, #GPIO_BTN_UP
    bl asm_gpio_set_irq                 @Call the subroutine to enable falling-edge interrupt for GPIO pin specified by r0

    movs    r0, #GPIO_BTN_DN          @ This value is the GPIO DN BTN pin on the PI PICO board
    bl      asm_gpio_init               @ Call the subroutine to initialise the GPIO pin specified by r0
    movs    r0, #GPIO_BTN_DN           @ This value is the GPIO DN BTN pin on the PI PICO board
    movs    r1, #GPIO_DIR_IN           @ We want this GPIO pin to be setup as an input pin
    bl      asm_gpio_set_dir            @ Call the subroutine to set the GPIO pin specified by r0 to state specified by r1
    movs r0, #GPIO_BTN_DN
    bl asm_gpio_set_irq                 @Call the subroutine to enable falling-edge interrupt for GPIO pin specified by r0

    pop     {pc}


install_gpio:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET) 
    ldr     r1, [r2] 
    movs    r2, #GPIO_ISR_OFFSET 
    add     r2, r1 
    ldr     r0, =gpio_func 
    str     r0, [r2]
    ldr    r1, =GPIO_LOC
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r1, [r2]
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    ldr    r1, =GPIO_LOC
    str     r1, [r2]
    bx      lr 


install_alarm:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)                                
    ldr     r1, [r2]                                                           
    movs    r2, #ALRM_ISR_OFFSET                                              
    add     r2, r1                                                             
    ldr     r0, =alarm_func                                       
    str     r0, [r2]                                                            
    movs    r1, #1
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r1 , [r2]
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    movs    r1, #1
    str     r1, [r2]
    bx      lr     

default_alarm:
    push {lr}
    ldr     r7, =DFLT_ALARM_TIME      @ Set to default time (R7 is the register which we will use to pass to our set_alarm function)                                
    bl      set_alarm                 @ Set alarm.
    pop {pc}


set_alarm:
    ldr     r1, =(TIMER_BASE + TIMER_TIMELR_OFFSET)                            
    ldr     r2, [r1]                                                           
    movs    r1, r7                                                           
    add     r1, r1, r2                                                         
    ldr     r2, =(TIMER_BASE + TIMER_ALARM0_OFFSET)                           
    str     r1, [r2]                                        
    bx      lr      


.thumb_func
gpio_func:
	push {lr}
	pop {pc}






.align 4
msg:    .asciz "Hello World!\n"
btn_msg:    .asciz "Button Pressed!"
alarm_msg:    .asciz "Toggling LED! (Alarm occured)"

.data
lstate: .word DFLT_STATE_STRT
ltimer: .word DFLT_ALARM_TIME
clockTime: .word SLEEP_TIME
