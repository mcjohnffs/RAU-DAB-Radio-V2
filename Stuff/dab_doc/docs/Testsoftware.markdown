---
layout: default
title: 3. Testsoftware
nav_order: 4
has_children: true
permalink: /Testsoftware/
---

# Code

[Testsoftware v 1.0 - Github](https://github.com/mcjohnffs/RAU-DAB-Radio-V2/tree/test)

```c
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
#include <lvgl.h>					// Light and versatile Embedded Graphics Library (LVGL)
#include <TFT_eSPI.h>				// General TFT library (TFT_eSPI)
#include "touch.h"					// Communication header for the touch controller - between "FT6206" and LVGL
#include <Wire.h>					// Arduino I2C library
#include <PCA9634.h>				// Led driver library (PCA9634)
#include <MCP23017.h>				// Port expander library (MPC23017)
#include "BM83.h"					// Bluetooth Module library (BM83/BM64)
#include <SoftwareSerial.h>			// SoftwareSerial Library for ESP32
#include <BD37544FS.h>				// Sound processor library (BD37544FS)
#include "ESPRotary.h"				// Encoder Library für ESP32
#include "soc/timer_group_struct.h" // Watchdog timer
#include "soc/timer_group_reg.h"	// Watchdog timer

// Defines
#define BUFFER_MULTIPLIER 35
#define RX_PIN 4
#define TX_PIN 0
#define TX_IND 5
#define DAC1 26

#define ENC1_ROTARY_PIN_A 34
#define ENC1_ROTARY_PIN_B 35

#define ENC2_ROTARY_PIN_A 39
#define ENC2_ROTARY_PIN_B 36

#define CLICKS_PER_STEP 2 // this number depends on your rotary encoder

#define MIN_POS 0
#define MAX_POS 87
#define START_POS 0
#define INCREMENT 1 // this number is the counter increment on each step

const int mfbPin = 23;

ESPRotary r;
ESPRotary u;

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

static lv_group_t *g;			  //An object group
static lv_indev_t *encoder_indev; //Encoder 1 input device

// Global LVGL object variables
lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;
lv_obj_t *tab3;
lv_obj_t *tab4;
lv_obj_t *gauge1;
lv_obj_t *led1;
lv_obj_t *btn;
lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *btn3;
lv_obj_t *sw1;
lv_obj_t *sw2;
lv_obj_t *sw3;
lv_obj_t *cont;
lv_obj_t *cont2;
lv_obj_t *cont3;
lv_obj_t *bar1;
lv_obj_t *lmeter;
lv_obj_t *lmeter2;
lv_obj_t *label;
lv_obj_t *spinbox;
lv_obj_t *slider_label;
lv_obj_t *slider_label2;
lv_obj_t *slider_label3;
lv_obj_t *slider_label4;
lv_obj_t *slider_label5;

// Global variables
uint8_t conf;
int i;
int vol;
int encoderLastValue = 0;

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

// The standard Arduino setup function used for setup and configuration
void setup()
{

	// I2C init @ 100 kHz
	Wire.begin();
	Wire.setClock(100000);

	// Serial debugging @ 115200 baud
	Serial.begin(115200);

	// Software serial for BM83/BM64 Uart communication @ 9600 baud
	swSerial.begin(9600);

	// Input read task creation
	xTaskCreatePinnedToCore(
		read_inputs,   /* Task function. */
		"read_inputs", /* String with name of task. */
		10000,		   /* Stack size in bytes. */
		NULL,		   /* Parameter passed as input of the task */
		1,			   /* Priority of the task. */
		&Task1,		   /* Task handle. */
		0);			   /* Core 0 or 1 (Core 1 is used for the arduino loop function for now)*/

	// Encoder read task creation
	//xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 10000, NULL, 2, &Task2, 0);

	// Sets the MFB pin (GPIO23) as output (BM83)
	pinMode(mfbPin, OUTPUT);

	// Encoder 1+2 init

	// Led driver init
	ledDriver.begin();
	// all on (inverted)
	ledDriver.allOff();

	// Charger IC setup----------------------------------------------------------------
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
	// --------------------------------------------------------------------

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
	/* Serial debugging */
	void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
	{

		Serial.printf("%s@%d->%s\r\n", file, line, dsc);
		Serial.flush();
	}
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

	lv_indev_drv_t enc_indev;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_ENCODER;
	indev_drv.read_cb = encoder_read;
	encoder_indev = lv_indev_drv_register(&indev_drv);

	static lv_style_t style1;
	lv_style_init(&style1);
	lv_style_set_border_color(&style1, LV_STATE_FOCUSED, LV_COLOR_RED);

	//Create Group for encoder 1
	g = lv_group_create();
	lv_indev_set_group(encoder_indev, g);

	static lv_color_t needle_colors[3];
	needle_colors[0] = LV_COLOR_BLUE;
	needle_colors[1] = LV_COLOR_ORANGE;
	needle_colors[2] = LV_COLOR_PURPLE;

	tabview = lv_tabview_create(lv_scr_act(), NULL);
	lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_BOTTOM);
	//lv_tabview_set_anim_time(tabview, 100);

	/*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
	tab1 = lv_tabview_add_tab(tabview, "Setups");
	tab2 = lv_tabview_add_tab(tabview, "Values");
	tab3 = lv_tabview_add_tab(tabview, "Enables");
	tab4 = lv_tabview_add_tab(tabview, "Parameters");

	lmeter = lv_linemeter_create(tab2, NULL);
	lv_linemeter_set_range(lmeter, 0, 20);	 /*Set the range*/
	lv_linemeter_set_value(lmeter, 0);		 /*Set the current value*/
	lv_linemeter_set_scale(lmeter, 360, 20); /*Set the angle and number of lines*/
	lv_obj_set_size(lmeter, 100, 100);
	lv_obj_align(lmeter, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

	lmeter2 = lv_linemeter_create(tab2, NULL);
	lv_linemeter_set_range(lmeter2, 0, 20);	  /*Set the range*/
	lv_linemeter_set_value(lmeter2, 0);		  /*Set the current value*/
	lv_linemeter_set_scale(lmeter2, 360, 20); /*Set the angle and number of lines*/
	lv_obj_set_size(lmeter2, 100, 100);
	lv_obj_align(lmeter2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

	cont = lv_cont_create(tab3, NULL);
	lv_obj_set_auto_realign(cont, true);				   /*Auto realign when the size changes*/
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
	lv_obj_set_auto_realign(cont2, true);					/*Auto realign when the size changes*/
	lv_obj_align_origo(cont2, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont2, LV_FIT_TIGHT);
	lv_cont_set_layout(cont2, LV_LAYOUT_GRID);

	btn1 = lv_btn_create(cont2, NULL);
	lv_obj_set_event_cb(btn1, event_bm83setup);
	lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	label = lv_label_create(btn1, NULL);
	lv_label_set_text(label, "BM83 Setup");
	lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style1);

	btn2 = lv_btn_create(cont2, NULL);
	lv_obj_set_event_cb(btn2, event_bm83pair);
	lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 0);
	label = lv_label_create(btn2, NULL);
	lv_label_set_text(label, "BM83 Pairing");
	lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &style1);

	btn3 = lv_btn_create(cont2, NULL);
	lv_obj_set_event_cb(btn3, event_soundsetup);
	lv_obj_align(btn3, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 0);
	label = lv_label_create(btn3, NULL);
	lv_label_set_text(label, "Sound Proc. Setup");
	lv_obj_add_style(btn3, LV_BTN_PART_MAIN, &style1);

	/* Create a slider in the center of the display */
	lv_obj_t *slider = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider, LV_DPI * 1);
	lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 25);
	lv_obj_set_event_cb(slider, slider_event_cb_ingain);
	lv_slider_set_range(slider, 5, 20);

	/* Create a label below the slider */
	slider_label = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label, "0");
	lv_obj_set_auto_realign(slider_label, true);
	lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	lv_obj_t *info = lv_label_create(tab4, NULL);
	lv_label_set_text(info, "Input Gain (0-20dB)");
	lv_obj_align(info, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 5);

	//______________________________________________________________

	lv_obj_t *slider2 = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider2, LV_DPI * 1);
	lv_obj_align(slider2, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 60);
	lv_obj_set_event_cb(slider2, slider_event_cb_fade1);
	lv_slider_set_range(slider2, 0, 87);

	/* Create a label below the slider */
	slider_label2 = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label2, "0");
	lv_obj_set_auto_realign(slider_label2, true);
	lv_obj_align(slider_label2, slider2, LV_ALIGN_OUT_BOTTOM_MID, 5, 20);

	/* Create an informative label */
	lv_obj_t *info2 = lv_label_create(tab4, NULL);
	lv_label_set_text(info2, "Fader 1 (0-(-87dB))");
	lv_obj_align(info2, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 25);

	lv_group_add_obj(g, tabview);
	lv_group_add_obj(g, sw1);
	lv_group_add_obj(g, sw2);
	lv_group_add_obj(g, sw3);
	lv_group_add_obj(g, btn1);
	lv_group_add_obj(g, btn2);
	lv_group_add_obj(g, btn3);

	lv_group_set_wrap(g, true);
	lv_group_set_editing(g, true);

	r.begin(ENC1_ROTARY_PIN_A, ENC1_ROTARY_PIN_B, CLICKS_PER_STEP);
	r.setChangedHandler(rotate_r);
	r.setLeftRotationHandler(showDirection_r);
	r.setRightRotationHandler(showDirection_r);

	u.begin(ENC2_ROTARY_PIN_A, ENC2_ROTARY_PIN_B, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
	u.setChangedHandler(rotate_u);
	u.setLeftRotationHandler(showDirection_u);
	u.setRightRotationHandler(showDirection_u);
}

void loop()
{
	// Watchdog timer manip
	TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	TIMERG0.wdt_feed = 1;
	TIMERG0.wdt_wprotect = 0;

	conf = mcp1.readRegister(MCP23017Register::GPIO_A); // MCP1 register read for inputs
	r.loop();
	u.loop();

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

		digitalWrite(mfbPin, HIGH); // sets the MFB Pin 23 "High" to power on BM83 over BAT_IN
		delay(500);
		bm83.run();
		bm83.powerOn();			   // Sends "power on" command over UART to BM83
		digitalWrite(mfbPin, LOW); // sets the MFB Pin 23 "LOW" (no longer needed after power on process)
	}
}

static void event_bm83pair(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING);
	}
}

static void event_soundsetup(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		bd.setSelect(1);  // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
		bd.setIn_gain(0); // int 0...7 === 0...20 dB
		bd.setVol_1(0);	  // int 0...87 === 0...-87 dB
		bd.setFad_1(0);	  // int 0...87 === 0...-87 dB
		bd.setFad_2(0);	  // int 0...87 === 0...-87 dB
		bd.setBass(0);	  // int -7...0...+7 === -14...+14 dB
		bd.setMidd(0);	  // int -7...0...+7 === -14...+14 dB
		bd.setTreb(0);	  // int -7...0...+7 === -14...+14 dB
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

void rotate_r(ESPRotary &r)
{
	//Serial.println(r.getPosition());
}

// on left or right rotattion
void showDirection_r(ESPRotary &r)
{
	//Serial.println(r.directionToString(r.getDirection()));
}

// on change
void rotate_u(ESPRotary &u)
{
	Serial.println(u.getPosition());
	int k = u.getPosition();
	bd.setVol_1(k); // int 0...87 === 0...-87 dB
	Serial.print("Vol:");
	Serial.println(k);
	delay(2);
}

void showDirection_u(ESPRotary &u)
{
	//Serial.println(u.directionToString(u.getDirection()));
}

bool encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{

	data->enc_diff = enc_get_new_moves();

	if (mcp1.digitalRead(5) == 1)
		data->state = LV_INDEV_STATE_PR;
	else
		data->state = LV_INDEV_STATE_REL;
	return false; /*No buffering now so no more data read*/
}

int enc_get_new_moves()
{
	int encoderCount = r.getPosition();
	int diff = encoderCount - encoderLastValue;
	encoderLastValue = encoderCount;
	Serial.print("Diff:");
	Serial.println(diff);
	return diff;
}

static void slider_event_cb_ingain(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setIn_gain(lv_slider_get_value(slider));
	}
}

static void slider_event_cb_fade1(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setFad_1(0);
		(lv_slider_get_value(slider));
	}
}

static void slider_event_fade2(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setIn_gain(lv_slider_get_value(slider));
	}
}

static void slider_event_cb_bass(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setIn_gain(lv_slider_get_value(slider));
	}
}

static void slider_event_mid(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setIn_gain(lv_slider_get_value(slider));
	}
}

static void slider_event_treb(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
		snprintf(buf, 4, "%u", lv_slider_get_value(slider));
		lv_label_set_text(slider_label, buf);
		bd.setIn_gain(lv_slider_get_value(slider));
	}
}
```
