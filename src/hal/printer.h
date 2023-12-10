#ifndef _PRINTER_H
#define _PRINTER_H

#include "global/config.h"
#include "global/device.h"
#include "spi.h"
#include "motor.h"
#include "timer.h"
#include "utils/queue.h"
#include "ble.h"

void set_heat_density(uint8_t density);
void printer_init(void);
void start_printing_by_queuebuf(void);
void testSTB(void);
#endif