/**
 * @file DAB_2_V0.1.ino
 *
 * @mainpage DAB Radio V2.0
 *
 * @section description Übersicht
 * An example sketch demonstrating how to use Doxygen style comments for
 * generating source code documentation with Doxygen.
 *
 * @section circuit Produkt
 * - 1
 * - 2
 *
 * @section libraries Libraries
 * - 2
 *   - 1
 *
 * @section notes Notizen
 * - Comments are Doxygen compatible.
 *
 * @section todo TODO
 * - Don't use Doxygen style formatting inside the body of a function.
 *
 * @section author Author
 * - Created by Egzon Isaku on 16/06/2021.
 * - Modified by 
 *
 * Copyright (c) RAU Regionales Ausbildungszentrum AU.  All rights reserved.
 */

// Libraries
#include <lvgl.h>                   // Light and versatile Embedded Graphics Library (LVGL)
#include <TFT_eSPI.h>               // General TFT library (TFT_eSPI)
#include "touch.h"                  // Communication header for the touch controller between "FT6206" and LVGL
#include <Wire.h>                   // Arduino I2C library
#include <PCA9634.h>                // Led driver library (PCA9634)
#include <MCP23017.h>               // Port expander library (MPC23017)
#include "BM83.h"                   // Bluetooth Module library (BM83/BM64)
#include <SoftwareSerial.h>         // SoftwareSerial Library for ESP32
#include <BD37544FS.h>              // Sound processor library (BD37544FS)
#include "AiEsp32RotaryEncoder.h"   // Encoder Library für ESP32
#include "soc/timer_group_struct.h" // Watchdog timer
#include "soc/timer_group_reg.h"    // Watchdog timer

// Defines
#define BUFFER_MULTIPLIER 35
#define RX_PIN 4
#define TX_PIN 0
#define TX_IND 5
#define DAC1 26
#define ROTARY_ENCODER1_A_PIN 34
#define ROTARY_ENCODER1_B_PIN 35

#define ROTARY_ENCODER2_A_PIN 36
#define ROTARY_ENCODER2_B_PIN 39


const int mfbPin = 23;

// Encoder 1+2 instances ()
AiEsp32RotaryEncoder rotaryEncoder1 = AiEsp32RotaryEncoder(ROTARY_ENCODER1_A_PIN, ROTARY_ENCODER1_B_PIN, -1,-1);
AiEsp32RotaryEncoder rotaryEncoder2 = AiEsp32RotaryEncoder(ROTARY_ENCODER2_A_PIN, ROTARY_ENCODER2_B_PIN, -1,-1);
// Sound processor instance (BD37544FS)
BD37544FS bd;

// Bluetooth module instance (BM83)
SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

// Led driver instance (PCA9634)
PCA9634 ledDriver(0x15, 4);

// MPC 1 + 2 instance (MCP23017)
MCP23017 mcp1 = MCP23017(0x20);
MCP23017 mcp2 = MCP23017(0x24);

// LVGL task handles
TaskHandle_t Task1;
TaskHandle_t Task2;

// Global LVGL object variables
lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;
lv_obj_t *tab3;
lv_obj_t *tab4;
lv_obj_t *gauge1;
lv_obj_t *led1;
lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *btn3;
lv_obj_t *sw1;
lv_obj_t *sw2;
lv_obj_t *sw3;
lv_obj_t *cont;
lv_obj_t *cont2;
lv_obj_t *bar1;
lv_obj_t *lmeter;
lv_obj_t *lmeter2;
lv_obj_t *label;

// Global variables
int16_t encoderValue2;
uint32_t encoderValue;
int8_t encoderDelta;
int8_t encoderDelta2;
uint8_t conf;
int i;
int vol;

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * BUFFER_MULTIPLIER];
static lv_color_t buf_2[LV_HOR_RES_MAX * BUFFER_MULTIPLIER];
#if USE_LV_LOG != 0
/* Serial debugging */

void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{

  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

// The standard Arduino setup function used for setup and configuration tasks.
void setup()
{

  // I2C init @ 100 kHz
  Wire.begin();
  Wire.setClock(100000);

  // Serial debugging @ 115200 baud
  Serial.begin(115200);

  // Software serial for BM83/BM64 Uart connection @ 9600 baud
  swSerial.begin(9600);

  // Input read task creation
  xTaskCreatePinnedToCore(
      read_inputs,   /* Task function. */
      "read_inputs", /* String with name of task. */
      10000,         /* Stack size in bytes. */
      NULL,          /* Parameter passed as input of the task */
      1,             /* Priority of the task. */
      &Task1,        /* Task handle. */
      0);            /* Core 0 or 1 (Core 1 is used for the arduino loop function for now)*/

  // Encoder read task creation
  xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 10000, NULL, 2, &Task2, 0);

  // Sets the MFB pin 4 as output (BM83)
  pinMode(mfbPin, OUTPUT);



  // Encoder 1+2 init
  rotaryEncoder1.begin();
  rotaryEncoder1.setup([]
                       { rotaryEncoder1.readEncoder_ISR(); });
  rotaryEncoder1.setBoundaries(0, 20, false);


  rotaryEncoder2.begin();
  rotaryEncoder2.setup([]
                       { rotaryEncoder2.readEncoder_ISR(); });
  rotaryEncoder2.setBoundaries(0, 20, false);


  // Led driver init
  ledDriver.begin();
  ledDriver.allOff();

  // Charger IC setup
  Wire.beginTransmission(0x6A);
  Wire.write(0x14);
  Wire.write(0xB9);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x02);
  Wire.write(0x30);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x03);
  Wire.write(0x5A);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x04);
  Wire.write(0x40);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x05);
  Wire.write(0x66);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x06);
  Wire.write(0x5A);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x07);
  Wire.write(0x89);
  Wire.endTransmission();

  Wire.beginTransmission(0x6A);
  Wire.write(0x09);
  Wire.write(0x04);
  Wire.endTransmission();

  delay(50);

  //MCP 1 init
  mcp1.init();
  mcp1.portMode(MCP23017Port::A, 0b11111111); // Set all as inputs (MCP1)
  delay(5);

  //MCP 2 init
  mcp2.init();
  mcp2.portMode(MCP23017Port::A, 0); // Set all as outputs (MCP2)
  delay(5);

  // Standby sequence
  mcp2.digitalWrite(1, 0); // 7.5V disable
  delay(10);
  mcp2.digitalWrite(0, 0); // +-5V disable
  delay(10);
  mcp2.digitalWrite(2, 0); // PVCC disable
  delay(10);
  mcp2.digitalWrite(3, 0); // Power Amp shutdown activation
  delay(10);
  mcp2.digitalWrite(4, 1); // Power Amp mute activation

  // Power up sequence
  delay(20);
  mcp2.digitalWrite(1, 1);
  Serial.println("7.5V enabled!"); // 7.5V enable
  delay(10);
  mcp2.digitalWrite(0, 1);
  Serial.println("+-5V enabled!"); // +-5V enable
  delay(10);
  dacWrite(DAC1, 255);
  delay(10);
  mcp2.digitalWrite(2, 1);
  Serial.println("PVCC enabled!"); // PVCC enable
  delay(10);
  mcp2.digitalWrite(3, 1);
  Serial.println("Power Amp Shutdown deactivated!"); // Power amp shutdown deactivation
  delay(10);
  mcp2.digitalWrite(4, 0);
  Serial.println("Power Amp Mute deactivated!"); // Power amp mute deactivation
  delay(10);

  // LVGL init
  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  // TFT init
  tft.begin();
  tft.setRotation(3);

  // Touch device init
  if (!touch.begin(150))
  {
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1)
      ;
  }
  else
  {
    Serial.println("FT6206 touchscreen controller connected!");
  }

  lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 10);

  // Display init
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = tft.width();
  disp_drv.ver_res = tft.height();
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  // Display input driver init
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  static lv_color_t needle_colors[3];
  needle_colors[0] = LV_COLOR_BLUE;
  needle_colors[1] = LV_COLOR_ORANGE;
  needle_colors[2] = LV_COLOR_PURPLE;

  tabview = lv_tabview_create(lv_scr_act(), NULL);
  lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_BOTTOM);
  //lv_tabview_set_anim_time(tabview, 100);

  /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
  tab1 = lv_tabview_add_tab(tabview, "Setups");
  tab2 = lv_tabview_add_tab(tabview, "Encoders");
  tab3 = lv_tabview_add_tab(tabview, "Enables");
  tab4 = lv_tabview_add_tab(tabview, "Options");

  /*Create a Preloader object*/
  lv_obj_t *preload = lv_spinner_create(tab4, NULL);
  lv_obj_set_size(preload, 100, 100);
  lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);

  lmeter = lv_linemeter_create(tab2, NULL);
  lv_linemeter_set_range(lmeter, 0, 20);   /*Set the range*/
  lv_linemeter_set_value(lmeter, 0);       /*Set the current value*/
  lv_linemeter_set_scale(lmeter, 360, 20); /*Set the angle and number of lines*/
  lv_obj_set_size(lmeter, 100, 100);
  lv_obj_align(lmeter, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  lmeter2 = lv_linemeter_create(tab2, NULL);
  lv_linemeter_set_range(lmeter2, 0, 20);   /*Set the range*/
  lv_linemeter_set_value(lmeter2, 0);       /*Set the current value*/
  lv_linemeter_set_scale(lmeter2, 360, 20); /*Set the angle and number of lines*/
  lv_obj_set_size(lmeter2, 100, 100);
  lv_obj_align(lmeter2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  cont = lv_cont_create(tab3, NULL);
  lv_obj_set_auto_realign(cont, true);                   /*Auto realign when the size changes*/
  lv_obj_align_origo(cont, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
  lv_cont_set_fit(cont, LV_FIT_TIGHT);
  lv_cont_set_layout(cont, LV_LAYOUT_GRID);

  sw1 = lv_switch_create(cont, NULL);
  lv_obj_align(sw1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  label = lv_label_create(sw1, NULL);
  lv_label_set_text(label, "7.5V");
  lv_obj_set_event_cb(sw1, event_sw1);

  sw2 = lv_switch_create(cont, NULL);
  lv_obj_align(sw2, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 0);
  label = lv_label_create(sw2, NULL);
  lv_label_set_text(label, "+-5V");
  lv_obj_set_event_cb(sw2, event_sw1);

  sw3 = lv_switch_create(cont, NULL);
  lv_obj_align(sw3, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 0);
  label = lv_label_create(sw3, NULL);
  lv_label_set_text(label, "PVCC");
  lv_obj_set_event_cb(sw3, event_sw1);

  cont2 = lv_cont_create(tab1, NULL);
  lv_obj_set_auto_realign(cont2, true);                   /*Auto realign when the size changes*/
  lv_obj_align_origo(cont2, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
  lv_cont_set_fit(cont2, LV_FIT_TIGHT);
  lv_cont_set_layout(cont2, LV_LAYOUT_GRID);

  btn1 = lv_btn_create(cont2, NULL);
  lv_obj_set_event_cb(btn1, event_bm83setup);
  lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "BM83 Setup");

  btn2 = lv_btn_create(cont2, NULL);
  lv_obj_set_event_cb(btn2, event_bm83pair);
  lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 0);
  label = lv_label_create(btn2, NULL);
  lv_label_set_text(label, "BM83 Pairing");

  btn3 = lv_btn_create(cont2, NULL);
  lv_obj_set_event_cb(btn3, event_soundsetup);
  lv_obj_align(btn3, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 0);
  label = lv_label_create(btn3, NULL);
  lv_label_set_text(label, "Sound Proc. Setup");
}

void loop()
{
  // Watchdog timer fix
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;

  conf = mcp1.readRegister(MCP23017Register::GPIO_A); // MCP1 register read for inputs

  // LVGL task handler
  lv_task_handler();

  delay(3);
}

void read_inputs(void *parameter)
{

  for (;;)
  {
    bm83.run();
    for (i = 7; i >= 0; i--)
    {

      if (bitRead(conf, i) == 1)
      {

        switch (i)
        {
        case 7:

          break;
        case 6:

          break;
        case 5:

          break;
        case 4:
          lv_tabview_set_tab_act(tabview, 3, LV_ANIM_OFF);

          break;
        case 3:
          lv_tabview_set_tab_act(tabview, 2, LV_ANIM_OFF);

          break;
        case 2:
          lv_tabview_set_tab_act(tabview, 1, LV_ANIM_OFF);
          break;
        case 1:
          lv_tabview_set_tab_act(tabview, 0, LV_ANIM_OFF);
          break;
        case 0:
          break;
        }
      }
    }
  }
  vTaskDelay(5);
}

static void event_bm83setup(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    printf("Clicked\n");

    digitalWrite(mfbPin, HIGH); // sets the MFB Pin 4 "High" to power on BM83 over BAT_IN
    delay(500);
    bm83.run();
    bm83.powerOn();            // Sends "power on" command over UART to BM83
    digitalWrite(mfbPin, LOW); // sets the MFB Pin 4 "LOW" (no longer needed after power on process)
  }
}

static void event_bm83pair(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    printf("Clicked\n");
    bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING);
  }
}

static void event_soundsetup(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    printf("Clicked\n");
    bd.setSelect(1);  // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
    bd.setIn_gain(0); // int 0...7 === 0...20 dB
    bd.setVol_1(0);   // int 0...87 === 0...-87 dB
    bd.setFad_1(0);   // int 0...87 === 0...-87 dB
    bd.setFad_2(0);   // int 0...87 === 0...-87 dB
    bd.setBass(0);    // int -7...0...+7 === -14...+14 dB
    bd.setMidd(0);    // int -7...0...+7 === -14...+14 dB
    bd.setTreb(0);    // int -7...0...+7 === -14...+14 dB
  }
}

static void event_sw1(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    printf("State: %s\n", lv_switch_get_state(obj) ? "On" : "Off");
    if (lv_switch_get_state(sw1) == 1)
    {
      mcp2.digitalWrite(1, 1);
    }
    else
    {
      mcp2.digitalWrite(1, 0);
    }
    if (lv_switch_get_state(sw2) == 1)
    {
      mcp2.digitalWrite(0, 1);
    }
    else
    {
      mcp2.digitalWrite(0, 0);
    }
    if (lv_switch_get_state(sw3) == 1)
    {
      mcp2.digitalWrite(2, 1);
    }
    else
    {
      mcp2.digitalWrite(2, 0);
    }
  }
}

void encoder_loop(void *pvParameters)
{

  for (;;)
  {

    //lets see if anything changed
    encoderDelta = rotaryEncoder1.encoderChanged();
    encoderDelta2 = rotaryEncoder2.encoderChanged();

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
    encoderValue = rotaryEncoder1.readEncoder();
    encoderValue2 = rotaryEncoder2.readEncoder();

    //if value is changed compared to our last read
    if (encoderDelta != 0)
    {
      //now we need current value

      //process new value. Here is simple output.
      Serial.print("Value 1:  ");
      Serial.println(encoderValue);

      lv_linemeter_set_value(lmeter, encoderValue);
      vol = encoderValue;
      bd.setVol_1(vol);
      vTaskDelay(2);
    }

    if (encoderDelta2 != 0)
    {

      Serial.print("Value 2: ");
      Serial.println(encoderValue2);
      lv_linemeter_set_value(lmeter2, encoderValue2);
    }

    vTaskDelay(2);
  }
}