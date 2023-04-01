#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "assign02.pio.h"

#define IS_RGBW true  // Will use RGBW format
#define NUM_PIXELS 1  // There is 1 WS2812 device in the chain
#define WS2812_PIN 28 // The GPIO pin that the WS2812 connected
#define ARRAY_SIZE 38
#define MAX_STRING 30

int lives;
int count; // count will be incremented/decremented if a player gets the correct answer

char *translate(char c);
void checkEntry(char C, char *string, int lives);
char *level1();
char *level2();
char *level3();
char *level4();
void setLED(int lives);
/**
 * @brief Wrapper function used to call the underlying PIO
 *        function that pushes the 32-bit RGB colour value
 *        out to the LED serially using the PIO0 block. The
 *        function does not return until all of the data has
 *        been written out.
 *
 * @param pixel_grb The 32-bit colour value generated by urgb_u32()
 */
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

// Must declare the main assembly entry point before use.
void main_asm();

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin)
{
    gpio_init(pin);
}

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out)
{
    gpio_set_dir(pin, out);
}

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin)
{
    return gpio_get(pin);
}

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value)
{
    gpio_put(pin, value);
}

// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin)
{
    uint64_t t0, t1;
    double total;
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
    t0 = time_us_64();
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, false);
    t1 = time_us_64();
    total = t1 - t0;
    printf("TIME: %d\n", total);
}

char alphabet[ARRAY_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                             'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};

char words[][20] = {"HELLO", "WORLD", "MORSE", "CODE", "MICRO", "COMP", "PICO", "WATER", "BREAD", "BOARD", "CAR", "COPY", "PASTE"};

char *level1()
{
    printf("\nLEVEL 1\n-------\nIn this level your task is to enter the morse code corresponding to the given character\n");
    printf("You are given both a character and the morse equivalent below \n");
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    char *s = translate(c);
    printf("\nCharacter: %c\n", c);
    printf("Morse equivalent of %c: %s \n", c, s);
    return s;
}

char *level2()
{
    printf("\nLEVEL 2\n-------\nIn this level your task is to enter the morse code corresponding to the given character\n");
    int random = rand() % (ARRAY_SIZE - 1);
    char c = alphabet[random];
    char *s = translate(c);
    printf("\nCharacter: %c\n", c);
    return s;
}

char *level3()
{
    printf("\nLEVEL 3\n-------\nIn this level your task is to enter the morse code corresponding to the given word\n");
    printf("You are given both a word and the morse equivalent below \n");
    int random = rand() % (12);
    char *s = words[random];
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
        strncat(trans, morse, 4);
        morse = translate(space);
        strncat(trans, morse, 1);
    }
    char *newTrans = malloc(sizeof(char) * (MAX_STRING));
    if (newTrans == NULL)
    {
        printf("\nError allocating memory for 'newTrans'\n");
    }
    newTrans[0] = '\0';
    strncpy(newTrans, trans, strlen(trans) - 1);
    newTrans[strlen(trans) - 1] = '\0';

    printf("\nWord: %s\n", s);
    printf("Morse equivalent of %s: %s \n", s, newTrans);
    return newTrans;
}

char *level4()
{
    printf("\nLEVEL 4\n-------\nIn this level your task is to enter the morse code corresponding to the given word\n");
    int random = rand() % (12);
    char *s = words[random];
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
        strncat(trans, morse, 4);
        morse = translate(space);
        strncat(trans, morse, 1);
    }
    char *newTrans = malloc(sizeof(char) * (MAX_STRING));
    if (newTrans == NULL)
    {
        printf("\nError allocating memory for 'newTrans'\n");
    }
    newTrans[0] = '\0';
    strncpy(newTrans, trans, strlen(trans) - 1);
    newTrans[strlen(trans) - 1] = '\0';

    printf("\nWord: %s\n", s);
    return newTrans;
}

void setLED(int lives)
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
    char *morse[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
                     "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
                     "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---",
                     "...--", "....-", ".....", "-....", "--...", "---..", "----.", " "};

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

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
int main()
{
    stdio_init_all();
    // main_asm();
    //  Initialise the PIO interface with the WS2812 code
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    printf("\n-------------------------\n");
    printf("\n-------------------------\n");
    printf("\n-------------------------\n");
    level1();
    level2();
    level3();
    level4();
    printf("\n-------------------------\n");
    printf("\n-------------------------\n");
    printf("\n-------------------------\n");

    while (true)
    {
        setLED(0);
        sleep_ms(500);
        setLED(1);
        sleep_ms(500);
        setLED(2);
        sleep_ms(500);
        setLED(3);
        sleep_ms(500);
    }
    return 0;
}
