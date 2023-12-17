#include "device.h"

device_state_struct device_state;   // 设备状态结构体

// 设备状态初始化函数
void device_state_init(void)
{
    memset(&device_state, 0, sizeof(device_state_struct));
    device_state.battery = 100;
    device_state.connection = false;
    device_state.paper_state = PAPER_NORMAL;
    device_state.temperature = 30;
    device_state.printer_state = PRINTER_INIT;
}

// 获取设备状态结构体的引用
device_state_struct& get_device_state(void)
{
    return device_state;
}