---
title: C:/Users/Jack/Documents/GitHub/RAU-DAB-Radio-V2/New Code_ab 16.06.2021/DAB_2_V0.1/touch.h

---

# C:/Users/Jack/Documents/GitHub/RAU-DAB-Radio-V2/New Code_ab 16.06.2021/DAB_2_V0.1/touch.h

## Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[my_touchpad_read](/Files/touch_8h/#function-my_touchpad_read)**(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) |

## Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[oldTouchX](/Files/touch_8h/#variable-oldtouchx)**  |
| int | **[oldTouchY](/Files/touch_8h/#variable-oldtouchy)**  |
| Adafruit_FT6206 | **[touch](/Files/touch_8h/#variable-touch)**  |
| unsigned long | **[last_user_input](/Files/touch_8h/#variable-last_user_input)**  |
| bool | **[tft_is_off](/Files/touch_8h/#variable-tft_is_off)**  |
| bool | **[tft_needs_on](/Files/touch_8h/#variable-tft_needs_on)**  |


## Functions Documentation

### function my_touchpad_read

```cpp
bool my_touchpad_read(
    lv_indev_drv_t * indev_driver,
    lv_indev_data_t * data
)
```



## Attributes Documentation

### variable oldTouchX

```cpp
int oldTouchX = 0;
```


### variable oldTouchY

```cpp
int oldTouchY = 0;
```


### variable touch

```cpp
static Adafruit_FT6206 touch = Adafruit_FT6206();
```


### variable last_user_input

```cpp
unsigned long last_user_input = 0;
```


### variable tft_is_off

```cpp
bool tft_is_off = false;
```


### variable tft_needs_on

```cpp
bool tft_needs_on = false;
```



## Source code

```cpp
#include <Adafruit_FT6206.h>
#include <lvgl.h>



int oldTouchX = 0;
int oldTouchY = 0;

static Adafruit_FT6206 touch = Adafruit_FT6206();

unsigned long last_user_input = 0;
bool tft_is_off = false;
bool tft_needs_on = false;

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  uint16_t touchX, touchY;

  if (touch.touched())
  {   
    // First wake up the screen if its shut off
    if (tft_is_off) {
        tft_is_off = false;
        tft_needs_on = true;
    }

    // Retrieve a point  
    TS_Point p = touch.getPoint(); 

    touchX = p.y;
    touchY = p.x;
    touchY = 240-touchY; //320 instead of 240 for rotation 0 ???
    
    // 2.8" with Rotation 1
    //touchX = p.y;
    //touchY = p.x;
    //touchX = 320-touchX; 
    
    // 3.2" with Rotation 1
    // touchX = p.y;
    // touchY = p.x;
    // touchY = 240-touchY;  
    

    // rotate coordinate system
    // flip it around to match the screen. only on rotation 0!!!!
    //uint16_t swapped_touchX = touchY;
    //touchY = touchX;
    //touchX = swapped_touchX;
    //touchX = map(touchX, 0, 320, 320, 0);
    // touchY = map(touchY, 0, 320, 320, 0);

    // if ((touchX != oldTouchX) || (touchY != oldTouchY)) //no need
    // { //no need
    oldTouchY = touchY;
    oldTouchX = touchX;
    data->state = LV_INDEV_STATE_PR; 
    //  data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
    data->point.x = touchX;
    data->point.y = touchY;
    last_user_input = millis();

    Serial.print("X: ");
    Serial.print(touchX);
    Serial.print(" Y: ");
    Serial.println(touchY);
    // }
  } else {
    
    data->point.x = oldTouchX;
    data->point.y = oldTouchY;
    data->state =LV_INDEV_STATE_REL;
    } 
  return 0;
}
```


-------------------------------

Updated on  1 July 2021 at 16:33:25 W. Europe Summer Time
