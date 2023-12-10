#include "queue.h"

print_buf_struct* ble_read_write_buffer_ptr;

QueueHandle_t queue_handler = NULL;

void queue_init(void)
{
    queue_handler = xSemaphoreCreateMutex();
}

void clear_queue(void)
{
    ble_read_write_buffer_ptr->line_need_to_print = 0;
    ble_read_write_buffer_ptr->read_index = 0;
    ble_read_write_buffer_ptr->write_index = 0;
}

void write_oneline_data_to_queue(uint8_t* pdata, size_t length)
{
    if(length == 0) // 没有待写入数据时，直接返回
        return;
    if(ble_read_write_buffer_ptr->line_need_to_print >= MAX_LINE)
    {
        ble_read_write_buffer_ptr->line_need_to_print = MAX_LINE;
        return;
    }
    if(length >= MAX_BYTE_ONELINE)
        length = MAX_BYTE_ONELINE;
    if(xSemaphoreTake(queue_handler, (portTickType)10) == pdPASS)
    {
        memcpy(&(ble_read_write_buffer_ptr->print_buffer[ble_read_write_buffer_ptr->write_index]),
                pdata, length);
        ble_read_write_buffer_ptr->write_index++;
        ble_read_write_buffer_ptr->line_need_to_print++;
        if(ble_read_write_buffer_ptr->write_index >= MAX_LINE)
            ble_read_write_buffer_ptr->write_index = 0;
        if(ble_read_write_buffer_ptr->line_need_to_print >= MAX_LINE)
            ble_read_write_buffer_ptr->line_need_to_print = MAX_LINE;
        xSemaphoreGive(queue_handler);
    }
}

uint8_t* read_oneline_data_from_queue(void)
{
    if(ble_read_write_buffer_ptr->line_need_to_print > 0)
    {
        if(xSemaphoreTake(queue_handler, (portTickType)10) == pdPASS)
        {
            ble_read_write_buffer_ptr->line_need_to_print--;
            size_t index = ble_read_write_buffer_ptr->read_index;
            ble_read_write_buffer_ptr->read_index++;
            if(ble_read_write_buffer_ptr->read_index >= MAX_LINE)
                ble_read_write_buffer_ptr->read_index = 0;
            xSemaphoreGive(queue_handler);
            return ble_read_write_buffer_ptr->print_buffer[index].buffer;
        }
    }
    return NULL;
}