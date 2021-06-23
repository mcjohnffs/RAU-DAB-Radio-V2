/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"





uint32_t adc_wert;


void adc_read(void *pvParameter)
{

while(1)
    {
    printf("reading analog data...");
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
    adc_wert = adc1_get_raw(ADC1_CHANNEL_0); 
    vTaskDelay(20);
    
}
 vTaskDelete( NULL );
}

void adc_print(void *pvParameter)
{   

     while (1)
     {
    
    printf(" adc data = %i\n", adc_wert);
    vTaskDelay(20);

     }
    
vTaskDelete( NULL );
    }


  


 
void app_main()
{
    xTaskCreate(&adc_read, "adc_read", 2048, NULL, 1, NULL);
    xTaskCreate(&adc_print, "adc_print", 2048, NULL, 2, NULL);
    

}