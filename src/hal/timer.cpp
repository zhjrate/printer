#include "timer.h"
#include "my_hal.h"

Ticker print_timeout_timer;     // 打印超时计时器 20s
Ticker read_state_timer;        // 读设备状态计时器 10s

bool print_timeout_error = false;   // 打印超时异常标志位
bool read_state_timeout = false;    // 读设备状态定时标志位

// 打印超时回调
static void print_timeout_callback(void)
{
    print_timeout_error = true;
}

// 开启打印超时侦听
void open_print_timeout_timer(void)
{
    print_timeout_error = false;
    print_timeout_timer.attach_ms(20000, print_timeout_callback);
}

// 关闭打印超时侦听
void close_print_timeout_timer(void)
{
    print_timeout_error = false;
    if(print_timeout_timer.active() == true)
        print_timeout_timer.detach();
}

// 获取打印超时异常标志位
bool get_print_timeout_error(void)
{
    return print_timeout_error;
}

// 读设备状态回调
void read_state_callback(void)
{
    read_state_timeout = true;
}

// 读设备状态定时器初始化
void read_state_timer_init(void)
{
    read_state_timeout = false;
    read_state_timer.attach_ms(10000, read_state_callback); // 开启定时
}

// 获取读设备状态定时标志位
bool get_read_state_timeout(void)
{
    return read_state_timeout;
}

// 清除读设备状态定时标志位
void clean_read_state_timeout(void)
{
    read_state_timeout = false;
}