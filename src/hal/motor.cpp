#include "motor.h"
#include <Ticker.h>

// 电机4相8拍时序表
const uint8_t motor_table[8][4] =
{
  {1, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 1, 0, 1},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {1, 0, 1, 0},
  {1, 0, 0, 0}
};

Ticker motor_timer;     // 步进电机计时器
uint8_t motor_pos = 0;  // 步进电机位置

// 初始化步进电机
void motor_init(void)
{
  pinMode(MOTOR_AP_PIN, OUTPUT);
  pinMode(MOTOR_AN_PIN, OUTPUT);
  pinMode(MOTOR_BP_PIN, OUTPUT);
  pinMode(MOTOR_BN_PIN, OUTPUT);

  digitalWrite(MOTOR_AP_PIN, 0);
  digitalWrite(MOTOR_AN_PIN, 0);
  digitalWrite(MOTOR_BP_PIN, 0);
  digitalWrite(MOTOR_BN_PIN, 0);
}

// 步进电机计时器回调函数
void motor_timer_callback(void)
{
  // 按照时序表循环执行，步进电机持续运转
  digitalWrite(MOTOR_AP_PIN, motor_table[motor_pos][0]);
  digitalWrite(MOTOR_AN_PIN, motor_table[motor_pos][1]);
  digitalWrite(MOTOR_BP_PIN, motor_table[motor_pos][2]);
  digitalWrite(MOTOR_BN_PIN, motor_table[motor_pos][3]);
  motor_pos++;
  if(motor_pos >= 8)
    motor_pos = 0;
}

// 步进电机开始运转
void motor_start(void)
{
  if(motor_timer.active() == false)
  {
    // 启动计时器，每2ms中断一次，频率为500Hz
    motor_timer.attach_ms(MOTOR_WAIT_TIME, motor_timer_callback);
    Serial.printf("motor start now.\n");
  }
}

// 步进电机停止运转
void motor_stop(void)
{
  digitalWrite(MOTOR_AP_PIN, 0);
  digitalWrite(MOTOR_AN_PIN, 0);
  digitalWrite(MOTOR_BP_PIN, 0);
  digitalWrite(MOTOR_BN_PIN, 0);
  if(motor_timer.active())
  {
    // 关闭计时器
    motor_timer.detach();
    Serial.printf("motor stop now.\n");
  }
}

// 步进电机运转多步
void motor_run_step(uint32_t steps)
{
  while(steps)
  {
    digitalWrite(MOTOR_AP_PIN, motor_table[motor_pos][0]);
    digitalWrite(MOTOR_AN_PIN, motor_table[motor_pos][1]);
    digitalWrite(MOTOR_BP_PIN, motor_table[motor_pos][2]);
    digitalWrite(MOTOR_BN_PIN, motor_table[motor_pos][3]);
    us_delay(MOTOR_WAIT_TIME_US);
    motor_pos++;
    if(motor_pos >= 8)
      motor_pos = 0;
    steps--;
  }
}