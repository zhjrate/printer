#ifndef _SPI_H
#define _SPI_H

#include "global/config.h"
#include <SPI.h>

void spi_init(void);
void spi_send_buffer(uint8_t* buffer, uint32_t size);

#endif