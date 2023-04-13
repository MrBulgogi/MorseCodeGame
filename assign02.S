#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"

.syntax unified
.cpu    cortex-m0plus
.thumb
.global main_asm
.global Homescreen
.global level0
.align  4


.equ    DFLT_STATE_STRT, 1            @ Specify the value to start flashing
.equ    DFLT_STATE_STOP, 0              @ Specify the value to stop flashing
.equ    DFLT_ALARM_TIME, 1000000      @ Specify the default ALARM timeout


.equ    GPIO_BTN_FALL_MSK, 0x00400000   @ Bit-22 for falling-edge event on GP21
.equ    GPIO_BTN_RISE_MSK, 0x00800000   @ Bit-26 for rising-edge event on GP21

.equ    GPIO_BTN_EN,  21              @ Specify pin for the "enter" button
.equ    GPIO_LED_PIN, 25              @ Specify pin for the built-in LED
.equ    GPIO_DIR_IN,   0              @ Specify input direction for a GPIO pin
.equ    GPIO_DIR_OUT,  1              @ Specify output direction for a GPIO pin

.equ    ZERO_SEC,    0              @ Specify value that turns the LED "on"
.equ    ONE_SEC,     1              @ Specify value that turns the LED "off"
.equ    TWO_SEC,     2   

.equ    DOT_TIME,    333333             @ Specify value that should display dot
.equ    DASH_TIME,   1000000            @ Specify value that should display dash
.equ    EMPTY_TIME,  333333          @ Specify value that should display spoace
.equ    SPASE_TIME,  1000000           @ Specify value that should check code
.equ    ENTER_TIME,  2333333
.equ    MIN_TIME,    100000 



.equ    GPIO_ISR_OFFSET, 0x74         @ GPIO is int #13 (vector table entry 29)
.equ    ALRM_ISR_OFFSET, 0x40         @ ALARM0 is int #0 (vector table entry 16)

@ Entry point to the ASM portion of the program
main_asm:
    bl      init_gpio_led               @Initialise the GPIO LED pin                          
    bl      init_gpio_en                @Initialise the gpoip button
    bl      install_alrm_isr            @Initialise the alarm interput
    bl      install_gpio_isr            @Initialise the gpio interupt
    bl      init_watchdog
Homescreen: 
    bl      homescreen                  @call hoemscreen fucntion from c and display
    bl      setLED                      @set the RGB to default state
    bl      sleep                       @call sleep function from C sleep for 2 seconds
    ldr     r4, =#3                     @initiate register 4 as 3
    ldr     r5, =#0                     @initiate register 5 as 0
    bl      alarm                       @start the alrm routine
    b       ChooseLevel                 @go to ChooseLevel
    
@waiting for user to enter input
Inputting0:                             
    cmp     r5, #1                      @comparing r5 with 1, if r5 is zero user has not entered
    beq     Inputtingover0              @go to onputtingover if r5 is 1
    b       Inputting0                  @else loop back to inputting0
@user is choosing level/level0
ChooseLevel:                            
    cmp     r5, #1                      @compare register 5 with 1
    bne     Inputting0                  @if not equal user is still choosing level 
    b       ChooseLevel                 @if register 5 is 1 then wait for it to go low, as this is from previous enter
@user has entered an answer
Inputtingover0:                         
    BL      watchdog_enabler            @enable the watchdog timer
    ldr     r5, =#ZERO_SEC              @reset register 5 back to 0 for next enter
    bl      level0                      @call level0 function from c which returns number for levels.
    cmp     r0, #1                      @cmp returned number with 1
    beq     Level1                      @if equal start the level1 routine
    cmp     r0, #2                      @cmp returned number with 2
    beq     Level2                      @if equal start the level2 routine
    cmp     r0, #3                      @cmp returned number with 3
    beq     Level3                      @if equal start the level3 routine
    cmp     r0, #4                      @cmp returned number with 4
    beq     Level4                      @if equal start the level4 routine
    cmp     r0, #0
    beq     nomatch
    b       ChooseLevel                 @if none go back to choose level

nomatch:
    ldr     r0, =msgagain
    bl      printf
    b       ChooseLevel

@user is still inputting answer
Inputting:
    cmp     r5, #ONE_SEC                 @compare register 5 with 1              
    beq     Inputtingover               @if equal then user has finished entering answer
    b       Inputting                   @if not then keep looping till answer is entered
@Level 1 routine
Level1:
    bl      level1                      @call level1 function from c which procs a question for level1
    cmp     r5, #ONE_SEC                @compare register 5 with 1 
    bne     Inputting                   @if not equal then user is still entering answer
    b       Level1                      @if register 5 is 1 then wait for it to go low, as this is from previous enter
@user has entered final answer
Inputtingover:
    ldr     r5, =#ZERO_SEC               @reset register 5 back to 0 for next enter
    bl      checkEntry                  @call C function to check if user answer is right or wrong
    cmp     r0, #1                      @ funtion return 1 if user has 5 correc tin a row, compare
    beq     next1                       @if this is equal, go to next1 routine
    b       Level1                      @else go back to Level1
@user has completed the level
next1:
    bl      statistics_printout         @if level1 is complete, call C function toprint statistics
    bl      reset_levelstatistics       @call C function to reset the statistics per level
    bl      sleeplong                   @call sleep long which sleeps for 5 seconds so user can view stats
    bl      Level2header                @call C function to print the level 2 header
    b       Level2                      @go to the level 2 routine

@user is still inputting answer
Inputting2:
    cmp     r5, #ONE_SEC                 @compare register 5 with 1     
    beq     Inputtingover2              @if equal then user has finished entering answer
    b       Inputting2                  @if not then keep looping till answer is entered
@Level 2 routine
Level2:
    bl      level2                      @call level2 function from c which procs a question for level2
    cmp     r5, #ONE_SEC                @compare register 5 with 1 
    bne     Inputting2                  @if not equal then user is still entering answer
    b       Level2                      @if register 5 is 1 then wait for it to go low, as this is from previous enter
@user has entered final answer
Inputtingover2:
    ldr     r5, =#ZERO_SEC               @reset register 5 back to 0 for next enter
    bl      checkEntry                  @call C function to check if user answer is right or wrong
    cmp     r0, #1                      @ funtion return 1 if user has 5 correc tin a row, compare
    beq     next2                       @if this is equal, go to next2 routine
    b       Level2                      @else go back to Level2
next2:
    bl      statistics_printout         @if level2 is complete, call C function toprint statistics
    bl      reset_levelstatistics       @call C function to reset the statistics per level
    bl      sleeplong                   @call sleep long which sleeps for 5 seconds so user can view stats
    bl      Level3header                @call C function to print the level 3 header
    b       Level3                      @go to the level 3 routine

@user is still inputting answer
Inputting3:
    cmp     r5, #ONE_SEC                 @compare register 5 with 1     
    beq     Inputtingover3              @if equal then user has finished entering answer
    b       Inputting3                  @if not then keep looping till answer is entered
@Level 3 routine
Level3:
    bl      level3                      @call level3 function from c which procs a question for level3
    cmp     r5, #ONE_SEC                @compare register 5 with 1 
    bne     Inputting3                  @if not equal then user is still entering answer
    b       Level3                      @if register 5 is 1 then wait for it to go low, as this is from previous enter
@user has entered final answer
Inputtingover3:
    ldr     r5, =#ZERO_SEC               @reset register 5 back to 0 for next enter
    bl      checkEntry                  @call C function to check if user answer is right or wrong
    cmp     r0, #1                      @ funtion return 1 if user has 5 correc tin a row, compare
    beq     next3                       @if this is equal, go to next3 routine
    b       Level3                      @else go back to Level3
next3:
    bl      statistics_printout         @if level3 is complete, call C function toprint statistics
    bl      reset_levelstatistics       @call C function to reset the statistics per level
    bl      sleeplong                   @call sleep long which sleeps for 5 seconds so user can view stats
    bl      Level4header                @call C function to print the level 4 header
    b       Level4                      @go to the level 4 routine

@user is still inputting answer
Inputting4:
    cmp     r5, #ONE_SEC                 @compare register 5 with 1     
    beq     Inputtingover4              @if equal then user has finished entering answer
    b       Inputting4                  @if not then keep looping till answer is entered
@Level 4 routine
Level4:
    bl      level4                      @call level4 function from c which procs a question for level4
    cmp     r5, #ONE_SEC                @compare register 5 with 1 
    bne     Inputting4                  @if not equal then user is still entering answer
    b       Level4                      @if register 5 is 1 then wait for it to go low, as this is from previous enter
@user has entered final answer
Inputtingover4:
    ldr     r5, =#ZERO_SEC               @reset register 5 back to 0 for next enter
    bl      checkEntry                  @call C function to check if user answer is right or wrong
    cmp     r0, #1                      @ funtion return 1 if user has 5 correc tin a row, compare
    beq     GameFinished                @if this is equal, go to GameFinished routine
    b       Level4                      @else go back to Level4

@User has completed the game routine
GameFinished:
    bl      statistics_printout         @if game is complete, call C function toprint statistics
    bl      reset_levelstatistics       @call C function to reset the statistics per level
    bl      reset_totalstatistics       @call C function to reset the statistics for whole game
    bl      sleeplong                   @call sleep long which sleeps for 5 seconds so user can view stats
    bl      gamecompleted               @call C function which display game completed screen
    bl      sleep                       @call slep which sleeps for 2 seconds
    b       main_asm                    @loop






init_watchdog:
    push    {lr}
    ldr     r1, =(WATCHDOG_BASE + 0x04)
    movs    r0, #0xffffff 
    str     r0, [r1]
    pop     {pc}

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
    pop     {pc}

alarm:
    push    {lr}                                        @pushes return address to stack
    ldr     r2, =(TIMER_BASE + TIMER_TIMELR_OFFSET)     @loads timer component adress and 32 bitoffset adress into r2 
    ldr     r1, [r2]
    ldr     r2, =ltimer                                 @loads timing into r2
    ldr     r0, [r2]
    add     r1, r0                                      @adds the timing to the two address values
    ldr     r2, =(TIMER_BASE + TIMER_ALARM0_OFFSET)     @laods offset for alarm control register
    str     r1, [r2]                                    @ stores added value into register
    
    ldr     r2, =(TIMER_BASE + TIMER_INTE_OFFSET)       @loads offset of interupt enable
    ldr     r1, =#0x1
    str     r1, [r2]                                    @storing the correct value into the interupt register
    pop     {pc}

     
@ Subroutine used to install the alrm interrupt service handler
install_alrm_isr:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)        @loads rp2040 component adress and vector adress into r2 
    ldr     r1, [r2]                                    
    movs    r2, #ALRM_ISR_OFFSET                        @movs value of iser offset into r2
    add     r1, r2                                      @adding the values 
    ldr     r0, =alrm_isr                               @loading the alrm_isr function to r0
    str     r0, [r1]                                    @stroing it in the added address
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)   @setting up the pending/disable register and putting into r2
    ldr     r0, =#0x1                                   @ 
    str     r0, [r2]                                    @setting the correct value for the pending interupt
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)   @setting the enable interupt into r2
    ldr     r0, =#0x1                                   
    str     r0, [r2]                                    @ storing the correct value into the enable offset registers
    bx      lr                                          @ exiting the routine

.thumb_func                                             @ Required for all interrupt service routines
alrm_isr:                                               
    push    {lr}                                        @pushes return address to stack
    ldr     r2, =(TIMER_BASE + TIMER_INTR_OFFSET)       @loads offset of timer raw interupt
    ldr     r1, =#0x1                                   @ <TODO – add comment>
    str     r1, [r2]                                    
    cmp     r7, #ONE_SEC
    beq     alrm_done   
    ldr     r5, =ZERO_SEC                               @lds 0 into r5 so next question doesnt print
    ldr     r1, =ONE_SEC                                
    adds    r4, r4, r1                                  @Increments r4 so that when it reaches 2 it will enter user answer
    cmp     r4, #TWO_SEC  
    beq     dashproc    
    bl      alrm_done
dashproc:                                               @ storing the correct value into the register to disable interupt
    bl      printtheoutput                              @prints the user output if twos seconds go past with no interupts
    ldr     r5, =#1                                     @movs 1 into r5 so can move to next question
alrm_done:
    bl      alarm
    pop     {pc}                                        @exiting the fucntion

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
    bl      watchdog_timer_reset 
    ldr     r1, =(GPIO_BTN_FALL_MSK)                                @ loads value of falling edge for btn and compares to interupt
    cmp     r0, r1
    beq     gpiofall                                                @if its equal enter the fall routine
    ldr     r2, =(GPIO_BTN_RISE_MSK)                                @loads value of rising edge for btn and compares to interupt
    cmp     r0, r2
    beq     gpiorise                                                @if its equal enter the rise routine                                             @if its equal enter the engage routine
    b       done                                                    @ else exit

gpiofall:
    ldr     r2, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET)            @disables pending interupt by writing falling edge value to raw offset     
    ldr     r1, =#GPIO_BTN_FALL_MSK                                   
    str     r1,[r2]  
    ldr     r2, =(TIMER_BASE + 0x28)                                @reads 1st 32 bits of TIME 
    ldr     r1, [r2]
    ldr     r7, =#ONE_SEC                                           @moves 1 into r7 so we know that button is being preseed
    subs    r1, r6
    ldr     r2, =(TIMER_BASE + 0x28)                                @reads 1st 32 bits of TIME and stores in r6 for rising interupt
    ldr     r6, [r2]
    ldr     r2, =#SPASE_TIME                                        @comapres with 1s and goes to empty if its smaller
    cmp     r1, r2
    blt     empty
    ldr     r2, =#ENTER_TIME                                        @comapres with 2.33s and goes to space if its smaller
    cmp     r1, r2
    blt     spase
    b       done
    
empty:
    ldr     r2, =#MIN_TIME
    cmp     r1, r2
    blt     done
    movs    r0, #43
    bl      addtoanswer
    ldr     r0, =msgsapce
    bl      printf   
    b       done 

spase:
    movs    r0, #32
    bl      addtoanswer
    ldr     r0, =msgplus
    bl      printf  
    b       done 

       
gpiorise:
    ldr     r2, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET)            @disables pending interupt by writing falling edge value to raw offset     
    ldr     r1, =#GPIO_BTN_RISE_MSK                                   
    str     r1, [r2]  
    ldr     r2, =(TIMER_BASE + 0x28) 
    ldr     r1, [r2]
    ldr     r7, =#ZERO_SEC
    subs    r1, r6 
    ldr     r2, =(TIMER_BASE + 0x28) 
    ldr     r6, [r2]
    ldr     r2, =#DOT_TIME
    cmp     r1, r2
    blt     dot   
    bge     dash  
    b       done      
dot: 
    movs    r0, #46
    bl      addtoanswer
    ldr     r0, =msgdot
    bl      printf   
    b       done     

dash: 
    movs    r0, #45
    bl      addtoanswer
    ldr     r0, =msgdash
    bl      printf   
    b       done   
 
                                    @ <TODO – add comment>
done:                                
    ldr     r4, =#ZERO_SEC           
    bl      alarm    @proc the alarm routine 
    pop     {pc}                                                    @exit the routine


.align 4
msgdot:    .asciz "."
msgdash:   .asciz "-"
msgsapce:   .asciz " "
msgplus:    .asciz "   "
msgagain:    .asciz "No Match, Try Again"



.data
lstate: .word   DFLT_STATE_STRT
ltimer: .word   DFLT_ALARM_TIME
