# printer
*嵌入式创新实践项目——便携式热敏打印机 from SCUT*



## 项目开发日志

### 2023/11/27

---

1、完成板卡的上电测试，编写了Hello World程序来验证板卡工作正常

2、编写LED状态指示程序，实现快闪，慢闪，常亮，常灭

```c++
#include <Arduino.h>

#define LED_PIN 18

typedef enum
{
  LED_ON = 0,
  LED_OFF,
  LED_SLOW_FLASH,
  LED_QUICK_FLASH,
} LED_STATUS;

void led_flash(int ms)
{
  digitalWrite(LED_PIN, LOW);
  delay(ms);
  digitalWrite(LED_PIN, HIGH);
  delay(ms);
}

void set_led_status(const LED_STATUS& state)
{
  switch (state)
  {
  case LED_ON:
    digitalWrite(LED_PIN, LOW);
    break;
  case LED_OFF:
    digitalWrite(LED_PIN, HIGH);
    break;
  case LED_SLOW_FLASH:
    led_flash(1000);
    break;
  case LED_QUICK_FLASH:
    led_flash(200);
    break;
  }
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(LED_PIN, OUTPUT);
}

void loop() 
{
  set_led_status(LED_QUICK_FLASH);
}
```

### 2023/11/28

---

1、编写按键驱动程序，实现短按，长按按下，长按释放的判别

```c++
#include <Arduino.h>

#define LED_PIN 18
#define KEY_PIN 5

#define SHORT_PRESS_TIME 1000

typedef enum
{
  SHORT_PRESS = 0,
  LONG_PRESS,
  LONG_PRESS_FREE,
  FREE,
} KEY_STATUS;

bool key_is_press = false;
unsigned long click_time = 0;
bool display_once = false;
KEY_STATUS key_status = FREE;

void key_detect(void)
{
  // 按键没按下时
  if(!key_is_press)
  {
    if(digitalRead(KEY_PIN) == LOW)
    {
      delay(10);
      if(digitalRead(KEY_PIN) == LOW)
      {
        click_time = millis();  // 记录按下时刻
        key_is_press = true;
      }
    }
  }
  // 按键按下了
  if(key_is_press)
  {
    if(digitalRead(KEY_PIN) == HIGH)
    {
      delay(10);
      if(digitalRead(KEY_PIN) == HIGH)
      {
        if(key_status == LONG_PRESS)
        {
          key_status = LONG_PRESS_FREE;
          display_once = false;
          Serial.print("long press free\n");
        }
        else
        {
          key_status = SHORT_PRESS;
          Serial.print("short press free\n");
        }
        key_is_press = false;
        return;
      }
    }
    if(millis() - click_time > SHORT_PRESS_TIME)
    {
      key_status = LONG_PRESS;
      if(!display_once)
      {
        Serial.print("long press hold\n");
        display_once = true;
      }
    }
  }

}

void setup() 
{
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(KEY_PIN, INPUT);
}

void loop() 
{
  key_detect();
}
```

2、电量检测功能：在串口打印电量百分比

```c++
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
```

