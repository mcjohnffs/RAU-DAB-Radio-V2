/* MQTT (over TCP) Example

	Schickt en  "ping" string via button press -> task mqtt_send
	Verarbeitet "ping" und schickt "pong" zrug -> task mqtt_receive
	
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "tcpip_adapter.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "driver/gpio.h"

#define BUTTON_GPIO GPIO_NUM_4

static const char *TAG = "MQTT_EXAMPLE";

char datastring[100];
char topicstring[100];

char null_data[100] = "NULL";
char current_data[100];

esp_mqtt_client_handle_t client; //mqtt handle variable global setzen

TaskHandle_t xHandle = NULL;

void mqtt_receive(void *pvParameter)
{

    while (1)
    {
        sprintf(current_data, "%s", datastring);

        if (strcmp("ping", current_data) == 0)
        {
            printf("ping received\n");

            esp_mqtt_client_publish(client, "/data/michi/receive", "pong", 0, 0, 0);
            sprintf(datastring,"%s", null_data);
            
            printf("pong sent back\n");
        }

        printf("no ping received...waiting\n");

        printf("current data = %s \n\n", current_data);
        
        vTaskDelay(500);
    }

    vTaskDelete(xHandle);
}

void mqtt_send(void *pvParameter)
{
    int msg_id;
    bool BUTTON_state = 0;
    bool BUTTON_state_last = 0;

    while (1)
    {
        BUTTON_state = gpio_get_level(BUTTON_GPIO);
        if (BUTTON_state > BUTTON_state_last)
        {
            msg_id = esp_mqtt_client_publish(client, "/data/michi/receive", "PING", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        }
        BUTTON_state_last = BUTTON_state;
        vTaskDelay(100);
    }
    vTaskDelete(xHandle);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{

    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:

        ESP_LOGI(TAG, "connection successful");
        msg_id = esp_mqtt_client_subscribe(client, "/data/erika/receive", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");

        sprintf(topicstring, "%.*s", event->topic_len, event->topic);
        sprintf(datastring, "%.*s", event->data_len, event->data);

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
        .username = "mqtt",
        .password = "Es1s?koko",
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void app_main()
{
    //Configure button
    gpio_config_t btn_config;
    btn_config.intr_type = GPIO_INTR_ANYEDGE;       //Enable interrupt on both rising and falling edges
    btn_config.mode = GPIO_MODE_INPUT;              //Set as Input
    btn_config.pin_bit_mask = (1 << BUTTON_GPIO);   //Bitmask
    btn_config.pull_up_en = GPIO_PULLUP_DISABLE;    //Disable pullup
    btn_config.pull_down_en = GPIO_PULLDOWN_ENABLE; //Enable pulldown
    gpio_config(&btn_config);
    printf("Button configured\n");

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    xTaskCreate(&mqtt_send, "mqtt_send", 2048, NULL, 1, &xHandle);
    xTaskCreate(&mqtt_receive, "mqtt_receive", 2048, NULL, 1, &xHandle);

    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}
