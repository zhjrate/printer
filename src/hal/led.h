#ifndef _LED_H
#define _LED_H

#include "global/config.h"

// LED状态
typedef enum
{
    LED_CONNECT = 0,
    LED_ERROR,
    LED_PRINTING_START,
    LED_DISCONNECT,
    LED_ON,
    LED_OFF,
} LED_STATE;

void led_init(void);
void set_led_state(LED_STATE led_state);

#endif