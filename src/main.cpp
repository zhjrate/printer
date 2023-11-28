#include <Arduino.h>

#define LED_PIN 18
#define KEY_PIN 5
#define BATTERY_ADC_PIN 34

#define ADC_FILTERING_COUNT 10

uint8_t get_battery_percentage(void)
{
  long sum = 0;
  for(unsigned i=0; i<ADC_FILTERING_COUNT; i++)
    sum += analogReadMilliVolts(BATTERY_ADC_PIN);
  uint32_t voltage = (uint32_t)(sum / ADC_FILTERING_COUNT);
  uint8_t battery = map(voltage*2, 3700, 4200, 0, 100);
  Serial.printf("battery: %d \n", battery);
  return battery;
}


void setup()
{
  Serial.begin(9600);
  Serial.println("setup");
}

void loop() 
{
  delay(2000);
  get_battery_percentage();
}

