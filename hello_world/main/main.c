#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "max30003.h"

void app_main(void)
{
    //init SPI pin with your setup
    max30003_initchip(PIN_SPI_MISO,PIN_SPI_MOSI,PIN_SPI_SCK,PIN_SPI_CS);
    vTaskDelay(10/ portTICK_PERIOD_MS);

    while(1)
    {
        long db = max30003_read_ecg_data();
        ESP_LOGI("SWU_BME", "%ld", db);
        // printf("%ld, ", db);
        vTaskDelay(8/ portTICK_PERIOD_MS);
    }
}
