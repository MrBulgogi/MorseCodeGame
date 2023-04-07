/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

//bool temp;
int runs=0;
int main() {
    stdio_init_all();
    // Enable the watchdog, requiring the watchdog to be updated every 100ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    
    watchdog_enable(8000, 1);
    for (uint i = 0; i < 1600; i++) {
        printf("Updating watchdog %d\n", i);
        //printf("temp val=%d\n",temp);
        watchdog_update();
    }
    printf("runs:%d\n",runs);
    if (watchdog_caused_reboot()) 
    {
        printf("updating\n");
        runs=runs+1;
        printf("runs:%d\n",runs);
        watchdog_enable(1000,1);
        //printf("%d\n",runs);
    } 
    else 
    {
        printf("Clean boot\n");
    }
    // Wait in an infinite loop and don't update the watchdog so it reboots us
    printf("Waiting to be rebooted by watchdog\n");
    while(1);
}