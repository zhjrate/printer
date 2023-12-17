#include "printer.h"

// 打印机在进行打印时，需要对异常状况进行侦听:  \
    1. 打印超时异常  \
    2. 缺纸异常     \
    3. 温度异常     \
遇到异常情况需要立即停止打印，并上报

// 为了打印效果更好,可以根据打印数据来调整通道加热时间,数据多的时候就多加热一会，少的话就少加热一会

float stb_add_time[6] = {0};    // 各通道需要增加的加热时间
const float kAddTime = 0.01;    // 加热时间计算系数
uint8_t heat_density = 60;      // 热密度
uint32_t stb_heat_time[6] = {PRINT_HEAT_TIME};  // 各通道加热时间
bool print_successfully = false;    // 打印成功标志位
#define ALL_STB_NUM 0xFF            // 所有通道都打印

// 设置打印头热密度系数
void set_heat_density(uint8_t density)
{
    heat_density = density;
}

// 清空加热时间增量数组
static void clear_add_time(void)
{
    for(size_t i=0; i<6; i++)
        stb_add_time[i] = 0;
}

// 设置各通道处于不打印的空闲状态
static void set_stb_idle(void)
{
    digitalWrite(STB1_PIN, LOW);
    digitalWrite(STB2_PIN, LOW);
    digitalWrite(STB3_PIN, LOW);
    digitalWrite(STB4_PIN, LOW);
    digitalWrite(STB5_PIN, LOW);
    digitalWrite(STB6_PIN, LOW);
}

// 打印机初始化函数
void printer_init(void)
{
    motor_init();
    // 引脚初始化
    pinMode(VH_EN_PIN, OUTPUT);     // 升压电源引脚
    pinMode(LAT_PIN, OUTPUT);       // 数据锁存引脚
    pinMode(SPI_SCL_PIN, OUTPUT);   // SPI总线配置为输出
    pinMode(SPI_SDA_PIN, OUTPUT);
    pinMode(STB1_PIN, OUTPUT);      // 各通道设置为输出
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

// 通过spi发送一行数据给打印头
static void send_oneline_data(uint8_t* data)
{
    clear_add_time();
    // 根据数据的多少计算打印时间，为了获得更好的打印效果
    for(size_t i=0; i<6; i++)
    {
        for(size_t j=0; j<8; j++)
        {
            stb_add_time[i] += data[8*i + j];
        }
        stb_add_time[i] = kAddTime * (stb_add_time[i] * stb_add_time[i]);
        stb_heat_time[i] = (PRINT_HEAT_TIME + stb_add_time[i]) * ((float)heat_density / 100);
    }
    spi_send_buffer(data, PRINTER_ONELINE_BYTE);    // spi发送一行数据
    digitalWrite(LAT_PIN, LOW);     // 拉低锁存引脚，锁存数据
    delayMicroseconds(LAT_TIME);    // 等待1us
    digitalWrite(LAT_PIN, HIGH);    // 拉高锁存引脚，数据锁存完成
}

// 打印前的准备工作
static void prepare_for_printing(void)
{
    open_print_timeout_timer();     // 开启超时侦听
    set_stb_idle();
    digitalWrite(LAT_PIN, HIGH);    // 锁存引脚拉高，打印开始后不允许再有数据传入
    digitalWrite(VH_EN_PIN, HIGH);  // 开启打印头电源
}

// 打印停止
static void printing_stop(void)
{
    close_print_timeout_timer();    // 关闭打印超时计时器
    digitalWrite(VH_EN_PIN, LOW);   // 关闭打印头电源
    set_stb_idle();
    digitalWrite(LAT_PIN, HIGH);
}

// 检查打印出错函数--缺纸，温度过高，打印超时
static bool print_error_check(void)
{
    if(get_print_timeout_error() == true)   // 打印超时异常
    {
        clear_queue();      // 清空打印队列，以免下次打印数据混叠
        Serial.println("print timeout");
        return true;
    }
    if(get_device_state().paper_state == PAPER_LACK)    // 缺纸异常
    {
        clear_queue();      // 清空打印队列，以免下次打印数据混叠
        Serial.println("paper lack");
        ble_report();
        return true;
    }
    if(get_device_state().temperature >= 60)            // 过热异常
    {
        clear_queue();      // 清空打印队列，以免下次打印数据混叠
        Serial.println("temperature is too high");
        ble_report();
        return true;
    }
    return false;
}

// 打印通道加热函数--进行打印
static void stb_run(uint8_t stb_num)
{
    switch(stb_num)
    {
        case 0:
            digitalWrite(STB1_PIN, HIGH);                   // 加热
            us_delay(stb_heat_time[0] + STB1_ADD_TIME);     // 加热时间
            digitalWrite(STB1_PIN, LOW);                    // 冷却
            us_delay(PRINT_COOL_TIME);                      // 冷却时间
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

// 打印功能辅助函数--用于控制打印通道的加热和电机走纸
static bool aux_printing_by_stb(bool need_stop, uint8_t stb_num)
{
    // 需要停止标志位为真
    if(need_stop)
    {
        // 停止打印
        motor_stop();
        printing_stop();
        return true;
    }
    // 执行打印
    motor_run_step(1);
    if(stb_num == ALL_STB_NUM)  // 所有通道一起加热
    {
        for(uint8_t i=0; i<6; i++)
        {
            // 将电机走纸放到加热时执行，提高打印流畅度
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

// 单通道打印函数--仅用于测试打印功能
static void printing_by_stb(uint8_t stb_num, uint8_t* buffer, uint32_t length)
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

// 队列打印函数
void start_printing_by_queuebuf(void)
{
    uint8_t* pdata;
    prepare_for_printing();     // 开启打印头电源，开启超时侦听
    print_successfully = false;
    Serial.println("printing start now...");
    set_led_state(LED_PRINTING_START);
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
            print_successfully = true;
            break;
        }
        if(print_error_check() == true) // 判断出异常，此时有可能数据没打印完，需要清理现场
        {
            set_led_state(LED_ERROR);
            break;
        }
    }
    if(print_successfully) {
        motor_run_step(230);    // 电机前进一小部分，方便取纸
        Serial.println("printing successfully!");
    }
    printing_stop();    // 打印头停止工作
    motor_stop();       // 电机停止
    Serial.println("printer stop now!");
}

// 用于测试的打印数据
static void setDebugData(uint8_t *print_data)
{
    for (uint32_t index = 0; index < 48 * 5; ++index)
    {
        //0X55 = 0101 0101 0为白，1为黑
        print_data[index] = 0x55;
    }
}

// 测试打印功能的函数
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
    motor_run_step(200);
    Serial.println("test complete.");
}