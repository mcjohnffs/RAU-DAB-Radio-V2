#include <lvgl.h>
#include <TFT_eSPI.h>
#include "touch.h"



#define BUFFER_MULTIPLIER 35

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * BUFFER_MULTIPLIER];
static lv_color_t buf_2[LV_HOR_RES_MAX * BUFFER_MULTIPLIER];
#if USE_LV_LOG != 0
/* Serial debugging */




void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
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


    Serial.begin(115200); /* prepare for possible serial debug */

    lv_init();



#if USE_LV_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    // TFT initialization ===========================
    tft.begin();
    tft.setRotation(1);
    // ===============================================

    // Touch device initialization =================
    if (!touch.begin(150)) {
        Serial.println("Couldn't start FT6206 touchscreen controller");
        while (1);
    } else {
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


lv_obj_t * btn0 = lv_btn_create(lv_scr_act(), NULL);
lv_obj_set_x(btn0, 15);
lv_obj_set_y(btn0, 117);
lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
lv_obj_set_x(btn1, 181);
lv_obj_set_y(btn1, 14);
lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
lv_obj_set_x(btn2, 14);
lv_obj_set_y(btn2, 17);
lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), NULL);
lv_obj_set_x(btn3, 177);
lv_obj_set_y(btn3, 141);
lv_obj_set_width(btn3, 50);
lv_obj_set_height(btn3, 30);



}



void loop()
{

    lv_task_handler(); /* let the GUI do its work */
    delay(5);
}
