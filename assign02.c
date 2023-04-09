#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "assign02.pio.h"

#define IS_RGBW true  // Will use RGBW format
#define NUM_PIXELS 1  // There is 1 WS2812 device in the chain
#define WS2812_PIN 28 // The GPIO pin that the WS2812 connected
#define ARRAY_SIZE 38
#define MAX_STRING 30



// Declare the all the functions so they can be called whenever in the code
void main_asm();
void outputChar();
char *translate(char c);
int checkEntry();
char *level1();
char *level2();
char *level3();
char *level4();
void setLED();
void addtoanswer(int input);
void homescreen();
void printinput();
void timeoutscreen();
int level0();
void printtheoutput();
void entercommand();
void watchdog_enabler();
void watchdog_timer_reset();
void gameover();
void Level2header();
void Level3header();
void Level4header();
void statistics_printout();
void reset_levelstatistics();
void reset_totalstatistics();
void sleep();
void sleeplong();

//gloabal arrays for words and letters so we can get the morse in the code
char alphabet[ARRAY_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                             'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};

char words[][20] = {"HELLO", "WORLD", "MORSE", "CODE", "MICRO", "COMP", "PICO", "WATER", "BREAD", "BOARD", "CAR", "COPY", "PASTE"};
//global variables used in the code
char answer[50];
char finalanswer[50];
int  i = 0;
bool rightorwrong;
int lives = 3;
int count = 0; // count will be incremented/decremented if a player gets the correct answer
char s[50];
char b[50];
int attempt_tracker =0;
int correct_tracker=0;
int deaths=0;
double accuracy;
int attempt_tracker2 =0;
int correct_tracker2=0;
double accuracy2;



// Main entry point of the application
int main() {
    stdio_init_all();
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    watchdog_enabler();
    main_asm(); 

    return 0;                      // Application return code
}


// This function allows us to enable the watchdog timer
void watchdog_enabler(){
    watchdog_enable(8000, 0);
    if (watchdog_caused_reboot()) 
    {
        timeoutscreen();  //prints a timeout screen if the watchdog was activate
        printf("game starting in 3 seconds");
        sleep_ms(3000);   //sleeps for 3 seconds so the programme doesnt start immediately after
    } 
    else 
    {   printf("watchdog enabled\n");       //print watchdog enabled for when we call it
        sleep_ms(3000);
        //printf("Clean boot\n");
    }
}
//resets timer of watchdog,
void watchdog_timer_reset(){
    watchdog_update();
    //printf("timer reset");
}


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


static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

/**
 * @brief Function to generate an unsigned 32-bit composit GRB
 *        value by combining the individual 8-bit paramaters for
 *        red, green and blue together in the right order.
 *
 * @param r     The 8-bit intensity value for the red component
 * @param g     The 8-bit intensity value for the green component
 * @param b     The 8-bit intensity value for the blue component
 * @return uint32_t Returns the resulting composit 32-bit RGB value
 */
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void clearAnswer()
{
    answer[0] = '\0';
    finalanswer[0] = '\0';
    s[0] = '\0';
}

char *level1()
{
    watchdog_timer_reset();
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    strncpy(s, translate(c), 50);
    printf("\nCharacter: %c\n", c);
    printf("Morse equivalent of %c: %s \n", c, s);
    return s;
}

char *level2()
{
    watchdog_timer_reset();
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    strncpy(s, translate(c), 50);
    printf("\nCharacter: %c\n", c);
    return s;
}

char *level3()
{
    clearAnswer();
    watchdog_timer_reset();
    int random = rand() % (12);
    strncpy(s, words[random], 50);
    char *morse;
    char *trans = malloc(sizeof(char) * (MAX_STRING));
    if (trans == NULL)
    {
        printf("\nError allocating memory for 'trans'\n");
    }
    trans[0] = '\0';
    for (int i = 0; i < strlen(s); i++)
    {
        char c;
        c = s[i];
        morse = translate(c);
        char space = ' ';
        strncat(trans, morse, 8);
        morse = translate(space);
        strncat(trans, morse, 1);
        strncat(trans, morse, 1);
        strncat(trans, morse, 1);
    }
    char *newTrans = malloc(sizeof(char) * (MAX_STRING));
    if (newTrans == NULL)
    {
        printf("\nError allocating memory for 'newTrans'\n");
    }
    newTrans[0] = '\0';
    strncpy(newTrans, trans, strlen(trans) - 3);
    newTrans[strlen(trans) - 3] = '\0';

    printf("\nWord: %s\n", s);
    printf("Morse equivalent of %s: %s\n", s, newTrans);
    // printf("END");
    strcpy(s, newTrans);
    s[strlen(newTrans)] = '\0';
    return newTrans;
}

char *level4()
{
    clearAnswer();
    watchdog_timer_reset();
    int random = rand() % (12);
    strncpy(s, words[random], 50);
    char *morse;
    char *trans = malloc(sizeof(char) * (MAX_STRING));
    if (trans == NULL)
    {
        printf("\nError allocating memory for 'trans'\n");
    }
    trans[0] = '\0';
    for (int i = 0; i < strlen(s); i++)
    {
        char c;
        c = s[i];
        morse = translate(c);
        char space = ' ';
        strncat(trans, morse, 8);
        morse = translate(space);
        strncat(trans, morse, 1);
        strncat(trans, morse, 1);
        strncat(trans, morse, 1);
    }
    char *newTrans = malloc(sizeof(char) * (MAX_STRING));
    if (newTrans == NULL)
    {
        printf("\nError allocating memory for 'newTrans'\n");
    }
    newTrans[0] = '\0';
    strncpy(newTrans, trans, strlen(trans) - 3);
    newTrans[strlen(trans) - 3] = '\0';

    printf("\nWord: %s\n", s);
    strcpy(s, newTrans);
    s[strlen(newTrans)] = '\0';
    return newTrans;
}

void setLED()
{
    // if 3 lives set LED green
    if (lives == 3)
    {
        put_pixel(urgb_u32(0x00, 0x7F, 0x00));
    }
    // if 2 lives set LED blue
    if (lives == 2)
    {
        put_pixel(urgb_u32(0x00, 0x00, 0x7F));
    }
    // if 1 life set LED orange
    if (lives == 1)
    {
        put_pixel(urgb_u32(0x96, 0x1E, 0x00));
    }

    // if no lives left set LED red
    if (lives == 0)
    {
        put_pixel(urgb_u32(0xFF, 0x00, 0x00));
    }
}

char *translate(char C)
{
    int i = 0;
    // all possible morse outputs
    char *morse[] = {". -", "- . . .", "- . - .", "- . .", ".", ". . - .", "- - .", ". . . .", ". .", ". - - -",
                     "- . -", ". - . .", "- -", "- .", "- - -", ". - - .", "- - . -", ". - .", ". . .", "-",
                     ". . -", ". . . -", ". - -", "- . . -", "- . - -", "- - . .", "- - - - -", ". - - - -", ". . - - -",
                     ". . . - -", ". . . . -", ". . . . .", "- . . . .", "- - . . .", "- - - . .", "- - - - .", " "};

    // A to Z
    int firstChar = (int)'A'; // 65
    int lastChar = (int)'Z';  // 90
    int space = (int)' ';
    // first to last numbers
    int firstNum = (int)'0'; // 48
    int lastNum = (int)'9';  // 57

    if (C == space)
    {
        // if c is a space set index to 36
        i = 36;
    }
    // if ascii value of C is between ascii val of A & Z set index to
    // ascii of C - ascii of A
    else if (C <= lastChar && C >= firstChar)
    {
        i = (int)C - firstChar;
    }
    // if ascii value of C is between ascii val of 0 & 9 set index to
    // ascii of C - ascii of 0 and add 26 to skip the alphabet chars
    else if (C <= lastNum && C >= firstNum)
    {
        i = (int)C - firstNum + 26;
    }

    char *output = (char *)malloc(sizeof(char) * (strlen(morse[i]) + 1));
    if (output)
    {
        strcpy(output, morse[i]);
        return output;
    }
    else
    {
        printf("Malloc failedn\n");
        return NULL;
    }
}


int checkEntry()
{
    attempt_tracker++;
    attempt_tracker2++;
    if (strcmp(s, finalanswer) == 0)
    {   
        correct_tracker++;
        correct_tracker2++;
        printf("CORRECT!\n");
        if (lives < 3)
        {
            lives++;
            setLED();
        }
        count++;
        if (count == 6)
        {
            count = 1;
        }
    }
    else
    {
        printf("INCORRECT\n");
        if (lives > 0)
        {
            lives--;
            setLED();
            count = 0;
        }
        else
        {
            deaths++;
            gameover();
        }
    }
    printf("You have %d lives left!\n", lives);
    printf("You have gotten %d in a row!\n", count);
    //printf("accuracy:%f",accuracy);
    strncpy(s, "", 50);
    if (count == 5)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void statistics_printout(){
    float accuracy = ((float)correct_tracker / (float)attempt_tracker) * 100;
    float accuracy2 = ((float)correct_tracker2 / (float)attempt_tracker2) * 100;
    printf(" ______________________ \n");
    printf("|\\ __________________ /| \n");
    printf("| | Level Statistics | |\n");
    printf("| | Attempts:%d      | |\n",attempt_tracker);
    printf("| | Correct:%d       | |\n",correct_tracker);
    printf("| |Accuracy:%.2f%%   | |\n", accuracy);
    printf("| |__________________| | \n");
    printf("| | Player Overall   | |\n");
    printf("| | Attempts:%d      | |\n",attempt_tracker2);
    printf("| | Deaths:%d        | |\n",deaths);
    printf("| | Correct:%d       | |\n",correct_tracker2);
    printf("| | Accuracy:%.2f%%  | |\n", accuracy2);
    printf("| |__________________| | \n");
    printf("|/____________________\\| \n");
}

void reset_levelstatistics(){
    attempt_tracker = 0;
    correct_tracker = 0;
}

void reset_totalstatistics(){
    deaths = 0;
    attempt_tracker = 0;
    correct_tracker = 0;
    attempt_tracker2 = 0;
    correct_tracker2 = 0;
}




void printinput(){
    printf("\nFINAL ANSWER:%s\n", finalanswer);
}

void Level2header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 2                                                                                 |");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}

void Level3header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 3                                                                                 |");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
    printf("| You are given both a word and the morse equivalent below                                |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}
void Level4header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 4                                                                                 |");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}


void addtoanswer(int input){
    if (input == 46){
        answer[i] = '.';
        i++;
    }
    else if (input == 45){
        answer[i] = '-';
        i++;
    }
    else if (input == 43){
        answer[i] = ' ';
        i++;
    }
    else if (input == 32){
        answer[i] = ' ';
        answer[i+1] = ' ';
        answer[i+2] = ' ';
        i += 3;
    }
}

void printtheoutput(){
    strncpy(finalanswer, answer, 50);
    printinput();
    strncpy(answer, "", 50);
    printf("%s", answer);
    i = 0;
}
 
void homescreen(){
    printf("\n");
    printf(" ___________________________________________________________________ \n");
    printf("|\\ ________________________________________________________________/| \n");
    printf("| |  ╔╦╗╔═╗╦═╗╔═╗╔═╗  |   Enter Morse To Start Level              | | \n");
    printf("| |  ║║║║ ║╠╦╝╚═╗║╣   |   -------------------------------------   | | \n");
    printf("| |  ╩ ╩╚═╝╩╚═╚═╝╚═╝  |   <. - - - -> | Level 01 | Chars | EASY   | | \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗     |   -------------------------------------   | | \n");
    printf("| |  ║  ║ ║ ║║║╣      |   <. . - - -> | Level 02 | Chars | HARD   | | \n");
    printf("| |  ╚═╝╚═╝═╩╝╚═╝     |   -------------------------------------   | | \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗     |   <. . . - -> | Level 03 | Words | EASY   | | \n");
    printf("| |  ║ ╦╠═╣║║║║╣      |   -------------------------------------   | | \n");
    printf("| |  ╚═╝╩ ╩╩ ╩╚═╝     |   <. . . . -> | Level 04 | Words | HARD   | | \n");
    printf("| |_______________________________________________________________| | \n");
    printf("|/_________________________________________________________________\\| \n\n");
    
}

void gameover(){
    statistics_printout();
    sleeplong();
    printf("\n");
    printf(" ______________________ \n");
    printf("|\\ __________________ /| \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗    | | \n");
    printf("| |  ║ ╦╠═╣║║║║╣     | | \n");
    printf("| |  ╚═╝╩ ╩╩ ╩╚═╝    | | \n");
    printf("| |  ╔═╗╦  ╦╔═╗╦═╗   | | \n");
    printf("| |  ║ ║╚╗╔╝║╣ ╠╦╝   | | \n");
    printf("| |  ╚═╝ ╚╝ ╚═╝╩╚═   | | \n");
    printf("| |  ╦  ╔═╗╔═╗╔═╗╦═╗ | | \n");
    printf("| |  ║  ║ ║╚═╗║╣ ╠╦╝ | | \n");
    printf("| |  ╩═╝╚═╝╚═╝╚═╝╩╚═ | | \n");
    printf("| |__________________| | \n");
    printf("|/____________________\\| \n");
    sleep();
    main();
}

int level0(){
    watchdog_timer_reset();
    char level1[50] = ". - - - -";
    char level2[50] = ". . - - -";
    char level3[50] = ". . . - -";
    char level4[50] = ". . . . -";
    if (strcmp(finalanswer, level1) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 1                                                                                 |");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
        printf("| You are given both a character and the morse equivalent below                           |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 1;
    }
    else if (strcmp(finalanswer, level2) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 2                                                                                 |");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 2;
    }
    else if (strcmp(finalanswer, level3) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 3                                                                                 |");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
        printf("| You are given both a word and the morse equivalent below                                |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 3;
    }
    else if (strcmp(finalanswer, level4) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 4                                                                                 |");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 4;
    }
    else {
        return 0;
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

void gamecompleted(){
    printf(" __________________________________\n");
    printf("|\\ _______________________________/| \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗                | | \n");
    printf("| |  ║ ╦╠═╣║║║║╣                 | | \n");
    printf("| |  ╚═╝╩ ╩╩ ╩╚═╝                | | \n");
    printf("| |  ╦╔═╗                        | | \n");
    printf("| |  ║╚═╗                        | | \n"); 
    printf("| |  ╩╚═╝                        | | \n");
    printf("| |  ╔═╗╔═╗╔╦╗╔═╗╦  ╔═╗╔╦╗╔═╗╔╦╗ | | \n");
    printf("| |  ║  ║ ║║║║╠═╝║  ║╣  ║ ║╣  ║║ | | \n");
    printf("| |  ╚═╝╚═╝╩ ╩╩  ╩═╝╚═╝ ╩ ╚═╝═╩╝ | | \n");
    printf("| |______________________________| | \n");
    printf("|/________________________________\\| \n");
}

void sleep(){
    sleep_ms(2000);
}

void sleeplong(){
    sleep_ms(5000);
}
