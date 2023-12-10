#include "timer.h"
#include "my_hal.h"

Ticker print_timeout_timer;
Ticker read_state_timer;

bool print_timeout_error = false;
bool read_state_timeout = false;

static void print_timeout_callback(void)
{
    print_timeout_error = true;
}

void open_print_timeout_timer(void)
{
    print_timeout_error = false;
    print_timeout_timer.attach_ms(20000, print_timeout_callback);
}

void close_print_timeout_timer(void)
{
    print_timeout_error = false;
    if(print_timeout_timer.active() == true)
        print_timeout_timer.detach();
}

bool get_print_timeout_error(void)
{
    return print_timeout_error;
}

void read_state_callback(void)
{
    read_state_timeout = true;
}

void read_state_timer_init(void)
{
    read_state_timeout = false;
    read_state_timer.attach_ms(10000, read_state_callback);
}

bool get_read_state_timeout(void)
{
    return read_state_timeout;
}

void clean_read_state_timeout(void)
{
    read_state_timeout = false;
}