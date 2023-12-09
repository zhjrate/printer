#include "led.h"

void led_init(void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 1);
}

void set_led_state(LED_STATE led_state)
{
    switch(led_state)
    {
        case LED_CONNECT:
            digitalWrite(LED_PIN, 0);
            delay(100);
            digitalWrite(LED_PIN, 1);
            break;
        case LED_ERROR:
            digitalWrite(LED_PIN, 0);
            delay(100);
            digitalWrite(LED_PIN, 1);
            delay(100);
            digitalWrite(LED_PIN, 0);
            delay(100);
            digitalWrite(LED_PIN, 1);
            delay(100);
            digitalWrite(LED_PIN, 0);
            delay(100);
            digitalWrite(LED_PIN, 1);
            break;
        case LED_PRINTING_START:
            digitalWrite(LED_PIN, 0);
            delay(100);
            digitalWrite(LED_PIN, 1);
            break;
        default:
            break;
    }
}