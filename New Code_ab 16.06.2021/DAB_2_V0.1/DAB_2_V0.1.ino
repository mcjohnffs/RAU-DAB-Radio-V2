#include <lvgl.h>
#include <TFT_eSPI.h>
#include "touch.h"
#include <Wire.h>
#include <PCA9634.h>
#include "Adafruit_MCP23017.h"

#define BUFFER_MULTIPLIER 35

PCA9634 ledDriver(0x15, 4);

Adafruit_MCP23017 mcp;



lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;
lv_obj_t *tab3;
lv_obj_t *tab4;

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

uint8_t conf;
int i;

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

void setup()
{

  Wire.begin();

  Serial.begin(115200);

  xTaskCreate(
      taskOne,   /* Task function. */
      "TaskOne", /* String with name of task. */
      15000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      1,         /* Priority of the task. */
      NULL);     /* Task handle. */

  xTaskCreate(
      taskTwo,   /* Task function. */
      "TaskTwo", /* String with name of task. */
      15000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      1,         /* Priority of the task. */
      NULL);     /* Task handle. */

  while (!Serial)
    ;

  mcp.begin(0x20);

  

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

mcp.pinMode(1, INPUT);

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

  // ===============================================

  tabview = lv_tabview_create(lv_scr_act(), NULL);
  lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_BOTTOM);

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  tab1 = lv_tabview_add_tab(tabview, "Tab 1");
  tab2 = lv_tabview_add_tab(tabview, "Tab 2");
  tab3 = lv_tabview_add_tab(tabview, "Tab 3");
  tab4 = lv_tabview_add_tab(tabview, "Tab 4");

  /*Add content to the tabs*/
  lv_obj_t *label = lv_label_create(tab1, NULL);
  lv_label_set_text(label, "1 tab");

  led1 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led1, 30, 30);
  lv_obj_align(led1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);
  lv_led_off(led1);

  label = lv_label_create(tab2, NULL);
  lv_label_set_text(label, "\nOut 8\n\nOut 7\n\nOut 6\n\nOut 5\n\nOut 4\n\nOut 3\n\nOut 2\n\nOut 1");

  label = lv_label_create(tab3, NULL);
  lv_label_set_text(label, "3 tab");

  label = lv_label_create(tab4, NULL);
  lv_label_set_text(label, "4 tab");

  /*Create a chart*/

  chart = lv_chart_create(tab2, NULL);
  lv_obj_set_size(chart, 260, 170);
  lv_obj_align(chart, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and poi
  +nts too*/
  lv_chart_set_range(chart, 0, 80);
  /*Add two data series*/
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_div_line_count(chart, 8, 8);
  lv_chart_set_point_count(chart, 200);

  /*
  lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); //Max. opa.
  lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255); //Max opa on the top
  lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);
  */

  lv_obj_set_style_local_size(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);

  ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
  ser2 = lv_chart_add_series(chart, LV_COLOR_LIME);
  ser3 = lv_chart_add_series(chart, LV_COLOR_MAROON);
  ser4 = lv_chart_add_series(chart, LV_COLOR_MAGENTA);
  ser5 = lv_chart_add_series(chart, LV_COLOR_BLUE);
  ser6 = lv_chart_add_series(chart, LV_COLOR_BLACK);
  ser7 = lv_chart_add_series(chart, LV_COLOR_PURPLE);
  ser8 = lv_chart_add_series(chart, LV_COLOR_AQUA);

  lv_obj_t *cpicker;

  cpicker = lv_cpicker_create(tab3, NULL);
  lv_obj_set_size(cpicker, 150, 150);
  lv_obj_align(cpicker, NULL, LV_ALIGN_CENTER, 0, 0);

  /*Create a Preloader object*/
  lv_obj_t *preload = lv_spinner_create(tab4, NULL);
  lv_obj_set_size(preload, 100, 100);
  lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    printf("Clicked\n");
  }
  else if (event == LV_EVENT_VALUE_CHANGED)
  {
    printf("Toggled\n");
  }
}

void loop()
{

  /* let the GUI do its work */
  //read_mcp();
  int x = 0;
  x = mcp.digitalRead(1);
  Serial.println(x);
  
  if (mcp.digitalRead(1) == 0 bh){

    lv_tabview_set_tab_act(tabview, 0, LV_ANIM_ON);
  }
  lv_task_handler();
  
  delay(5);
}

void taskOne(void *parameter)
{

  for (int i = 0; i < 10; i++)
  {

    Serial.println("Hello from task 1");

    
    delay(1000);
  }

  Serial.println("Ending task 1");
  vTaskDelete(NULL);
}

void taskTwo(void *parameter)
{

  for (int i = 0; i < 10; i++)
  {

    Serial.println("Hello from task 2");
    delay(1000);
  }
  Serial.println("Ending task 2");
  vTaskDelete(NULL);
}