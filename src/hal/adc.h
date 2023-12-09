#ifndef _ADC_H
#define _ADC_H

#include "global/config.h"

uint8_t get_battery_percentage(void);
float get_temperature(void);
static float calculate_temperature(float Rx);

#endif