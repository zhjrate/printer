#include "spi.h"
SPIClass hspi = SPIClass(HSPI);
SPISettings printer_spi_settings = SPISettings(SPI_CLK, SPI_MSBFIRST, SPI_MODE0);

void spi_init(void)
{
  hspi.begin(SPI_SCL_PIN, 16, SPI_SDA_PIN, -1);
  hspi.setFrequency(2000000);
}

void spi_send_buffer(uint8_t* buffer, uint32_t size)
{
  hspi.beginTransaction(printer_spi_settings);
  hspi.transfer(buffer, size);
  hspi.endTransaction();
}
