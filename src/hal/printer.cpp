#include "printer.h"

// 打印机在进行打印时，需要对异常状况进行侦听:
//      1. 打印超时异常  2. 缺纸异常     3. 温度异常
// 遇到异常情况需要立即停止打印，并上报

// 为了打印效果更好,可以根据打印数据来调整通道加热时间,数据多的时候就多加热一会，少的话就少加热一会

float stb_add_time[6] = {0};
const float kAddTime = 0.01;
uint8_t heat_density = 60;  // 热密度
uint32_t stb_heat_time[6] = {PRINT_HEAT_TIME};
#define ALL_STB_NUM 0xFF    // 所有通道都打印

void set_heat_density(uint8_t density)
{
    heat_density = density;
}

void clear_add_time(void)
{
    for(size_t i=0; i<6; i++)
        stb_add_time[i] = 0;
}

void set_stb_idle(void)
{
    digitalWrite(STB1_PIN, LOW);
    digitalWrite(STB2_PIN, LOW);
    digitalWrite(STB3_PIN, LOW);
    digitalWrite(STB4_PIN, LOW);
    digitalWrite(STB5_PIN, LOW);
    digitalWrite(STB6_PIN, LOW);
}

void printer_init(void)
{
    motor_init();
    // 引脚初始化
    pinMode(VH_EN_PIN, OUTPUT);
    pinMode(LAT_PIN, OUTPUT);
    pinMode(SPI_SCL_PIN, OUTPUT);
    pinMode(SPI_SDA_PIN, OUTPUT);
    pinMode(STB1_PIN, OUTPUT);
    pinMode(STB2_PIN, OUTPUT);
    pinMode(STB3_PIN, OUTPUT);
    pinMode(STB4_PIN, OUTPUT);
    pinMode(STB5_PIN, OUTPUT);
    pinMode(STB6_PIN, OUTPUT);
    // 关闭打印头加热通道
    set_stb_idle();
    // 关闭打印头电源
    digitalWrite(VH_EN_PIN, LOW);
    // 初始化spi
    spi_init();
}

void send_oneline_data(uint8_t* data)
{
    clear_add_time();
    for(size_t i=0; i<6; i++)
    {
        for(size_t j=0; j<8; j++)
        {
            stb_add_time[i] += data[8*i + j];
        }
        stb_add_time[i] = kAddTime * (stb_add_time[i] * stb_add_time[i]);
        stb_heat_time[i] = (PRINT_HEAT_TIME + stb_add_time[i]) * ((float)heat_density / 100);
    }
    spi_send_buffer(data, PRINTER_ONELINE_BYTE);
    digitalWrite(LAT_PIN, LOW);
    delayMicroseconds(LAT_TIME);
    digitalWrite(LAT_PIN, HIGH);
}

void prepare_for_printing(void)
{
    open_print_timeout_timer();     // 开启超时侦听
    set_stb_idle();
    digitalWrite(LAT_PIN, HIGH);
    digitalWrite(VH_EN_PIN, HIGH);  // 开启打印头电源
}

void printing_stop(void)
{
    close_print_timeout_timer();
    digitalWrite(VH_EN_PIN, LOW);   // 关闭打印头电源
    set_stb_idle();
    digitalWrite(LAT_PIN, HIGH);
}

bool print_error_check(void)
{
    if(get_print_timeout_error() == true)
    {
        clear_queue();
        Serial.println("print timeout");
        return true;
    }
    if(get_device_state()->paper_state == PAPER_LACK)
    {
        clear_queue();
        Serial.println("paper lack");
        ble_report();
        return true;
    }
    if(get_device_state()->temperature >= 60)
    {
        clear_queue();
        Serial.println("temperature is too high");
        ble_report();
        return true;
    }
    return false;
}

void stb_run(uint8_t stb_num)
{
    switch(stb_num)
    {
        case 0:
            digitalWrite(STB1_PIN, HIGH);
            us_delay(stb_heat_time[0] + STB1_ADD_TIME);
            digitalWrite(STB1_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
        case 1:
            digitalWrite(STB2_PIN, HIGH);
            us_delay(stb_heat_time[1] + STB2_ADD_TIME);
            digitalWrite(STB2_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
        case 2:
            digitalWrite(STB3_PIN, HIGH);
            us_delay(stb_heat_time[2] + STB3_ADD_TIME);
            digitalWrite(STB3_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
        case 3:
            digitalWrite(STB4_PIN, HIGH);
            us_delay(stb_heat_time[3] + STB4_ADD_TIME);
            digitalWrite(STB4_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
        case 4:
            digitalWrite(STB5_PIN, HIGH);
            us_delay(stb_heat_time[4] + STB5_ADD_TIME);
            digitalWrite(STB5_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
        case 5:
            digitalWrite(STB6_PIN, HIGH);
            us_delay(stb_heat_time[5] + STB6_ADD_TIME);
            digitalWrite(STB6_PIN, LOW);
            us_delay(PRINT_COOL_TIME);
            break;
    }
}

bool aux_printing_by_stb(bool need_stop, uint8_t stb_num)
{
    if(need_stop)
    {
        motor_stop();
        printing_stop();
        return true;
    }
    motor_run_step(1);
    if(stb_num == ALL_STB_NUM)
    {
        for(uint8_t i=0; i<6; i++)
        {
            stb_run(i);
            if(i==1 || i==3 || i==5)
                motor_run_step(1);
        }
    }
    else
    {
        stb_run(stb_num);
        motor_run_step(3);
    }
    return false;
}

void printing_by_stb(uint8_t stb_num, uint8_t* buffer, uint32_t length)
{
  uint32_t offset = 0;
  uint8_t* data_ptr = buffer;
  bool need_stop = false;
  prepare_for_printing();
  while(1)
  {
    if(offset < length)
    {
      send_oneline_data(data_ptr);
      offset += 48;
      data_ptr += 48;
    }
    else
      need_stop = true;
    if(aux_printing_by_stb(need_stop, stb_num) == true)
      break;
  }
  motor_run_step(40);
  motor_stop();
}

void start_printing_by_queuebuf(void)
{
    uint8_t* pdata;
    prepare_for_printing();     // 开启打印头电源，开启超时侦听
    while(1)
    {
        if(get_line_need_to_print() > 0)    // 数据还没打完
        {
            pdata = read_oneline_data_from_queue();     // 读入一行数据
            if(pdata != NULL)                           // 读入成功
            {
                send_oneline_data(pdata);               // 发送到打印模组
                aux_printing_by_stb(false, ALL_STB_NUM);// 进行打印
            }
        }       
        else    // 数据打完了
        {
            printing_stop();    // 打印头停止工作
            break;
        }
        if(print_error_check() == true) // 判断出异常，此时有可能数据没打印完，需要清理现场
        {
            printing_stop();    // 打印头停止工作
            break;
        }
    }
    motor_run_step(230);
    motor_stop();
}

static void setDebugData(uint8_t *print_data)
{
    for (uint32_t index = 0; index < 48 * 5; ++index)
    {
        //0X55 = 0101 0101 0为白，1为黑
        print_data[index] = 0x55;
    }
}

void testSTB()
{
    //每行48byte 1byte=8bit 384bit
    //48*5=5行
    uint8_t print_data[48*5];
    uint32_t print_len;
    Serial.println("start printing test...");
    print_len = 48*5;
    //设置打印的数据内容
    setDebugData(print_data);
    //通道0打印5行
    printing_by_stb(0, print_data, print_len);
    setDebugData(print_data);
    //通道1打印5行
    printing_by_stb(1, print_data, print_len);
    setDebugData(print_data);
    printing_by_stb(2, print_data, print_len);
    setDebugData(print_data);
    printing_by_stb(3, print_data, print_len);
    setDebugData(print_data);
    printing_by_stb(4, print_data, print_len);
    setDebugData(print_data);
    printing_by_stb(5, print_data, print_len);
    Serial.println("test complete.");
}