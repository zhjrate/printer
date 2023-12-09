#include "my_hal.h"
#include "global/device.h"

bool need_report = false;

bool get_need_report_flag(void)
{
    return need_report;
}

void ARDUINO_ISR_ATTR paper_isr(void)
{
    need_report = true;
    get_device_state()->paper_state = PAPER_LACK;
    detachInterrupt(PAPER_PIN);
}

void read_paper_state(void)
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
