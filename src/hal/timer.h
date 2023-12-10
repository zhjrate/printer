#ifndef _TIMER_H
#define _TIMER_H

#include "global/config.h"
#include "global/device.h"
#include <Ticker.h>

void open_print_timeout_timer(void);
void close_print_timeout_timer(void);
bool get_print_timeout_error(void);
void read_state_timer_init(void);
bool get_read_state_timeout(void);
void clean_read_state_timeout(void);

#endif