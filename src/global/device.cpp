#include "device.h"

device_state_struct* device_state;
PRINTER_STATE printer_state;

void device_state_init(void)
{
    memset(device_state, 0, sizeof(device_state_struct));
    device_state->battery = 100;
    device_state->connection = false;
    device_state->paper_state = PAPER_NORMAL;
    device_state->temperature = 30;
    device_state->printer_state = PRINTER_INIT;
    device_state->ble_write_finished = false;
}

device_state_struct* get_device_state(void)
{
    return device_state;
}

PRINTER_STATE get_printer_state(void)
{
    return printer_state;
}

void set_ble_write_finished(bool finish)
{
    device_state->ble_write_finished = finish;
}