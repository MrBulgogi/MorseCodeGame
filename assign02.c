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
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
}

char answer[50];
int  i = 0;
bool rightorwrong;

void addtoanswer(int input){
    if (input == 46){
        answer[i] = '.';
        i++;
        printinput();
    }
    else if (input == 45){
        answer[i] = '-';
        i++;
        printinput();
    }
    else if (input == 32){
        answer[i] = ' ';
        i++;
        printinput();
    }
    else if (input == 64){
        answer[i] = 'e';
        printinput();
        stringcomparison(answer, question);
    }
    else {
        answer[i] = '?';
        printinput();
    }
}

void watchdog(){
}

void homescreen(){
    system("clear");
    printf(" ______________________ \n");
    printf("|\\ __________________ /| \n");
    printf("| |  ╔╦╗╔═╗╦═╗╔═╗╔═╗ | | \n");
    printf("| |  ║║║║ ║╠╦╝╚═╗║╣  | | \n");
    printf("| |  ╩ ╩╚═╝╩╚═╚═╝╚═╝ | | \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗    | | \n");
    printf("| |  ║  ║ ║ ║║║╣     | | \n");
    printf("| |  ╚═╝╚═╝═╩╝╚═╝    | | \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗    | | \n");
    printf("| |  ║ ╦╠═╣║║║║╣     | | \n");
    printf("| |  ╚═╝╩ ╩╩ ╩╚═╝    | | \n");
    printf("| |__________________| | \n");
    printf("|/____________________\\| \n");
    printf("Enter Morse To Start Level\n<.----> | Level 01 | Chars | EASY\n<..---> | Level 02 | Chars | HARD\n<...--> | Level 03 | Words | EASY\n<....-> | Level 04 | Words | HARD\n\n");  // Basic print to console
}

void timeoutscreen(){
    system("clear");
    printf(" ______________________ \n");
    printf("|\\ __________________ /| \n");
    printf("| |  ╔╦╗╦╔╦╗╔═╗╔╦╗   | | \n");
    printf("| |   ║ ║║║║║╣  ║║   | | \n");
    printf("| |   ╩ ╩╩ ╩╚═╝═╩╝   | | \n");
    printf("| |  ╔═╗╦ ╦╔╦╗   //  | | \n");
    printf("| |  ║ ║║ ║ ║   //   | | \n");
    printf("| |  ╚═╝╚═╝ ╩  //    | | \n");
    printf("| |  ╦ ╦╦═╗╔═╗╔╗╔╔═╗ | | \n");
    printf("| |  ║║║╠╦╝║ ║║║║║ ╦ | | \n");
    printf("| |  ╚╩╝╩╚═╚═╝╝╚╝╚═╝ | | \n");
    printf("| |__________________| | \n");
    printf("|/____________________\\| \n");
}

void printinput(){
    system("clear");
    printf("%s, %s", answer, converted);
}


void chooselevel(){
    if (answer == ".----"){
        level1();
    }
    else if (answer == "..---"){
        level2();
    }
    else if (answer == "...--"){
        level3();
    }
    else if (answer == "....-"){
        level4();
    }
    else {
        timeoutscreen();
        sleep_ms(5000);
        mainscreen();
    }
}



void stringcomparison(char input[], char question[]){
    if (input == question){
        rightinput();
    }
    else {
        wronginput();
    }
}

// Main entry point of the application
int main() {
    stdio_init_all();
    sleep_ms(5000);
    main_asm(); 
    return 0;                      // Application return code
}