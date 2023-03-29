#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

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

void morsematching(){
    

    OUPUT =
}

void printuser(){
    morsematching();
    printf("\33[2K\r")
    printf("%s | %s", OUPUT, INPUT)
}
void stringadd(msg){
    if(msg == "dot"){
        strcat(INPUT, ".");
        printuser();
    }
    else if(msg = "dash"){
        strcat(INPUT, "-");
        printuser();
    }
    else if(msg = "space"){
        strcat(INPUT, " ");
        printuser();
    }
    else if(msg == "enter"){
        strcpy(INPUT, "");
        printuser();
    }
}

void stringadd(msg){
    if(msg == "dot"){
        strcat(INPUT, ".");
    }
    else if(msg = "dash"){
        strcat(INPUT, "-");
    }
    else if(msg = "space"){
        strcat(INPUT, " ");
    }
    else if(msg == "enter"){
        strcpy(INPUT, "");
    }
}

bool entermorse(){
    return True;
}
void watchdog(){
    
}

void homescreen(){
    FILE *fptr;  
    fptr = fopen("ascii_art.txt", "r"); 
    printf("%c", fgetc(fptr));
    printf("Enter Morse To Start Level\n< . - - - - > | Level 01 | Chars | EASY\n< . . - - - > | Level 02 | Chars | HARD\n< . . . - - > | Level 03 | Words | EASY\n< . . . . - > | Level 04 | Words | HARD\n\n");  // Basic print to console
}

void homescreenclr(){
    clrscr();
}


// Main entry point of the application
int main() {
    stdio_init_all();
    char[20] INPUT = "";
    char[1] OUTPUT = "";
    return 0;                      // Application return code
}