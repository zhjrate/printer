#include "adc.h"

// 限幅函数--内部调用
static void limitation(uint8_t& value, uint8_t up_limit, uint8_t down_limit)
{
  if(up_limit < down_limit) {
    Serial.println("error occur in function 'limitation', up_limit must bigger than down_limit!");
    return;
  }
  if(value > up_limit)
    value = up_limit;
  if(value < down_limit)
    value = down_limit;
}

// 获取电池电量百分比函数
uint8_t get_battery_percentage(void)
{
    long sum = 0;
    // 连续读ADC的值进行平均值滤波
    for(unsigned i=0; i<ADC_FILTERING_COUNT; i++)
        sum += analogReadMilliVolts(BATTERY_ADC_PIN);
    uint32_t voltage = (uint32_t)(sum / ADC_FILTERING_COUNT);
    // 电压值转化为电量值，限幅在0~100之间
    uint8_t battery = map(voltage*2, BATTERY_LOW, BATTERY_FULL, 0, 100);
    limitation(battery, 100, 0);
    // Serial.printf("battery: %d \n", battery);
    return battery;
}

// 公式法计算温度值
static float calculate_temperature(float Rx)
{
  const float B = 3950;
  const float R25 = 30000;
  const float T25 = 25 + 273.15;
  const float offset = 0.5;
  float temp = 1 / (log(Rx/R25)/B + 1/T25) - 273.15 + offset;
  return temp;
}

// 获取打印头温度函数
float get_temperature(void)
{
  const float R1 = 10000;
  float Vi = (float)analogReadMilliVolts(TEMP_ADC_PIN) * (3.3/4095);  // ADC读取此时电压值
  float Rx = (Vi*R1) / (3.3-Vi);
  float temp = calculate_temperature(Rx);
  return temp;
}