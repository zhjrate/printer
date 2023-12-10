#ifndef _DEVICE_H
#define _DEVICE_H

#include <Arduino.h>

typedef enum
{
    PAPER_NORMAL = 0,
    PAPER_LACK,
} PAPER_STATE;

typedef enum
{
    PRINTER_INIT = 0,
    PRINTER_START,
    PRINTER_WORKING,
    PRINTER_FINISH,
} PRINTER_STATE;

typedef struct 
{
    uint8_t temperature;
    uint8_t battery;
    PAPER_STATE paper_state;
    bool connection;
    PRINTER_STATE printer_state;
    bool ble_write_finished;
}device_state_struct;

void device_state_init(void);
device_state_struct* get_device_state(void);
void set_ble_write_finished(bool finish);


#endif