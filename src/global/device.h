#ifndef _DEVICE_H
#define _DEVICE_H

#include <Arduino.h>

// 热敏纸状态--缺纸 or 正常
typedef enum
{
    PAPER_NORMAL = 0,
    PAPER_LACK,
} PAPER_STATE;

// 打印机状态
typedef enum
{
    PRINTER_INIT = 0,   // 初始化完成
    PRINTER_START,      // 开始打印
    PRINTER_WORKING,    // 正在打印
    PRINTER_FINISH,     // 打印完成
} PRINTER_STATE;

// 设备状态
typedef struct 
{
    uint8_t temperature;        // 打印头温度
    uint8_t battery;            // 电量
    PAPER_STATE paper_state;    // 热敏纸状态
    bool connection;            // 蓝牙连接状态
    PRINTER_STATE printer_state;// 打印机状态
}device_state_struct;

void device_state_init(void);
device_state_struct& get_device_state(void);


#endif