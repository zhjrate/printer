#ifndef _QUEUE_H
#define _QUEUE_H

#include <Arduino.h>

#define MAX_BYTE_ONELINE 48     // 每行最大字节数
#define MAX_LINE 1000           // 最大行数

typedef struct{
    uint8_t buffer[MAX_BYTE_ONELINE];   // 存放一行数据的缓冲区数组
}oneline_buf_struct;

typedef struct{
    oneline_buf_struct print_buffer[MAX_LINE];  // 存放所有接收数据的打印机缓冲区数组
    size_t read_index;                          // 读数据索引值
    size_t write_index;                         // 写数据索引值
    size_t line_need_to_print;                  // 当前缓冲区需要打印的数据的行数
}print_buf_struct;

void queue_init(void);
void clear_queue(void);
void write_oneline_data_to_queue(uint8_t* pdata, size_t length);
uint8_t* read_oneline_data_from_queue(void);

#endif