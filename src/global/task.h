#ifndef _TASK_H
#define _TASK_H

#include "global/config.h"
#include "global/device.h"
#include "hal/printer.h"
#include "hal/my_hal.h"
#include "hal/timer.h"
#include "hal/button.h"
#include "utils/queue.h"

void initialization(void);
void task_print(void);

#endif