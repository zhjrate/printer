#ifndef _MY_HAL_H
#define _MY_HAL_H

#include "adc.h"
#include "led.h"
#include "global/device.h"

bool get_paper_lack_flag(void);
void read_all_hal(void);
void hal_init(void);


#endif