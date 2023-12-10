#include "task.h"

void runReportTask(void)
{
    if(get_read_state_timeout() == true)
    {
        read_all_hal();
        ble_report();
        clean_read_state_timeout();
    }
    if(get_need_report_flag() == true)
    {
        ble_report();
    }
}

void runButtonTask(void)
{

}

void runPrintingTask(void)
{
    if(get_ble_write_finished() == true)
    {
        device_state_struct* now_device_state = get_device_state();
        if(now_device_state->printer_state == PRINTER_INIT
        || now_device_state->printer_state == PRINTER_FINISH)
        {
            now_device_state->printer_state = PRINTER_START;
            Serial.println("printing started...");
            ble_report();
            set_ble_write_finished(false);
        }
        if(now_device_state->printer_state == PRINTER_START)
        {
            start_printing_by_queuebuf();
            now_device_state->printer_state = PRINTER_FINISH;
        }
    }
}

void task_report(void* pvParameter)
{
    for(;;)
    {
        runReportTask();
        vTaskDelay(100);
    }
}

void task_button(void* pvParameter)
{
    for(;;)
    {
        runButtonTask();
        vTaskDelay(20);
    }
}

void task_print(void)
{
    for(;;)
    {
        runPrintingTask();
    }
}

void initialization(void)
{
    Serial.begin(115200);
    Serial.println("initialization task...");
    device_state_init();
    hal_init();
    read_state_timer_init();
    queue_init();
    ble_init();
    printer_init();
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