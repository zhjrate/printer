#ifndef _MOTOR_H
#define _MOTOR_H

#include "global/config.h"

void motor_init(void);
void motor_start(void);
void motor_stop(void);
void motor_run_step(uint32_t steps);

#endif