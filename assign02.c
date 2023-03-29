#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

int startbtn;
int endbtn;
int timebtn;
int startgap;
int endgap;
int timegap;

// Declare the main assembly code entry point.
void main_asm();

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin) {
    gpio_init(pin);
}

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out) {
    gpio_set_dir(pin, out);
}

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin) {
    return gpio_get(pin);
}

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value) {
    gpio_put(pin, value);
}

// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin) {
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
}

void asm_gpio_set_irq2(uint pin) {
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
}

void printdash(){
    printf("-");
}

void printdot(){
    printf(".");
}

void printspace(){
    if (i < 2){
        printf(" ");
    }
}
void watchdog(){
    
}


// Main entry point of the application
int main() {
    FILE *fptr;  
    fptr = fopen("ascii_art.txt", "r"); 
    printf("%c", fgetc(fptr));
    printf("Enter Morse To Start Level\n< . - - - - > | Level 01 | Chars | EASY\n< . . - - - > | Level 02 | Chars | HARD\n< . . . - - > | Level 03 | Words | EASY\n< . . . . - > | Level 04 | Words | HARD\n\n");  // Basic print to console
    stdio_init_all();  
    //clrscr();
    /*while (true){
        if (timebtn <= 1000000){
            printf(".");
        }
        else if (timebtn <= 3000000 && timebtn >= 1000000){
            printf("-");
        }

        if (timegap >= 2500000 && timegap <= 3500000){
            printf(" ");
        }
        else if (timegap )
    }

    }*/
    return 0;                      // Application return code
}