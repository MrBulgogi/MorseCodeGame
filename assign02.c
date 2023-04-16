//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defining global variables and including used libraries
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare the all the functions so they can be called whenever in the code
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_asm();
void Homescreen();
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
void clearAnswer();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global arrays for words and letters so we can get the morse in the code
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char alphabet[ARRAY_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                             'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
char words[][20] = {"HELLO", "WORLD", "MORSE", "CODE", "MICRO", "COMP", "PICO", "WATER", "BREAD", "BOARD", "CAR", "COPY", "PASTE"};
char answer[50];
char finalanswer[50];
int  i = 0;
bool rightorwrong;
int lives = 3;
int count = 0; 
char s[50];
char b[50];
int attempt_tracker =0;
int correct_tracker=0;
int deaths=0;
double accuracy;
int attempt_tracker2 =0;
int correct_tracker2=0;
double accuracy2;


/**
 * @brief main function which starts the program 
 * // Main entry point of the application
 */
int main() {
    stdio_init_all();
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    main_asm(); 

    return 0;                      // Application return code
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Watchdog function
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function to Enable the watchdog timer
 * enables the watchdog ttimer and prints either a
 * timeoutscreen when called from the timer
 * or print watchdog enabled if called when initing
 */
void watchdog_enabler(){
    watchdog_enable(8000, 0);
    if (watchdog_caused_reboot()) 
    {
        timeoutscreen();  //prints a timeout screen if the watchdog was activate
        printf("watchdog enableded\n"); 
    } 
    else 
    {         //print watchdog enabled for when we call it
        printf("watchdog enabled\n"); 
    }
}
//resets timer of watchdog,
void watchdog_timer_reset(){
    watchdog_update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GPIO FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB LIGHT FUCNTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

/**
 * @brief Function setLed changes the LED colour depending on user lives remaining
 */
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LEVELS FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function level1 prints a questions for level one and assigns a morse value to compare
 */
char *level1()
{
    clearAnswer();
    watchdog_timer_reset();
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    strncpy(s, translate(c), 50);
    printf("\nCharacter: %c\n", c);
    printf("Morse equivalent of %c: %s \n", c, s);
    return s;
}
/**
 * @brief Function level2 prints a questions for level two and assigns a morse value to compare
 */
char *level2()
{
    clearAnswer();
    watchdog_timer_reset();
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    strncpy(s, translate(c), 50);
    printf("\nCharacter: %c\n", c);
    return s;
}
/**
 * @brief Function level3 prints a questions for level three and assigns a morse value to compare
 */
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
/**
 * @brief Function level4 prints a questions for level four and assigns a morse value to compare
 */
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TRANSLATION FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief translate takes an input letter and returns the morse equivalent
 *
 * @param C     input letter that needs morse translation
 * @return  returns the tranlation of the letter
 */
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

/**
 * @brief Converts morse code into a letter.
 *
 * @param M     The inputted morse from the user 
 * @return returns a letter of the morse inputted.
 */
char morseToChar(char *M)
{
    int i = 0;
    // all possible morse outputs
    char *morse[] = {". -", "- . . .", "- . - .", "- . .", ".", ". . - .", "- - .", ". . . .", ". .", ". - - -",
                     "- . -", ". - . .", "- -", "- .", "- - -", ". - - .", "- - . -", ". - .", ". . .", "-",
                     ". . -", ". . . -", ". - -", "- . . -", "- . - -", "- - . .", "- - - - -", ". - - - -", ". . - - -",
                     ". . . - -", ". . . . -", ". . . . .", "- . . . .", "- - . . .", "- - - . .", "- - - - .", " "};

    for (i = 0; i <= 36; i++)
    {
        if (strcmp(M, morse[i]) == 0)
        {
            //printf("\n%s is the equivalent of %c", M, alphabet[i]);
            return alphabet[i];
            break;
        }
    }
    //printf("?");
    //return '?';
}
/**
 * @brief Converts morse code into a word
 *
 * @param M     The inputted morse from the user 
 * @return returns a word or letter correlating to the users morse input or ? if none can be matched
 */
void morseToString(char *MORSE)
{
    char *STRING = malloc(sizeof(char) * MAX_STRING);
    if (STRING == NULL)
    {
        printf("\nError allocating memory for morseToString\n");
        return;
    }
    char *morse = malloc(sizeof(char) * MAX_STRING);
    if (morse == NULL)
    {
        printf("\nError allocating memory for morseToString\n");
        return;
    }
    STRING[0] = '\0';
    morse[0] = '\0';
    char c = '\0';
    int i, j, k, z;
    i = 0;
    j = i;
    k = j + 1;
    z = k + 1;
    while (MORSE[i] != '\0')
    {
        if ((MORSE[j] == ' ') && (MORSE[k] == ' ') && (MORSE[z] == ' '))
        {
            //morse[strlen(morse) - 1] = '\0';
            c = morseToChar(morse);
            char temp2[2] = {c, '\0'};
            strncat(STRING, temp2, 2); 
            morse[0] = '\0';
            i = i + 2;
            j = j + 2;
            k = k + 2;
            z = z + 2;
        }
        else
        {
            char temp1[2] = {MORSE[i], '\0'};
            strncat(morse, temp1, 2);
    
        }
        i++;
        j++;
        k++;
        z++;
    }
//    morse[strlen(morse) - 1] = '\0';
    c = morseToChar(morse);
    char temp2[2] = {c, '\0'};
    strncat(STRING, temp2, 2); 
    printf("\nFinal Anser | MORSE: %s | STRING/CHAR:%s\n", MORSE, STRING);
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// USER ANSWER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief This functions clears the users answers
 */
void clearAnswer()
{
    answer[0] = '\0';
    finalanswer[0] = '\0';
    s[0] = '\0';
}
/**
 * @brief checks if the users answer matches the question and increments/decrements statistics
 * @return  returns 1 to move onto next level and 0 if not to
 */
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
            lives = 3;
            gameover();
        }
    }
    printf("You have %d lives left!\n", lives);
    printf("You have gotten %d in a row!\n", count);
    //printf("accuracy:%f",accuracy);
    strncpy(s, "", 50);
    if (count == 5)
    {
        lives = 3;
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief prints the users answer
 */
void printinput(){
    morseToString(finalanswer);
}

/**
 * @brief adds the gpio interupts for dash/dot/spaces to the answer array
 *
 * @param input   The dash/dot or sapce needed to be added to the answer
 */
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
/**
 * @brief copied the answer into final answer to be printed while deleting contents of answer
 */
void printtheoutput(){
    strncpy(finalanswer, answer, 50);
    printinput();
    strncpy(answer, "", 50);
    printf("%s", answer);
    i = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATISTICS FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief cprints the level and total statistics the the game has tracked
 */
void statistics_printout(){
    float accuracy = ((float)correct_tracker / (float)attempt_tracker) * 100;
    float accuracy2 = ((float)correct_tracker2 / (float)attempt_tracker2) * 100;
    printf(" ______________________ \n");
    printf("|\\ __________________ /| \n");
    printf("| | Level Statistics | |\n");
    printf("| | Attempts:%d       | |\n",attempt_tracker);
    printf("| | Correct:%d        | |\n",correct_tracker);
    printf("| | Accuracy:%.2f%%   | |\n", accuracy);
    printf("| |__________________| | \n");
    printf("| | Player Overall   | |\n");
    printf("| | Attempts:%d       | |\n",attempt_tracker2);
    printf("| | Deaths:%d         | |\n",deaths);
    printf("| | Correct:%d        | |\n",correct_tracker2);
    printf("| | Accuracy:%.2f%%   | |\n", accuracy2);
    printf("| |__________________| | \n");
    printf("|/____________________\\| \n");
}

/**
 * @brief resets statistics associted with one level
 */
void reset_levelstatistics(){
    attempt_tracker = 0;
    correct_tracker = 0;
}
/**
 * @brief resets all of the statistics
 */
void reset_totalstatistics(){
    deaths = 0;
    attempt_tracker = 0;
    correct_tracker = 0;
    attempt_tracker2 = 0;
    correct_tracker2 = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LEVEL0/STARTUP FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /**
 * @brief Prints the homescreen and morse input required for each level
 */
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
/**
 * @brief Function for morse matching and calling of levels. 
 *        supplies morse for each level to match and compares with finalanswer
 *        before printing each levels header
 * 
 * @return return values 1-4 for each level if called
 */
int level0(){
    clearAnswer();
    watchdog_timer_reset();
    char level1[50] = ". - - - -";
    char level2[50] = ". . - - -";
    char level3[50] = ". . . - -";
    char level4[50] = ". . . . -";
    if (strcmp(finalanswer, level1) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 1                                                                                 |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
        printf("| You are given both a character and the morse equivalent below                           |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 1;
    }
    else if (strcmp(finalanswer, level2) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 2                                                                                 |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 2;
    }
    else if (strcmp(finalanswer, level3) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 3                                                                                 |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
        printf("| You are given both a word and the morse equivalent below                                |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 3;
    }
    else if (strcmp(finalanswer, level4) == 0){
        printf("\n -----------------------------------------------------------------------------------------\n");
        printf("| LEVEL 4                                                                                 |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
        printf(" -----------------------------------------------------------------------------------------\n");
        return 4;
    }
    else {
        return 0;
    }
}

/**
 * @brief Prints the header for level 2 so user knows what level they are in
 */
void Level2header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 2                                                                                 |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given character |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}

/**
 * @brief Prints the header for level 3 so user knows what level they are in
 */
void Level3header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 3                                                                                 |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
    printf("| You are given both a word and the morse equivalent below                                |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}

/**
 * @brief Prints the header for level 4 so user knows what level they are in
 */
void Level4header(){
    printf("\n -----------------------------------------------------------------------------------------\n");
    printf("| LEVEL 4                                                                                 |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
    printf("| In this level your task is to enter the morse code corresponding to the given word      |\n");
    printf(" -----------------------------------------------------------------------------------------\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY PRINTOUT FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Timed out screen to show if watchdog was called
 */
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
    clearAnswer();
}
/**
 * @brief Game completed screen printout so user knows they have finished the game
 */
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

/**
 * @brief Prints the Gameover screen and calls the statistic printout function 
 *        before returning to main
 */
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
    sleep_ms(8000);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMING FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Sleep functions so sleep can be used in arm one long and one short
 *        watchdog is called so the game doesn't reset if sleep is used
 */
void sleep(){
    watchdog_timer_reset();
    sleep_ms(2000);
    watchdog_timer_reset();
}

void sleeplong(){
    watchdog_timer_reset();
    sleep_ms(5000);
    watchdog_timer_reset();
}
