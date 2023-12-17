#include <Arduino.h>
#include "global/task.h"

void setup()
{
  initialization();
}

void loop() 
{
  task_print();
}