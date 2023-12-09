#include "adc.h"

uint8_t get_battery_percentage(void)
{
    long sum = 0;
    for(unsigned i=0; i<ADC_FILTERING_COUNT; i++)
        sum += analogReadMilliVolts(BATTERY_ADC_PIN);
    uint32_t voltage = (uint32_t)(sum / ADC_FILTERING_COUNT);
    uint8_t battery = map(voltage*2, BATTERY_LOW, BATTERY_FULL, 0, 100);
    Serial.printf("battery: %d \n", battery);
    return battery;
}

static float calculate_temperature(float Rx)
{
  const float B = 3950;
  const float R25 = 30000;
  const float T25 = 25 + 273.15;
  const float offset = 0.5;
  float temp = 1 / (log(Rx/R25)/B + 1/T25) - 273.15 + offset;
  return temp;
}

float get_temperature(void)
{
  const float R1 = 10000;
  float Vi = (float)analogReadMilliVolts(TEMP_ADC_PIN) * (3.3/4095);
  float Rx = (Vi*R1) / (3.3-Vi);
  float temp = calculate_temperature(Rx);
  return temp;
}