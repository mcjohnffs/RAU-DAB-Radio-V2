#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "BM83.h"                 //Bluetooth Module BM83 Library
#include <SoftwareSerial.h>       //Funktionierende SoftwareSerial Library für ESP32
#include <APA102.h>               //RGB LED's Library
#include <BD37544FS.h>            //Sound Prozessor Library
#include "Wire.h"                 //I2C Standart library
#include <SparkFunBQ27441.h>      //Fuel Gauge library
#include <DFRobot_MCP23017.h>     // MCP23017 GPIO Port Expander Library
#include "AiEsp32RotaryEncoder.h" //Encoder Library für ESP32

/* Littlevgl specific */
#include <lvgl_main.h>

/*********************
 *      DEFINES
 *********************/
#define TAG "demo"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void IRAM_ATTR lv_tick_task(void *arg);
void guiTask();

BD37544FS bd;                     // Sound Processor Haupt-Funktion Initialisierung
DFRobot_MCP23017 mcp(Wire, 0x20); // MCP23017 Port Expander Initialisierung

// FreeRTOS Handles
SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xGuiSemaphore;
TaskHandle_t xHandle;

//Encoder 1+2 Pins und Initialisierung
#define ROTARY_ENCODER1_A_PIN 39
#define ROTARY_ENCODER1_B_PIN 36
#define ROTARY_ENCODER1_BUTTON_PIN 34

#define ROTARY_ENCODER2_A_PIN 32
#define ROTARY_ENCODER2_B_PIN 35
#define ROTARY_ENCODER2_BUTTON_PIN 33

AiEsp32RotaryEncoder rotaryEncoder1 = AiEsp32RotaryEncoder(ROTARY_ENCODER1_A_PIN, ROTARY_ENCODER1_B_PIN, ROTARY_ENCODER1_BUTTON_PIN, -1);
AiEsp32RotaryEncoder rotaryEncoder2 = AiEsp32RotaryEncoder(ROTARY_ENCODER2_A_PIN, ROTARY_ENCODER2_B_PIN, ROTARY_ENCODER2_BUTTON_PIN, -1);

// Definieren der gesamten Kapazität des Akkus/der Batterie mithilfe "BATTERY_CAPACITY" in mAh
const unsigned int BATTERY_CAPACITY = 9000; // e.g. 850mAh battery

//BM83 Uart Pins
#define RX_PIN 16 //SWSerial RX_PIN
#define TX_PIN 17 //SWSerial TX_PIN
#define TX_IND 5  //SWSerial TX_IND

const int mfbPin = 4; //BM83 "Power On" Pin

int fuelgauge_init = 0; // Fuel Gauge Init Variable

// BM83 Uart SoftwareSerial Initialisierung
SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

#define led2Pin 18    // S2 LED Pin
int buttonState2 = 0; // S2 Button Pin Status Variable
#define led3Pin 19    // S3 LED Pin
int buttonState3 = 0; // S3 Button Pin Status Variable
#define led4Pin 26    // S4 LED Pin
int buttonState4 = 0; // S4 Button Pin Status Variable
#define led5Pin 27    // S5 LED Pin
int buttonState5 = 0; // S5 Button Pin Status Variable

uint32_t *newencodervalue1 = 0;

lv_obj_t *bar1;
lv_obj_t *lmeter;
lv_obj_t *sw1;
lv_obj_t *sw2;
lv_obj_t *list1;
lv_obj_t *list_btn;


void guiTask(void *pvParameters)
{
  xGuiSemaphore = xSemaphoreCreateMutex();

  Serial.println("INIT");
  lv_init();

  lvgl_driver_init();

  static lv_color_t buf1[DISP_BUF_SIZE];
  static lv_color_t buf2[DISP_BUF_SIZE];
  static lv_disp_buf_t disp_buf;
  lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = disp_driver_flush;

  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

#if CONFIG_LVGL_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.read_cb = touch_driver_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
#endif
  esp_timer_create_args_t periodic_timer_args;
  periodic_timer_args.callback = &lv_tick_task;
  periodic_timer_args.name = "periodic_gui";

  esp_timer_handle_t periodic_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
  //On ESP32 it's better to create a periodic task instead of esp_register_freertos_tick_hook
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1 * 1000)); //1ms (expressed as microseconds)

  lmeter = lv_linemeter_create(lv_scr_act(), NULL);
  lv_linemeter_set_range(lmeter, 0, 20);   /*Set the range*/
  lv_linemeter_set_value(lmeter, 0);       /*Set the current value*/
  lv_linemeter_set_scale(lmeter, 360, 20); /*Set the angle and number of lines*/
  lv_obj_set_size(lmeter, 100, 100);
  lv_obj_align(lmeter, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  bar1 = lv_bar_create(lv_scr_act(), NULL);
  lv_obj_set_size(bar1, 100, 10);
  lv_obj_align(bar1, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_bar_set_anim_time(bar1, 2000);
  lv_bar_set_range(bar1, 0, 20);
  lv_bar_set_value(bar1, 0, LV_ANIM_ON);

  /*Create a switch and apply the styles*/
  sw1 = lv_switch_create(lv_scr_act(), NULL);
  lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, 0, -50);
  lv_obj_set_event_cb(sw1, event_handler);

  /*Copy the first switch and turn it ON*/
  sw2 = lv_switch_create(lv_scr_act(), sw1);
  lv_switch_on(sw2, LV_ANIM_ON);
  lv_obj_align(sw2, NULL, LV_ALIGN_CENTER, 0, 50);

    /*Create a list*/
    list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add buttons to the list*/
    

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_obj_set_event_cb(list_btn, event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_set_event_cb(list_btn, event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_set_event_cb(list_btn, event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
    lv_obj_set_event_cb(list_btn, event_handler);

  while (1)
  {
    vTaskDelay(1);
    //Try to lock the semaphore, if success, call lvgl stuff
    if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE)
    {
      lv_task_handler();
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  //A task should NEVER return
  vTaskDelete(NULL);
}

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    printf("Clicked: %s\n", lv_list_get_btn_text(obj));
  }
}

void bm83_loop(void *pvParameters)
{
  while (1)
  {

    bm83.run();

    vTaskDelete(NULL);
  }
}

void mcp23017_buttons(void *pvParameters)
{
  while (1)
  {

    uint8_t value2 = mcp.digitalRead(/*pin = */ mcp.eGPA2);
    uint8_t value3 = mcp.digitalRead(/*pin = */ mcp.eGPA3);
    uint8_t value4 = mcp.digitalRead(/*pin = */ mcp.eGPA4);
    uint8_t value5 = mcp.digitalRead(/*pin = */ mcp.eGPA5);

    if (value2 == LOW)
    {
      Serial.println("Button 2 pressed. Configuring Sound Processor!");

      bd.setSelect(0);  // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
      bd.setIn_gain(0); // int 0...7 === 0...20 dB
      bd.setVol_1(0);   // int 0...87 === 0...-87 dB
      bd.setFad_1(0);   // int 0...87 === 0...-87 dB
      bd.setFad_2(0);   // int 0...87 === 0...-87 dB
      bd.setBass(0);    // int -7...0...+7 === -14...+14 dB
      bd.setMidd(0);    // int -7...0...+7 === -14...+14 dB
      bd.setTreb(0);    // int -7...0...+7 === -14...+14 dB
      Serial.println("Setup p complete!");

      ledcWrite(0, 1024);
      delay(3);
      ledcWrite(0, 0);
      delay(3);
      ledcWrite(0, 1024);
      delay(3);
      ledcWrite(0, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }

    if (value3 == LOW)
    {
      Serial.println("Button 3 pressed!");

      ledcWrite(1, 1024);
      delay(3);
      ledcWrite(1, 0);
      delay(3);
      ledcWrite(1, 1024);
      delay(3);
      ledcWrite(1, 0);
      delay(200);
    }

    else
    {
      //Serial.println("Button release!");
    }

    if (value4 == LOW)
    {
      Serial.println("Button 4 pressed!");

      ledcWrite(2, 1024);
      delay(3);
      ledcWrite(2, 0);
      delay(3);
      ledcWrite(2, 1024);
      delay(3);
      ledcWrite(2, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }

    if (value5 == LOW)
    {
      Serial.println("Button 5 pressed!");

      ledcWrite(3, 1024);
      delay(3);
      ledcWrite(3, 0);
      delay(3);
      ledcWrite(3, 1024);
      delay(3);
      ledcWrite(3, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }
    vTaskDelay(50);
  }
}

void encoder_loop(void *pvParameters)
{

  while (1)
  {
    //first lets handle rotary encoder button click
    if (rotaryEncoder1.currentButtonState() == BUT_RELEASED || rotaryEncoder2.currentButtonState() == BUT_RELEASED)
    {
      //we can process it here or call separate function like:
    }

    //lets see if anything changed
    int8_t encoderDelta = rotaryEncoder1.encoderChanged();
    int8_t encoderDelta2 = rotaryEncoder2.encoderChanged();

    //optionally we can ignore whenever there is no change
    //if (encoderDelta == 0) return;

    //for some cases we only want to know if value is
    //increased or decreased (typically for menu items)
    if (encoderDelta > 0 || encoderDelta2 > 0)
    {

      Serial.print("+");
    }
    if (encoderDelta < 0 || encoderDelta2 < 0)
    {

      Serial.print("-");

      //for other cases we want to know what is current value.
      //Additionally often we only want if something changed
      //example: when using rotary encoder to set termostat temperature, or sound volume etc
    }
    uint32_t encoderValue = rotaryEncoder1.readEncoder();
    int16_t encoderValue2 = rotaryEncoder2.readEncoder();

    //if value is changed compared to our last read
    if (encoderDelta != 0)
    {
      //now we need current value

      //process new value. Here is simple output.
      Serial.print("Value 1:  ");
      Serial.println(encoderValue);

      lv_linemeter_set_value(lmeter, encoderValue);

      bd.setVol_1(encoderValue);
    }

    if (encoderDelta2 != 0)
    {

      Serial.print("Value 2: ");
      Serial.println(encoderValue2);
      lv_bar_set_value(bar1, encoderValue2, LV_ANIM_ON);
    }

    vTaskDelay(2);
  }
}
void loop()
{
  vTaskDelay(100);
}

static void IRAM_ATTR lv_tick_task(void *arg)
{
  (void)arg;

  lv_tick_inc(portTICK_RATE_MS); //Take note it is 1ms
}

void read_print_fuelgauge(void *pvParameters)
{
  while (1)
  {
    if (fuelgauge_init == 1)
    {

      xSemaphoreTake(xSemaphore, 30);
      Serial.println("read_print_fuelgauge: Using Semaphore to read Charger Board Values....");

      // Read battery stats from the BQ27441-G1A
      unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
      unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
      int current = lipo.current(AVG);                 // Read average current (mA)
      unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
      unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
      int power = lipo.power();                        // Read average power draw (mW)
      int health = lipo.soh();                         // Read state-of-health (%)

      // Now print out those values:
      String toPrint = String(soc) + "% | ";
      toPrint += String(volts) + " mV | ";
      toPrint += String(current) + " mA | ";
      toPrint += String(capacity) + " / ";
      toPrint += String(fullCapacity) + " mAh | ";
      toPrint += String(power) + " mW | ";
      toPrint += String(health) + "%";

      Serial.println(toPrint);
      xSemaphoreGive(xSemaphore);
      Serial.println("read_print_fuelgauge: Reading complete...returning Semaphore");
    }

    vTaskDelay(5000);
  }
}
void bm83_setup(void *pvParameters)
{
  xSemaphoreTake(xSemaphore, 30);
  Serial.println("bm83_setup: Using Semaphore");

  Serial.println("Setting up BM83");
  //BM83 start routine (required)1
  pinMode(mfbPin, OUTPUT);    // sets the MFB Pin 4 as output
  digitalWrite(mfbPin, HIGH); // sets the MFB Pin 4 "High" to power on BM83 over BAT_IN
  delay(10);
  //bm83.powerOn(); // Sends "power on" command over UART to BM83

  digitalWrite(mfbPin, LOW); // sets the MFB Pin 4 "LOW" (no longer needed after power on process)
  Serial.println("bm83_setup: BM83 Setup complete...");
  xSemaphoreGive(xSemaphore);
  Serial.println("bm83_setup: Returning Semaphore...");
  vTaskDelete(NULL);
}

void sound_proc_setup(void *pvParameters)
{
  while (1)
  {

    if (buttonState5 == HIGH)
    {
      xSemaphoreTake(xSemaphore, 30);
      Serial.println("sound_proc_setup: Using Semaphore");

      xSemaphoreGive(xSemaphore);
      Serial.println("sound_proc_setup: Sound p Setup complete...returning Semaphore");
    }
    vTaskDelete(NULL);
  }
}

void setup()
{
  Wire.begin(21, 22, 400000);
  swSerial.begin(115200);

  Serial.begin(9600);
  Serial.println("running setup");

  // possible use of "CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE"
  // xTaskCreatePinnedToCore Core Auswahl

  xSemaphore = xSemaphoreCreateBinary();

  // Setup Tasks

  xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 4, NULL, 4, NULL, 1);
  xTaskCreatePinnedToCore(bm83_setup, "bm83_setup", 4000, NULL, 3, &xHandle, 0);
  xTaskCreatePinnedToCore(sound_proc_setup, "sound_proc_setup", 2048, NULL, 3, &xHandle, 0);

  //Loop Tasks
  xTaskCreatePinnedToCore(bm83_loop, "bm83_loop", 4000, NULL, 2, &xHandle, 1);
  xTaskCreatePinnedToCore(read_print_fuelgauge, "read_print_fuelgauge", 2048, NULL, 2, &xHandle, 1);
  xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 4096, NULL, 1, &xHandle, 1);
  xTaskCreatePinnedToCore(mcp23017_buttons, "mcp23017_buttons", 4096, NULL, 3, &xHandle, 1);

  rotaryEncoder1.begin();
  rotaryEncoder1.setup([] { rotaryEncoder1.readEncoder_ISR(); });
  rotaryEncoder1.setBoundaries(0, 20, false);

  rotaryEncoder2.begin();
  rotaryEncoder2.setup([] { rotaryEncoder2.readEncoder_ISR(); });
  rotaryEncoder2.setBoundaries(0, 20, false);

  // initialize the pushbuttons and leds as inputs | outputs:

  while (mcp.begin() != 0)
  {
    Serial.println("Fehler: Initialisierung des Port Expanders(MCP23017) fehlgeschlagen, bitte ueberpruefen Sie die Verbindungen zum IC!");
  }

  mcp.pinMode(/*pin = */ mcp.eGPA2, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA3, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA4, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA5, /*mode = */ INPUT);

  // Konfiguration der PWM Channels
  for (int i = 0; i < 4; i++)
  {
    ledcSetup(i, 2000, 8); //2 kHz , 8 bit
  }
  ledcAttachPin(led2Pin, 0);
  ledcAttachPin(led3Pin, 1);
  ledcAttachPin(led4Pin, 2);
  ledcAttachPin(led5Pin, 3);

  ledcWrite(0, 0); // 0 = LED On ; 1024 = LED Off
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);

  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
    //  If communication fails, print an error message and loop forever.
    Serial.println("Fehler: Initialisierung des Ladechips(BQ27441-G1A) fehlgeschlagen, bitte ueberpruefen Sie die Verbindungen zum IC!");
    while (1)
      ;
  }
  Serial.println("Verbindung zum Ladechip(BQ27441-G1A) erfolgreich");
  int fuelgauge_init = 1;

  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}
