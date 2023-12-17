#include "task.h"

Button_t button;            // 声明一个按键对象
bool test_printer = false;  // 是否需要测试打印功能

// --------------------------- ↓ 按键任务相关函数 ↓ ---------------------------

// 单击事件--测试打印功能--用户无需调用！！
static void button_down_callback(void* btn)
{
    Serial.println("button press callback!");
    test_printer = true;
}

// 双击事件--无--用户无需调用！！
static void button_double_callback(void* btn)
{
    Serial.println("button double callback!");
}

// 长按事件--电机空转走纸--用户无需调用！！
static void button_long_callback(void* btn)
{
    Serial.println("button long callback!");
    device_state_struct device_state = get_device_state();
    // 在不缺纸并且没有进行打印的时候才允许电机空转
    if(device_state.paper_state == PAPER_NORMAL) {
        if(device_state.printer_state == PRINTER_INIT || device_state.printer_state == PRINTER_FINISH) {
            motor_start();  // 电机开始空转
        }
    } else {
        set_led_state(LED_ON);
        Serial.println("printer is not ready now!");
    }
}

// 长按释放事件--电机停止空转--用户无需调用！！
static void button_long_free_callback(void* btn)
{
    Serial.println("button long free callback!");
    motor_stop();           // 电机停止空转
    set_led_state(LED_OFF);
}

// 读按键电平的接口函数--用户无需调用！！
static uint8_t read_button_level(void)
{
    return digitalRead(KEY_PIN);
}

// 按键初始化函数
static void button_init(void)
{
    // 生成一个按键
    Button_Create(
        "Button1",
        &button,
        read_button_level,
        LOW
    );
    // 配置按键相关的事件触发回调函数
    Button_Attach(&button, BUTTON_DOWM, button_down_callback);
    Button_Attach(&button, BUTTON_DOUBLE, button_double_callback);
    Button_Attach(&button, BUTTON_LONG, button_long_callback);
    Button_Attach(&button, BUTTON_LONG_FREE, button_long_free_callback);
}
// --------------------------- ↑ 按键任务相关函数 ↑ ---------------------------

// --------------------------- ↓ 全局多任务相关函数 ↓ ---------------------------

// 设备状态上报任务执行函数
static void runReportTask(void)
{
    if(get_read_state_timeout() == true)    // 10s计时结束
    {
        read_all_hal();             // 读设备硬件状态：电量，打印头温度，缺纸情况
        ble_report();               // 执行上报
        clean_read_state_timeout(); // 清除计时标志位
    }
    if(get_paper_lack_flag() == true)      // 如果发现缺纸，需要立即上报
    {
        ble_report();       // 执行上报
    }
}

// 打印功能执行函数
void runPrintingTask(void)
{
    if(get_ble_write_finished() == true)    // 蓝牙打印数据写入完成
    {
        device_state_struct now_device_state = get_device_state();
        // 打印机当前是否处于就绪状态
        if(now_device_state.printer_state == PRINTER_INIT
        || now_device_state.printer_state == PRINTER_FINISH)
        {
            now_device_state.printer_state = PRINTER_START;     // 设置为开始打印状态
            Serial.println("printing started...");
            ble_report();
            set_ble_write_finished(false);  // 清空蓝牙打印数据写入完成的标志位
        }
        if(now_device_state.printer_state == PRINTER_START)     // 如果为开始打印状态
        {
            start_printing_by_queuebuf();   // 开始打印
            now_device_state.printer_state = PRINTER_FINISH;    // 设置为打印完成状态
        }
    }
}

// 上报任务
void task_report(void* pvParameter)
{
    for(;;)
    {
        runReportTask();
        vTaskDelay(100);
    }
}

// 按键任务
void task_button(void* pvParameter)
{
    for(;;)
    {
        Button_Process();
        vTaskDelay(20);
    }
}

// 打印任务
void task_print(void)
{
    for(;;)
    {
        runPrintingTask();
        // 是否进行打印测试
        if(test_printer) {
            testSTB();
            test_printer = false;
        }
    }
}

// 设备初始化函数
void initialization(void)
{
    Serial.begin(115200);
    device_state_init();
    hal_init();    
    read_state_timer_init();
    ble_init();
    printer_init();
    button_init();                 
    Serial.println("hardware initialization finished! task running now...");
    queue_init();
    xTaskCreate(
        task_report,
        "task_report",
        1024 * 10,
        NULL,
        1,
        NULL
    );
    xTaskCreate(
        task_button,
        "task_button",
        1024 * 5,
        NULL,
        0,
        NULL
    );
}

// --------------------------- ↑ 全局多任务相关函数 ↑ ---------------------------