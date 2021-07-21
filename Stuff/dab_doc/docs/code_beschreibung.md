
# Code Beschreibung

## Code Aufbau

### 1. Bibliotheken / Libraries

```c
#include <lvgl.h>			//!< Light and versatile Embedded Graphics Library (LVGL)
#include <TFT_eSPI.h>		//!< General TFT library (TFT_eSPI)
#include "touch.h"			//!< Communication header for the touch controller - between "FT6206" and LVGL
#include <Wire.h>			//!< Arduino I2C library
#include <PCA9634.h>		//!< LED driver library (PCA9634)
#include <MCP23017.h>		//!< Port expander library (MPC23017)
#include "BM83.h"			//!< Bluetooth Module library (BM83/BM64)
#include <SoftwareSerial.h> //!< SoftwareSerial Library for ESP32
#include <BD37544FS.h>		//!< Sound processor library (BD37544FS)
#include "ESPRotary.h"		//!< Encoder library for ESP32
```

### 2. Variablen

```c
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

static lv_group_t *g;			  //!< An object group
static lv_indev_t *encoder_indev; //!< Encoder 1 LVGL input device

LV_IMG_DECLARE(raulogo);

// Global variables
uint8_t conf;
int i;
int vol; //!< Current volume value
int encoderLastValue = 0;
int k; //!< Current encoder position (volume)
int menu_status = 0;

TaskHandle_t Task1; //!< Taskhandle for "read_inputs" task
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4; //!< Taskhandle for "read_inputs" task
TaskHandle_t Task5;
TaskHandle_t Task6;

// Global LVGL object variables
lv_obj_t *splash_screen;

lv_obj_t *music_control_container;
lv_obj_t *status_bar_container;
lv_obj_t *audio_control_container;

lv_obj_t *tabview;

lv_obj_t *tab_home;
lv_obj_t *tab_vis;
lv_obj_t *tab_bt;
lv_obj_t *tab_aux;
lv_obj_t *tab_dab;
lv_obj_t *tab_menu;
lv_obj_t *tab_bt_settings;
lv_obj_t *tab_audio_settings;
lv_obj_t *tab_power_settings;
lv_obj_t *tab_display_settings;
lv_obj_t *tab_led_settings;

lv_obj_t *btn_play;
lv_obj_t *btn_next;
lv_obj_t *btn_prev;
lv_obj_t *btn_mute;

lv_obj_t *list1;
lv_obj_t *list_btn_bt;
lv_obj_t *list_btn_audio;
lv_obj_t *list_btn_disp;
lv_obj_t *list_btn_leds;
lv_obj_t *list_btn_misc;
lv_obj_t *list_btn_pwr;

lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *btn3;

lv_obj_t *sw1;
lv_obj_t *sw2;
lv_obj_t *sw3;

lv_obj_t *label;
lv_obj_t *label_battery;
lv_obj_t *label_bluetooth;
lv_obj_t *label_clock;

// Sound processor config sLiders "objects"
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

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * BUFFER_MULTIPLIER]; //!< LVGL display buffer
static lv_color_t buf_2[LV_HOR_RES_MAX * BUFFER_MULTIPLIER]; //!< LVGL display buffer
```

### LVGL
- 

### TFT_eSPI
- 

### touch
- 
### Wire
- 
### PCA9634
- 
### MCP23017
- 
### BM83
- 
### SoftwareSerial
- 
### BD37544FS
- 
### ESPRotary
- 

