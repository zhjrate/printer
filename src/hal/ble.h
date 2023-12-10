#ifndef _BLE_H
#define _BLE_H

#include "global/config.h"
#include "global/device.h"
#include <BLEDevice.h>
#include <BLE2902.h>

// Server -> Service -> Charateristic -> Property

void ble_init(void);
void ble_report(void);
void set_ble_write_finished(bool finish);
bool get_ble_write_finished(void);

#endif