

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

#define BUFFER_MULTIPLIER 35 //!< LVGL buffer multiplier

#define RX_PIN 4 //!< BM83 UART RX
#define TX_PIN 0 //!< BM83 UART TX
#define TX_IND 5 //!< BM83 TX IND
#define DAC1 26	 //!< DAC pin for DC_SET_V

#define ENC1_ROTARY_PIN_A 34 //!< Encoder 1 Pin A
#define ENC1_ROTARY_PIN_B 35 //!< Encoder 1 Pin B

#define ENC2_ROTARY_PIN_A 39 //!< Encoder 2 Pin A
#define ENC2_ROTARY_PIN_B 36 //!< Encoder 2 Pin B

#define CLICKS_PER_STEP 2 // Number depends on the encoder

#define MIN_POS 87	//!< Max volume 0 dB
#define MAX_POS 0	//!< Min volume -87 dB
#define START_POS 0 //!< Volume start position
#define INCREMENT 1 //!< Volume increment

const int mfbPin = 23; //!< BM83 MFB Pin, required for BM83 power on

ESPRotary r; //!< Encoder 1 instance
ESPRotary u; //!< Encoder 2 instance

BD37544FS bd; //!< Sound processor instance (BD37544FS)

SoftwareSerial swSerial(RX_PIN, TX_PIN); //!< Software serial instance (BM83)
BM83 bm83(swSerial, TX_IND);			 //!< BM83 instance (BM83)

PCA9634 ledDriver(0x15, 4); //!< Led driver instance (PCA9634)

MCP23017 mcp1 = MCP23017(0x20); //!< MCP1 (Display board)
MCP23017 mcp2 = MCP23017(0x24); //!< MCP2 (MCU board)

TaskHandle_t Task1; //!< Taskhandle for "read_inputs" task

static lv_group_t *g;			  //!< An object group
static lv_indev_t *encoder_indev; //!< Encoder 1 LVGL input device

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

// Sound processor config sLiders
lv_obj_t *slider_ingain;
lv_obj_t *slider_fade_1;
lv_obj_t *slider_fade_2;
lv_obj_t *slider_bass;
lv_obj_t *slider_mid;
lv_obj_t *slider_treb;
lv_obj_t *slider_label_ingain;
lv_obj_t *slider_label_fade_1;
lv_obj_t *slider_label_fade_2;
lv_obj_t *slider_label_bass;
lv_obj_t *slider_label_mid;
lv_obj_t *slider_label_treb;
lv_obj_t *info_ingain;
lv_obj_t *info_fade_1;
lv_obj_t *info_fade_2;
lv_obj_t *info_bass;
lv_obj_t *info_mid;
lv_obj_t *info_treb;

// Global variables
uint8_t conf;
int i;
int vol; //!< Current volume value
int encoderLastValue = 0;
int k; //!< Current encoder position (volume)

TFT_eSPI tft = TFT_eSPI(); //!< TFT instance

static char buf_ingain[4]; /* max 3 bytes for number plus 1 null terminating byte */
static char buf_fade_1[4]; /* max 3 bytes for number plus 1 null terminating byte */
static char buf_fade_2[4]; /* max 3 bytes for number plus 1 null terminating byte */
static char buf_bass[4]; /* max 3 bytes for number plus 1 null terminating byte */
static char buf_mid[4]; /* max 3 bytes for number plus 1 null terminating byte */
static char buf_treb[4]; /* max 3 bytes for number plus 1 null terminating byte */

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * BUFFER_MULTIPLIER]; //!< LVGL display buffer
static lv_color_t buf_2[LV_HOR_RES_MAX * BUFFER_MULTIPLIER]; //!< LVGL display buffer

#if USE_LV_LOG != 0
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc) //!< LVGL serial debugging
{

	Serial.printf("%s@%d->%s\r\n", file, line, dsc);
	Serial.flush();
}
#endif

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) //!< LVGL display flushing
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);
	tft.pushColors(&color_p->full, w * h, true);
	tft.endWrite();

	lv_disp_flush_ready(disp);
}

void setup() //!< The standard Arduino setup function used for setup and configuration
{

	Wire.begin();		   //!< I2C init
	Wire.setClock(100000); //!< Setting I2C clock to 100 KHz

	Serial.begin(115200); //!< Serial debugging @ 115200 baud

	swSerial.begin(9600); //!< Software serial for BM83/BM64 Uart communication @ 9600 baud

	// Input read task creation
	//xTaskCreatePinnedToCore(
	//	read_inputs,   /* Task function. */
	//	"read_inputs", /* String with name of task. */
	//	10000,		   /* Stack size in bytes. */
	//	NULL,		   /* Parameter passed as input of the task */
	//	1,			   /* Priority of the task. */
	//	&Task1,		   /* Task handle. */
	//	0);			   /* Core 0 or 1 (Core 1 is used for the arduino loop function for now)*/

	// Encoder read task creation
	//xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 10000, NULL, 2, &Task2, 0);

	pinMode(mfbPin, OUTPUT); //!< Sets the MFB pin (GPIO23) as output (BM83)

	ledDriver.begin(); //!< Led driver init

	ledDriver.allOff(); //!< All  LEDs on (inverted)

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

	mcp1.init();								//!< MCP1 init
	mcp1.portMode(MCP23017Port::A, 0b11111111); //!< Set all as inputs (MCP1)
	delay(5);

	mcp2.init();					   //!< MCP2 init
	mcp2.portMode(MCP23017Port::A, 0); //!< Set all as outputs (MCP2)
	delay(5);

	// Standby sequence
	mcp2.digitalWrite(1, 0); //!< 7.5V disable
	delay(10);
	mcp2.digitalWrite(0, 0); //!< +-5V disable
	delay(10);
	mcp2.digitalWrite(2, 0); //!< PVCC disable
	delay(10);
	mcp2.digitalWrite(3, 0); //!< Power Amp shutdown activation
	delay(10);
	mcp2.digitalWrite(4, 1); //!< Power Amp mute activation

	// Power up sequence
	delay(20);
	mcp2.digitalWrite(1, 1);
	Serial.println("7.5V enabled!"); //!< 7.5V enable
	delay(10);
	mcp2.digitalWrite(0, 1);
	Serial.println("+-5V enabled!"); //!< +-5V enable
	delay(10);
	dacWrite(DAC1, 255); //!< Sets GPIO26 to ~3.1 V
	delay(10);
	mcp2.digitalWrite(2, 1);
	Serial.println("PVCC enabled!"); //!< PVCC enable
	delay(10);
	mcp2.digitalWrite(3, 1);
	Serial.println("Power Amp Shutdown deactivated!"); //!< Power amp shutdown deactivation
	delay(10);
	mcp2.digitalWrite(4, 0);
	Serial.println("Power Amp Mute deactivated!"); //!< Power amp mute deactivation
	delay(10);

	lv_init(); //!< LVGL init

	tft.begin();		//!< TFT init
	tft.setRotation(3); //!< TFT rotation (180*)

	if (!touch.begin(150)) //!< Touch controller init
	{
		Serial.println("Couldn't start FT6206 touchscreen controller");
		while (1)
			;
	}
	else
	{
		Serial.println("FT6206 touchscreen controller connected!");
	}

	lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 10); //!< LVGL buffer init

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

	//Input gain slider________________________________________

	/* Create a slider in the center of the display */
	slider_ingain = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_ingain, LV_DPI * 1);
	lv_obj_align(slider_ingain, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 25);
	lv_obj_set_event_cb(slider_ingain, slider_event_cb_ingain);
	lv_slider_set_range(slider_ingain, 0, 20);
	lv_slider_set_value(slider_ingain, 0, LV_ANIM_OFF);

	/* Create a label below the slider */
	slider_label_ingain = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_ingain, "0");
	lv_obj_set_auto_realign(slider_label_ingain, true);
	lv_obj_align(slider_label_ingain, slider_ingain, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_ingain = lv_label_create(tab4, NULL);
	lv_label_set_text(info_ingain, "Input Gain (0-20dB)");
	lv_obj_align(info_ingain, slider_ingain, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

	//Fader 1 slider________________________________________________

	slider_fade_1 = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_fade_1, LV_DPI * 1);
	lv_obj_align(slider_fade_1, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 80);
	lv_obj_set_event_cb(slider_fade_1, slider_event_cb_fade_1);
	lv_slider_set_range(slider_fade_1, 0, 87);
	lv_slider_set_value(slider_ingain, 0, LV_ANIM_OFF);

	/* Create a label below the slider */
	slider_label_fade_1 = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_fade_1, "0");
	lv_obj_set_auto_realign(slider_label_fade_1, true);
	lv_obj_align(slider_label_fade_1, slider_fade_1, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_fade_1 = lv_label_create(tab4, NULL);
	lv_label_set_text(info_fade_1, "Fader 1 (-87)-(0dB)");
	lv_obj_align(info_fade_1, slider_fade_1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

	//Fader 2 slider________________________________________________

	slider_fade_2 = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_fade_2, LV_DPI * 1);
	lv_obj_align(slider_fade_2, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 135);
	lv_obj_set_event_cb(slider_fade_2, slider_event_cb_fade_2);
	lv_slider_set_range(slider_fade_2, 0, 87);
	lv_slider_set_value(slider_fade_2, 0, LV_ANIM_OFF);

	/* Create a label below the slider */
	slider_label_fade_2 = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_fade_2, "0");
	lv_obj_set_auto_realign(slider_label_fade_2, true);
	lv_obj_align(slider_label_fade_2, slider_fade_2, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_fade_2 = lv_label_create(tab4, NULL);
	lv_label_set_text(info_fade_2, "Fader 2 (-87)-(0dB)");
	lv_obj_align(info_fade_2, slider_fade_2, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

	//Bass slider_______________________________________________________

	slider_bass = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_bass, LV_DPI * 1);
	lv_obj_align(slider_bass, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 25);
	lv_obj_set_event_cb(slider_bass, slider_event_cb_bass);
	lv_slider_set_range(slider_bass, -7, 7);

	/* Create a label below the slider */
	slider_label_bass = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_bass, "0");
	lv_obj_set_auto_realign(slider_label_bass, true);
	lv_obj_align(slider_label_bass, slider_bass, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_bass = lv_label_create(tab4, NULL);
	lv_label_set_text(info_bass, "Bass (-14)-(+14dB)");
	lv_obj_align(info_bass, slider_bass, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

	//Middle slider_________________________________________________

	slider_mid = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_mid, LV_DPI * 1);
	lv_obj_align(slider_mid, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 80);
	lv_obj_set_event_cb(slider_mid, slider_event_cb_mid);
	lv_slider_set_range(slider_mid, -7, 7);

	/* Create a label below the slider */
	slider_label_mid = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_mid, "0");
	lv_obj_set_auto_realign(slider_label_mid, true);
	lv_obj_align(slider_label_mid, slider_mid, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_mid = lv_label_create(tab4, NULL);
	lv_label_set_text(info_mid, "Midd (-14)-(+14dB)");
	lv_obj_align(info_mid, slider_mid, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

	//Treble slider_________________________________________________

	slider_treb = lv_slider_create(tab4, NULL);
	lv_obj_set_width(slider_treb, LV_DPI * 1);
	lv_obj_align(slider_treb, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 135);
	lv_obj_set_event_cb(slider_treb, slider_event_cb_treb);
	lv_slider_set_range(slider_treb, -7, 7);
	lv_slider_set_value(slider_treb, 0, LV_ANIM_OFF);

	/* Create a label below the slider */
	slider_label_treb = lv_label_create(tab4, NULL);
	lv_label_set_text(slider_label_treb, "0");
	lv_obj_set_auto_realign(slider_label_treb, true);
	lv_obj_align(slider_label_treb, slider_treb, LV_ALIGN_OUT_BOTTOM_MID, 5, 10);

	/* Create an informative label */
	info_treb = lv_label_create(tab4, NULL);
	lv_label_set_text(info_treb, "Treb (-14)-(+14dB)");
	lv_obj_align(info_treb, slider_treb, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

	//______________________________________________________________

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

void loop() //< Standard arduino setup function
{
	TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE; //!< Watchdog timer manipulation
	TIMERG0.wdt_feed = 1;						//<
	TIMERG0.wdt_wprotect = 0;					//<

	conf = mcp1.readRegister(MCP23017Register::GPIO_A); //< MCP1 register read

	r.loop(); //< Encoder 1 menu loop
	u.loop(); //< Encoder 2 volume loop

	lv_task_handler(); //< LVG task handler loop

	delay(3);
}

void read_inputs(void *parameter) //< Buttons read function

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

static void event_bm83setup(lv_obj_t *obj, lv_event_t event) //< BM83 setup function
{
	if (event == LV_EVENT_CLICKED)
	{
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

// on change
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
	k = u.getPosition();
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

static void slider_event_cb_ingain(lv_obj_t *slider_ingain, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		
		snprintf(buf_ingain, 4, "%u", lv_slider_get_value(slider_ingain));
		lv_label_set_text(slider_label_ingain, buf_ingain);
		//bd.setIn_gain(lv_slider_get_value(slider_ingain));
	}
}

static void slider_event_cb_fade_1(lv_obj_t *slider_fade_1, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		
		snprintf(buf_fade_1, 4, "%u", lv_slider_get_value(slider_fade_1));
		lv_label_set_text(slider_fade_1, buf_fade_1);
		//bd.setFad_1(lv_slider_get_value(slider_fade_1));
	}
}

static void slider_event_cb_fade_2(lv_obj_t *slider_fade_2, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		snprintf(buf_fade_2, 4, "%u", lv_slider_get_value(slider_fade_2));
		lv_label_set_text(slider_fade_2, buf_fade_2);
		//bd.setFad_2(lv_slider_get_value(slider_fade_2));
	}
}

static void slider_event_cb_bass(lv_obj_t *slider_bass, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		
		snprintf(buf_bass, 4, "%u", lv_slider_get_value(slider_bass));
		lv_label_set_text(slider_bass, buf_bass);
		//bd.setBass(lv_slider_get_value(slider_bass));
	}
}

static void slider_event_cb_mid(lv_obj_t *slider_mid, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		
		snprintf(buf_mid, 4, "%u", lv_slider_get_value(slider_mid));
		lv_label_set_text(slider_label_mid, buf_mid);
		//bd.setMidd(lv_slider_get_value(slider_mid));
	}
}

static void slider_event_cb_treb(lv_obj_t *slider_treb, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		
		snprintf(buf_treb, 4, "%u", lv_slider_get_value(slider_treb));
		lv_label_set_text(slider_label_treb, buf_treb);
		//bd.setTreb(lv_slider_get_value(slider_treb));
	}
}