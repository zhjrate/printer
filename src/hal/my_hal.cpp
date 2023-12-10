#include "my_hal.h"
#include "global/device.h"

bool paper_lack_need_report = false;

bool get_need_report_flag(void)
{
    if(paper_lack_need_report == true)
    {
        paper_lack_need_report = false;
        return true;
    }
    return false;
}

static void ARDUINO_ISR_ATTR paper_isr(void)
{
    paper_lack_need_report = true;
    get_device_state()->paper_state = PAPER_LACK;
    detachInterrupt(PAPER_PIN);
}

static void read_paper_state(void)
{
    if(digitalRead(PAPER_PIN) == PAPER_NORMAL)
    {
        get_device_state()->paper_state = PAPER_NORMAL;
        attachInterrupt(PAPER_PIN, paper_isr, RISING);
    }
    else
    {
        get_device_state()->paper_state = PAPER_LACK;
    }
}

static void read_temperature_state(void)
{
    uint8_t temp = (uint8_t)get_temperature();
    get_device_state()->temperature = temp;
}

static void read_battery_state(void)
{
    get_device_state()->battery = get_battery_percentage();
}

void read_all_hal(void)
{
    read_paper_state();
    read_temperature_state();
    read_battery_state();
}

void hal_init(void)
{
    led_init();
    analogReadResolution(ADC_RESOLUTION_BIT);
    pinMode(PAPER_PIN, INPUT);
}