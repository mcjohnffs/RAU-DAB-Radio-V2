#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "freertos/semphr.h"

#define GRUEN 4
#define ROT 2

uint32_t adc_wert = 0;

SemaphoreHandle_t xSemaphore;



// xQueueHandle queue_adc;

void taskadc(void *pvParameters) //task zum adddc wert lese und in queue setze
{
    xSemaphore = xSemaphoreCreateBinary();
    

    while (1)
    {
        xSemaphoreTake(xSemaphore, 30);
        printf("taskadc: Using Semaphore.... \n");
        

        if (xSemaphore != NULL)
        {
            // da wird de adc wert glese am GPIO PIN 36
            adc1_config_width(ADC_WIDTH_BIT_12);
            adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
            adc_wert = adc1_get_raw(ADC1_CHANNEL_0);
            // das chöntsch no uskommentiere und sege was ADC_WIDTH_BIT_12, ADC1_CHANNEL_0 und ADC_ATTEN_DB_11 isch

            // glesene wert (&adc_wert) wird ad front vode queue (queue_adc) gschickt, dzahl am schluss sind ticks wos wartet
            // xQueueSendToBack(queue_adc, &adc_wert, 100);
            xSemaphoreGive(xSemaphore);
            printf("Current ADC Value = %i, Semaphore returned\n", adc_wert);
        }
    vTaskDelay(5000);
    }
        

}
void taskled(void *pvParameters) // task zum adc wert vode queue zhole und mit LED's azeige
{

    while (1)
    {

        if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)

        {
            printf("taskled: Using Semaphore.... \n");

            // anstatt xQueueReceive chamer au xQueuePeek mache, de Unterschied ish das xQueuePeek es ned uselöscht glaub****
            // xQueueReceive(queue_adc, &adc_wert, 200);

            // abfrag vode adc wert (rohi date) und azeig mit leds
            if (adc_wert > 2200)
            {
                gpio_set_direction(GRUEN, GPIO_MODE_OUTPUT);
                gpio_set_direction(ROT, GPIO_MODE_OUTPUT);
                gpio_pad_select_gpio(ROT);
                gpio_set_level(ROT, 1);
                gpio_pad_select_gpio(GRUEN);
                gpio_set_level(GRUEN, 0);
            }

            else
            {
                gpio_set_direction(GRUEN, GPIO_MODE_OUTPUT);
                gpio_set_direction(ROT, GPIO_MODE_OUTPUT);
                gpio_pad_select_gpio(GRUEN);
                gpio_set_level(GRUEN, 1);
                gpio_pad_select_gpio(ROT);
                gpio_set_level(ROT, 0);
            }

            xSemaphoreGive(xSemaphore);
            if (xSemaphore != NULL)
            {

                printf("Semaphore & ADC value received, Led's set and Semaphore returned \n");
            }

            vTaskDelay(20);
        }
    }
    vTaskDelete(NULL);
}

void app_main()
{
    // da wird queue created mit 3 slots und maximalem size vom adc wert bzw. de integer halt^^ (uint32_t)
    // queue_adc = xQueueCreate(3, sizeof(uint32_t));

    /* Create the semaphore to guard a shared resource.  As we are using
    the semaphore for mutual exclusion we create a mutex semaphore
    rather than a binary semaphore. */

    
    // wen "CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE" bruchsch wird glaub de speicher selber zeugwise
    xTaskCreate(&taskadc, "taskadc", CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(&taskled, "taskled", CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE, NULL, 2, NULL);
    

    ///////////////////////////old stuff/////////<3///////////////////////
    /*
     xTaskCreate(&adc_read, "adc_read", 1536, NULL, 2, NULL);
     xTaskCreate(&adc_print, "adc_print", 1536, NULL, 3, NULL);
     xTaskCreate(&meldeleuchte, "meldeleuchte", 2048, NULL, 2, NULL);
    */
    ///////////////////////////old stuff/////////////////////////////////
}

/*
///////////////////////////old stuff/////////////////////////////////

void adc_read(void *pvParameter)
{

   while (1)
 {
 printf("reading analog data...");

   vTaskDelay(20);
  }
vTaskDelete(NULL);
}



void adc_read(void *pvParameter)
{

    while (1)
    {

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
        adc_wert = adc1_get_raw(ADC1_CHANNEL_0);
        vTaskDelay(20);
   }
    vTaskDelete(NULL);
}


void adc_print(void *pvParameter)
{

    while (1)
  {

       printf(" adc data = %i\n", adc_wert);
       vTaskDelay(20);
   }

   vTaskDelete(NULL);
}

void meldeleuchte(void *pvParameter)
{

    while (1)

    {
        if (adc_wert > 3000)
        {
            gpio_set_direction(GRUEN, GPIO_MODE_OUTPUT);
            gpio_set_direction(ROT, GPIO_MODE_OUTPUT);
            gpio_pad_select_gpio(ROT);
            gpio_set_level(ROT, 0);
            gpio_pad_select_gpio(GRUEN);
            gpio_set_level(GRUEN, 1);
        }

        else
        {
            gpio_set_direction(GRUEN, GPIO_MODE_OUTPUT);
            gpio_set_direction(ROT, GPIO_MODE_OUTPUT);
            gpio_pad_select_gpio(GRUEN);
            gpio_set_level(GRUEN, 0);
            gpio_pad_select_gpio(ROT);
            gpio_set_level(ROT, 1);
        }

        vTaskDelay(20);
    }

    vTaskDelete(NULL);
}
*/
///////////////////////////old stuff/////////////////////////////////