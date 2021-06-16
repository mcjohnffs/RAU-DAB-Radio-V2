#include <lvgl.h>
#include <TFT_eSPI.h>
#include "touch.h"
#include <Wire.h>
//#include <MCP23017.h>

#define BUFFER_MULTIPLIER 35

//MCP23017 mcp = MCP23017(0x20);

lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;
lv_obj_t *tab3;
lv_obj_t *tab4;
lv_obj_t *tab5;

lv_obj_t *led1;
lv_obj_t *led2;
lv_obj_t *led3;
lv_obj_t *led4;
lv_obj_t *led5;
lv_obj_t *led6;
lv_obj_t *led7;
lv_obj_t *led8;

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
  Wire.setClock(100000);

  Serial.begin(115200);
  while (!Serial)
    ;
  // Serial.println("\nI2C Scanner");
  delay(10000);
  //mcp.init();


  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  // TFT initialization ===========================
  tft.begin();
  tft.setRotation(1);
  // ===============================================

  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);

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

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  tab1 = lv_tabview_add_tab(tabview, "Tab 1");
  tab2 = lv_tabview_add_tab(tabview, "Tab 2");
  tab3 = lv_tabview_add_tab(tabview, "Tab 3");
  tab4 = lv_tabview_add_tab(tabview, "Tab 4");
  tab5 = lv_tabview_add_tab(tabview, "Tab 5");

  /*Add content to the tabs*/
  lv_obj_t *label = lv_label_create(tab1, NULL);
  lv_label_set_text(label, "1 tab");

  led1 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led1, 30, 30);
  lv_obj_align(led1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);
  lv_led_off(led1);

  led2 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led2, 30, 30);
  lv_obj_align(led2, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 40);
  lv_led_off(led2);

  led3 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led3, 30, 30);
  lv_obj_align(led3, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 40);
  lv_led_off(led3);

  led4 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led4, 30, 30);
  lv_obj_align(led4, NULL, LV_ALIGN_IN_TOP_LEFT, 120, 40);
  lv_led_off(led4);

  led5 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led5, 30, 30);
  lv_obj_align(led5, NULL, LV_ALIGN_IN_TOP_LEFT, 160, 40);
  lv_led_off(led5);

  led6 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led6, 30, 30);
  lv_obj_align(led6, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 40);
  lv_led_off(led6);

  led7 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led7, 30, 30);
  lv_obj_align(led7, NULL, LV_ALIGN_IN_TOP_LEFT, 240, 40);
  lv_led_off(led7);

  led8 = lv_led_create(tab1, NULL);
  lv_obj_set_size(led8, 30, 30);
  lv_obj_align(led8, NULL, LV_ALIGN_IN_TOP_LEFT, 280, 40);
  lv_led_off(led8);

  label = lv_label_create(tab2, NULL);
  lv_label_set_text(label, "\nOut 8\n\nOut 7\n\nOut 6\n\nOut 5\n\nOut 4\n\nOut 3\n\nOut 2\n\nOut 1");

  label = lv_label_create(tab3, NULL);
  lv_label_set_text(label, "3 tab");

  label = lv_label_create(tab4, NULL);
  lv_label_set_text(label, "4 tab");

  label = lv_label_create(tab5, NULL);
  lv_label_set_text(label, "5 tab");

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
/*
void i2c_scanner()
{

  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(1000); // wait 5 seconds for next scan
}
*/

/*
void read_mcp()
{
  
  uint32_t timeout=millis();
  while(Wire.busy()&&(millis()-timeout<1000));
  Serial.println("Reading...");

  delay(500);
  //conf = mcp.readRegister(MCP23017Register::GPIO_A);

  //conf = ~conf;

  for (i = 7; i >= 0; i--)
  {

    if (bitRead(conf, i) == 1)
    {

      switch (i)
      {

      case 7:
        lv_led_on(led1);
        lv_chart_set_next(chart, ser1, 5);
        lv_chart_refresh(chart); 
        break;
      case 6:
        lv_led_on(led2);
        lv_chart_set_next(chart, ser2, 15);
        lv_chart_refresh(chart); 
        break;
      case 5:
        lv_led_on(led3);
        lv_chart_set_next(chart, ser3, 25);
        lv_chart_refresh(chart); 
        break;
      case 4:
        lv_led_on(led4);
        lv_chart_set_next(chart, ser4, 35);
        lv_chart_refresh(chart); 
        break;
      case 3:
        lv_led_on(led5);
        lv_chart_set_next(chart, ser5, 45);
        lv_chart_refresh(chart); 
        break;
      case 2:
        lv_led_on(led6);
        lv_chart_set_next(chart, ser6, 55);
        lv_chart_refresh(chart); 
        break;
      case 1:
        lv_led_on(led7);
        lv_chart_set_next(chart, ser7, 65);
        lv_chart_refresh(chart); 
        break;
      case 0:
        lv_led_on(led8);
        lv_chart_set_next(chart, ser8, 75);
        lv_chart_refresh(chart); 
        break;
      default:

        break;
      }
    }
    else if (bitRead(conf, i) == 0)
    {

      switch (i)
      {

      case 7:
        lv_led_off(led1);
        lv_chart_set_next(chart, ser1, 0);
        lv_chart_refresh(chart); 
        break;
      case 6:
        lv_led_off(led2);
        lv_chart_set_next(chart, ser2, 10);
        lv_chart_refresh(chart); 
        break;
      case 5:
        lv_led_off(led3);
        lv_chart_set_next(chart, ser3, 20);
        lv_chart_refresh(chart); 
        break;
      case 4:
        lv_led_off(led4);
        lv_chart_set_next(chart, ser4, 30);
        lv_chart_refresh(chart); 
        break;
      case 3:
        lv_led_off(led5);
        lv_chart_set_next(chart, ser5, 40);
        lv_chart_refresh(chart); 
        break;
      case 2:
        lv_led_off(led6);
        lv_chart_set_next(chart, ser6, 50);
        lv_chart_refresh(chart); 
        break;
      case 1:
        lv_led_off(led7);
        lv_chart_set_next(chart, ser7, 60);
        lv_chart_refresh(chart); 
        break;
      case 0:
        lv_led_off(led8);
        lv_chart_set_next(chart, ser8, 70);
        lv_chart_refresh(chart); 
        break;
      default:

        break;
        // Statement(s)
      }
    }
  }
}

*/

void loop()
{

  /* let the GUI do its work */
  //read_mcp();
  lv_task_handler();
  delay(5);
}
