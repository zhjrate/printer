#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>

//----------------------------- 引脚编号 ↓ -----------------------------
#define LED_PIN 18          // LED灯引脚
#define BATTERY_ADC_PIN 34  // 电量检测功能引脚
#define PAPER_PIN 35        // 缺纸检测功能引脚
#define TEMP_ADC_PIN 36     // 温度检测功能引脚
//----------------------------- 引脚编号 ↑ -----------------------------

//----------------------------- 参数配置 ↓ -----------------------------
#define ADC_FILTERING_COUNT 10  // ADC 采集数据时的滤波次数
#define ADC_RESOLUTION_BIT 12   // ADC 分辨率(bit)
#define BATTERY_LOW 3700        // 低电量电压(mV)
#define BATTERY_FULL 4200       // 满电量电压(mV)
//----------------------------- 参数配置 ↑ -----------------------------

#endif