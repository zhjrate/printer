/************************************************************
  * @brief   引脚定义以及全局参数配置
  * @param   NULL
  * @return  NULL
  * @author  yujc
  * @github  https://github.com/JC-Yu
  * @date    2023-12-16
  * @version v1.0
  * @note    config.h
  ***********************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>

//----------------------------- 引脚编号 ↓ -----------------------------
#define KEY_PIN 5           // 按键引脚
#define SPI_SDA_PIN 13      // SPI数据引脚
#define SPI_SCL_PIN 15      // SPI时钟引脚
#define LED_PIN 18          // LED灯引脚
#define MOTOR_AP_PIN 23     // 电机 A+ 引脚
#define MOTOR_AN_PIN 22     // 电机 A- 引脚
#define MOTOR_BP_PIN 21     // 电机 B+ 引脚
#define MOTOR_BN_PIN 19     // 电机 B- 引脚
#define BATTERY_ADC_PIN 34  // 电量检测功能引脚
#define PAPER_PIN 35        // 缺纸检测功能引脚
#define TEMP_ADC_PIN 36     // 温度检测功能引脚
#define VH_EN_PIN 17        // 打印头升压电源引脚
#define STB1_PIN 26         // 打印头通道1
#define STB2_PIN 27         // 打印头通道2
#define STB3_PIN 14         // 打印头通道3
#define STB4_PIN 32         // 打印头通道4
#define STB5_PIN 33         // 打印头通道5
#define STB6_PIN 25         // 打印头通道6
#define LAT_PIN 12          // 打印头数据锁存引脚
//----------------------------- 引脚编号 ↑ -----------------------------

//----------------------------- 参数配置 ↓ -----------------------------
#define ADC_FILTERING_COUNT 20  // ADC 采集数据时的滤波次数
#define ADC_RESOLUTION_BIT 12   // ADC 分辨率(bit)
#define BATTERY_LOW 3700        // 低电量电压(mV)
#define BATTERY_FULL 4100       // 满电量电压(mV)
#define DEVICE_NAME "Mini-Printer"                                  // 设备名称
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"         // 蓝牙服务UUID
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"  // 蓝牙特征UUID
#define MOTOR_WAIT_TIME 2       // 电机一步等待时间(ms)
#define MOTOR_WAIT_TIME_US 2000 // 电机一步等待时间(us)
#define SPI_CLK 1000000         // SPI 时钟频率
#define PRINTER_ONELINE_BYTE 48 // 打印机每行字节数
#define LAT_TIME 1              // 锁存时间(us)
#define PRINT_HEAT_TIME 2600    // 打印头加热基本时间
#define PRINT_COOL_TIME 200     // 打印头冷却基本时间
#define STB1_ADD_TIME 0         // 通道1多加热时间(us)
#define STB2_ADD_TIME 0         // 通道2多加热时间(us)
#define STB3_ADD_TIME 0         // 通道3多加热时间(us)
#define STB4_ADD_TIME 0         // 通道4多加热时间(us)
#define STB5_ADD_TIME 0         // 通道5多加热时间(us)
#define STB6_ADD_TIME 0         // 通道6多加热时间(us)
#define SHORT_PRESS_TIME 1000   // 按键短按时间(ms)
//----------------------------- 参数配置 ↑ -----------------------------
#define us_delay(us) delayMicroseconds(us)  // 微秒延时函数

#endif