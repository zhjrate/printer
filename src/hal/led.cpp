#include "led.h"

// led初始化函数
void led_init(void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 1);
}

// 设置led状态函数
void set_led_state(LED_STATE led_state)
{
    switch(led_state)
    {
        // 蓝牙连接状态--LED快闪2下
        case LED_CONNECT:
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            us_delay(100000);
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            break;
        // 设备出错状态--LED快闪3下
        case LED_ERROR:
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            us_delay(100000);
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            us_delay(100000);
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            break;
        // 打印开始状态--LED快闪1下
        case LED_PRINTING_START:
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            break;
        // 蓝牙断开状态--LED快闪2下
        case LED_DISCONNECT:
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            us_delay(100000);
            digitalWrite(LED_PIN, 0);
            us_delay(100000);
            digitalWrite(LED_PIN, 1);
            break;
        // LED常亮状态
        case LED_ON:
            digitalWrite(LED_PIN, 0);
            break;
        // LED常灭状态
        case LED_OFF:
            digitalWrite(LED_PIN, 1);
            break;
        default:
            break;
    }
}