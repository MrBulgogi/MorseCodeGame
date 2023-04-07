#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#define NUM_PIXELS 1  // There is 1 WS2812 device in the chain
#define WS2812_PIN 28 // The GPIO pin that the WS2812 connected
#define ARRAY_SIZE 38
#define MAX_STRING 30


// Declare the main assembly code entry point.
void main_asm();
void outputChar();
void checkEntry(char C, char *string, int lives);
char *level1();
char *level2();
char *level3();
char *level4();
void setLED(int lives);
void addtoanswer(char input);



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
char finalanswer[];
int  i = 0;
bool rightorwrong;
int lives;
int count; // count will be incremented/decremented if a player gets the correct answer

char *translate(char c);

void checkEntry(char C, char *string, int lives)
{
    char *morse = translate(C);
    if (C == 'A')
    {
        if (strcmp(morse, ".-") == 0)
        {
            printf("CORRECT!\n");
            if (lives < 3)
            {
                lives++;
            }
        }
        else
        {
            printf("INCOORECT\n");
            if (lives > 0)
            {
                lives--;
            }
        }
    }
    printf("You have %d lives left!\n", lives);
}


void printinput(){
    printf("\n%s", answer);
}


void addtoanswer(int input){
    if (input == 46){
        printinput(); 
        answer[i] = '.';
        i++;
    }
    else if (input == 45){
        printinput();
        answer[i] = '-';
        i++;
    }
    else if (input == 43){
        printinput();
        answer[i] = ' ';
        i++;
    }
    else if (input == 32){
        printinput();
        answer[i] = ' ';
        answer[i+1] = ' ';
        answer[i+2] = ' ';
        i += 3;
    }
}

void userenter(char answer[]){
    finalanswer[] = morsecoverter(answer);
}


void watchdog(){
}

void homescreen(){
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
    printf("Enter Morse To Start Level\n<. - - - -> | Level 01 | Chars | EASY\n<. . - - -> | Level 02 | Chars | HARD\n<. . . - -> | Level 03 | Words | EASY\n<. . . . -> | Level 04 | Words | HARD\n\n");  // Basic print to console
    
}

void level0(){
    char level1[] = "1";
    char level2[] = "2";
    char level3[] = "3";
    char level4[] = "4";
    if (strcmp(answer[], level1[])=0){
        printf("begin level 1");
    }

    else if (answer == level2){
        printf("begin level 2");
    }

    else if (answer == level3){
        printf("begin level 3");
    }

    else if (answer == level4){
        printf("begin level 4");
    }
    else {
        timeoutscreen();
    }
}

void timeoutscreen(){
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



/*void chooselevel(){
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
}*/

// Main entry point of the application
int main() {
    stdio_init_all();
    sleep_ms(5000);
    main_asm(); 
    return 0;                      // Application return code
}