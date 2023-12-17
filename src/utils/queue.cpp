#include "queue.h"

print_buf_struct ble_read_write_buffer;

QueueHandle_t queue_handler = NULL;

// 队列初始化
void queue_init(void)
{
    // 创建互斥信号量
    queue_handler = xSemaphoreCreateMutex();
}

// 清空队列--实际上是清空索引值
void clear_queue(void)
{
    ble_read_write_buffer.line_need_to_print = 0;
    ble_read_write_buffer.read_index = 0;
    ble_read_write_buffer.write_index = 0;
    Serial.println("queue is clear now");
}

// 往队列中写入一行数据
void write_oneline_data_to_queue(uint8_t* pdata, size_t length)
{
    if(length == 0) // 没有待写入数据时，直接返回
        return;
    // 当前缓冲区需要打印的数据行数超出队列允许最大行数时，直接截断
    if(ble_read_write_buffer.line_need_to_print >= MAX_LINE)
    {
        ble_read_write_buffer.line_need_to_print = MAX_LINE;
        return;
    }
    // 一行需要打印的字节数大于队列允许的最大字节数时，直接截断
    if(length >= MAX_BYTE_ONELINE)
        length = MAX_BYTE_ONELINE;
    // 数据正常写入队列
    if(xSemaphoreTake(queue_handler, (portTickType)10) == pdPASS)   // 用10个滴答时钟周期试图获取互斥量
    {
        memcpy(&(ble_read_write_buffer.print_buffer[ble_read_write_buffer.write_index]),
                pdata, length);     // 写入一行数据
        // 递增相关的索引值
        ble_read_write_buffer.write_index++;       // 写数据索引指向下一行
        ble_read_write_buffer.line_need_to_print++;
        if(ble_read_write_buffer.write_index >= MAX_LINE)
            ble_read_write_buffer.write_index = 0;
        if(ble_read_write_buffer.line_need_to_print >= MAX_LINE)
            ble_read_write_buffer.line_need_to_print = MAX_LINE;
        xSemaphoreGive(queue_handler);      // 释放互斥量
    }
}

// 读出队列中的一行数据
uint8_t* read_oneline_data_from_queue(void)
{
    if(ble_read_write_buffer.line_need_to_print > 0)   // 队列中是否有东西可读
    {
        if(xSemaphoreTake(queue_handler, (portTickType)10) == pdPASS)   // 用10个滴答时钟周期试图获取互斥量
        {
            // 递减相关的索引值
            ble_read_write_buffer.line_need_to_print--;
            size_t index = ble_read_write_buffer.read_index;
            ble_read_write_buffer.read_index++;    // 读数据索引指向下一行
            if(ble_read_write_buffer.read_index >= MAX_LINE)
                ble_read_write_buffer.read_index = 0;
            xSemaphoreGive(queue_handler);      // 释放互斥量
            return ble_read_write_buffer.print_buffer[index].buffer;   // 返回单行数据缓冲区
        }
    }
    return NULL;
}

// 获取队列当前需要打印的数据行数
size_t get_line_need_to_print(void)
{
    return ble_read_write_buffer.line_need_to_print;
}