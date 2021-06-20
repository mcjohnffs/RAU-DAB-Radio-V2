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
 * - Red LED connected to pin D2.
 * - Momentary push button connected to pin D3.
 *
 * @section libraries Libraries
 * - Arduino_LSM6DS3 (https://github.com/arduino-libraries/Arduino_LSM6DS3)
 *   - Interacts with on-board IMU.
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
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "touch.h"
#include <Wire.h>
#include <PCA9634.h>
#include <MCP23017.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

// Defines
#define BUFFER_MULTIPLIER 35

TaskHandle_t Task1;

PCA9634 ledDriver(0x15, 4);

MCP23017 mcp = MCP23017(0x20);

// Global LVGL objects
lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;
lv_obj_t *tab3;
lv_obj_t *tab4;
lv_obj_t *gauge1;
lv_obj_t *led1;
lv_obj_t *chart;
lv_chart_series_t *ser1;
lv_chart_series_t *ser2;
lv_chart_series_t *ser3;
lv_chart_series_t *ser4;
lv_chart_series_t *ser5;
lv_chart_series_t *ser6;
lv_chart_series_t *ser7;
lv_chart_series_t *ser8;

// Global Variables
uint8_t conf;
int i;
int x = 0;
int y = 0;

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
/**
 * The standard Arduino setup function used for setup and configuration tasks.
 */
void setup()
{

  Wire.begin();

  Serial.begin(115200);

  xTaskCreatePinnedToCore(
      read_inputs,   /* Task function. */
      "read_inputs", /* String with name of task. */
      20000,         /* Stack size in bytes. */
      NULL,          /* Parameter passed as input of the task */
      1,             /* Priority of the task. */
      &Task1,        /* Task handle. */
      0);            /* Core */

  while (!Serial)
    ;

  mcp.init();

  mcp.portMode(MCP23017Port::A, 0b11111111);

  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  // TFT initialization ===========================
  tft.begin();
  tft.setRotation(3);
  // ===============================================

  // Touch device initialization =================
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
  // ===============================================

  ledDriver.begin();

  ledDriver.allOff();

  lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = tft.width();
  disp_drv.ver_res = tft.height();
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  static lv_color_t needle_colors[3];
  needle_colors[0] = LV_COLOR_BLUE;
  needle_colors[1] = LV_COLOR_ORANGE;
  needle_colors[2] = LV_COLOR_PURPLE;

  // ===============================================

  tabview = lv_tabview_create(lv_scr_act(), NULL);
  lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_BOTTOM);
  //lv_tabview_set_anim_time(tabview, 100);

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  tab1 = lv_tabview_add_tab(tabview, "Main");
  tab2 = lv_tabview_add_tab(tabview, "Volume");
  tab3 = lv_tabview_add_tab(tabview, "Connections");
  tab4 = lv_tabview_add_tab(tabview, "Options");

  /*Add content to the tabs*/
  lv_obj_t *label = lv_label_create(tab1, NULL);
  lv_label_set_text(label, "1 tab");

  led1 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led1, 30, 30);
  lv_obj_align(led1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);

  label = lv_label_create(tab3, NULL);
  lv_label_set_text(label, "3 tab");

  label = lv_label_create(tab4, NULL);
  lv_label_set_text(label, "4 tab");

  /*Create a chart*/
  /*
  chart = lv_chart_create(tab2, NULL);
  lv_obj_set_size(chart, 260, 170);
  lv_obj_align(chart, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE); 
  lv_chart_set_range(chart, 0, 80);

  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_div_line_count(chart, 8, 8);
  lv_chart_set_point_count(chart, 200);
*/
  /*
  lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); //Max. opa.
  lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255); //Max opa on the top
  lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);
  */
  /*
  lv_obj_set_style_local_size(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);

  ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
  ser2 = lv_chart_add_series(chart, LV_COLOR_LIME);
  ser3 = lv_chart_add_series(chart, LV_COLOR_MAROON);
  ser4 = lv_chart_add_series(chart, LV_COLOR_MAGENTA);
  ser5 = lv_chart_add_series(chart, LV_COLOR_BLUE);
  ser6 = lv_chart_add_series(chart, LV_COLOR_BLACK);
  ser7 = lv_chart_add_series(chart, LV_COLOR_PURPLE);
  ser8 = lv_chart_add_series(chart, LV_COLOR_AQUA);
*/


  /*Create a Preloader object*/
  lv_obj_t *preload = lv_spinner_create(tab4, NULL);
  lv_obj_set_size(preload, 100, 100);
  lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);

  gauge1 = lv_gauge_create(tab2, NULL);
  lv_gauge_set_needle_count(gauge1, 3, needle_colors);
  lv_obj_set_size(gauge1, 170, 170);
  lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, 0);

  /*Set the values*/
  lv_gauge_set_value(gauge1, 0, 10);
  lv_gauge_set_value(gauge1, 1, 20);
  lv_gauge_set_value(gauge1, 2, 30);
}

void loop()
{
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  conf = mcp.readRegister(MCP23017Register::GPIO_A);

  lv_task_handler();
  delay(3);
}

void read_inputs(void *parameter)
{

  for (;;)
  {

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
