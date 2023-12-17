#include "my_hal.h"

bool paper_lack_need_report = false;    // 缺纸上报标志位

// 获取缺纸上报标志位，判断是否需要上报
bool get_paper_lack_flag(void)
{
    if(paper_lack_need_report == true)
    {
        paper_lack_need_report = false;
        return true;
    }
    return false;
}

// 缺纸中断--上升沿触发的外部中断
static void ARDUINO_ISR_ATTR paper_isr(void)
{
    // 标记为缺纸
    paper_lack_need_report = true;
    get_device_state().paper_state = PAPER_LACK;
    detachInterrupt(PAPER_PIN);     // 关闭中断
}

// 读取热敏纸状态
static void read_paper_state(void)
{
    // 当前不缺纸
    if(digitalRead(PAPER_PIN) == LOW)
    {
        get_device_state().paper_state = PAPER_NORMAL;  // 设置为正常状态
        attachInterrupt(PAPER_PIN, paper_isr, RISING);  // 开启外部中断，时刻提防打印机缺纸
    }
    else    // 缺纸了
    {
        get_device_state().paper_state = PAPER_LACK;    // 设置为缺纸状态
    }
}

// 读打印头温度函数
static void read_temperature_state(void)
{
    uint8_t temp = (uint8_t)get_temperature();
    get_device_state().temperature = temp;  // 更新设备状态
}

// 读电量函数
static void read_battery_state(void)
{
    get_device_state().battery = get_battery_percentage();
}

// 获取硬件设备状态函数--缺纸，电量，打印头温度
void read_all_hal(void)
{
    read_paper_state();
    read_temperature_state();
    read_battery_state();
}

// 硬件初始化函数
void hal_init(void)
{
    led_init();
    analogReadResolution(ADC_RESOLUTION_BIT);   // 设置ADC采集分辨率
    pinMode(PAPER_PIN, INPUT);
}