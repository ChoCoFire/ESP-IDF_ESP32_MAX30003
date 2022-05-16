#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/spi_master.h"

#include "max30003.h"

uint8_t SPI_TX_Buff[4];
uint8_t SPI_RX_Buff[10];

spi_device_handle_t spi;
char SPI_temp_32b[4];
signed long ecgdata;
unsigned long data;

//This function is called (in irq context!) just before a transmission starts.
void max30003_spi_pre_transfer_callback(spi_transaction_t *t)
{
;
}

void max30003_initchip(int pin_miso, int pin_mosi, int pin_sck, int pin_cs )
{
    esp_err_t ret;

    spi_bus_config_t buscfg=
    {
        .miso_io_num=pin_miso,
        .mosi_io_num=pin_mosi,
        .sclk_io_num=pin_sck,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    spi_device_interface_config_t devcfg=
    {
        .clock_speed_hz=14000000,             	                //Clock out at 10 MHz
        .mode=0,                               	                //SPI mode 0,1,2,3
        .spics_io_num=pin_cs,              		                //CS pin
        .queue_size=7,                                          //We want to be able to queue 7 transactions at a time
        .pre_cb=max30003_spi_pre_transfer_callback,             //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 0);				//use 1 instead of 0 to enable dma
    assert(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    assert(ret==ESP_OK);
    // max30003_start_timer();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    max30003_sw_reset();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    MAX30003_Reg_Write(CNFG_GEN, 0x080004);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    MAX30003_Reg_Write(CNFG_CAL, 0x720000);  // 0x700000
    vTaskDelay(100 / portTICK_PERIOD_MS);

    MAX30003_Reg_Write(CNFG_EMUX,0x0B0000);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    MAX30003_Reg_Write(CNFG_ECG, 0x400000);  // 256 sps
    vTaskDelay(100 / portTICK_PERIOD_MS);

    MAX30003_Reg_Write(CNFG_RTOR1,0x3fc600);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    unsigned mngr_int = 0x4 | (SAMPLES_PER_PACKET - 1) << 19;
    MAX30003_Reg_Write(MNGR_INT, mngr_int);
    vTaskDelay(100 / portTICK_PERIOD_MS);

	MAX30003_Reg_Write(EN_INT,0x000400);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    max30003_synch();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // MAX30003_init_sequence();
}

void MAX30003_Reg_Write (unsigned char WRITE_ADDRESS, unsigned long data)
{
  uint8_t wRegName = (WRITE_ADDRESS<<1) | WREG;

  uint8_t txData[4];

  txData[0]=wRegName;
  txData[1]=(data>>16);
  txData[2]=(data>>8);
  txData[3]=(data);

  esp_err_t ret;
  spi_transaction_t t;

  memset(&t, 0, sizeof(t));             //Zero out the transaction

  t.length=32;                          //Len is in bytes, transaction length is in bits.
  t.tx_buffer=&txData;                  //Data
  ret=spi_device_transmit(spi, &t);     //Transmit!
  assert(ret==ESP_OK);                  
}

void max30003_sw_reset(void)
{
    MAX30003_Reg_Write(SW_RST,0x000000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void max30003_synch(void)
{
    MAX30003_Reg_Write(SYNCH,0x000000);
}

void max30003_reg_read(unsigned char WRITE_ADDRESS)
{
    uint8_t Reg_address=WRITE_ADDRESS;

    SPI_TX_Buff[0] = (Reg_address<<1 ) | RREG;
    SPI_TX_Buff[1]=0x00;
    SPI_TX_Buff[2]=0x00;
    SPI_TX_Buff[3]=0x00;

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction

    t.length=32;
    t.rxlength=32;
    t.tx_buffer=&SPI_TX_Buff;
    t.rx_buffer=&SPI_RX_Buff;

    t.user=(void*)0;
    ret=spi_device_transmit(spi, &t);
    assert(ret==ESP_OK);            //Should have had no issues.

    SPI_temp_32b[0] = SPI_RX_Buff[1];
    SPI_temp_32b[1] = SPI_RX_Buff[2];
    SPI_temp_32b[2] = SPI_RX_Buff[3];

}

long max30003_read_ecg_data(void)
{
      max30003_reg_read(ECG_FIFO);

      unsigned long data0 = (unsigned long) (SPI_temp_32b[0]);
      data0 = data0 <<24;
      unsigned long data1 = (unsigned long) (SPI_temp_32b[1]);
      data1 = data1 <<16;
      unsigned long data2 = (unsigned long) (SPI_temp_32b[2]);
      data2 = data2 & 0xc0;
      data2 = data2 << 8;
      data = (unsigned long) (data0 | data1 | data2);
      ecgdata = (signed long) (data);

      return ecgdata;
}

