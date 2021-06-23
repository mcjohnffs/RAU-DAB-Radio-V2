#include "BM83.h"
#include <SoftwareSerial.h>

#define RX_PIN 5
#define TX_PIN 6
#define TX_IND 7

SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

void onEventCallback(BM83_event_t *event){
    // handle Event
    Serial.print("[EVENT]: ");
    Serial.println(event->event_code, HEX);

    for(int i=0; i<event->param_len; i++){
        Serial.print(event->parameter[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
}


void setup() {
    Serial.begin(9600);
    swSerial.begin(115200);
    bm83.setCallback(onEventCallback);
}

void loop() {
    //Check UART Event
    bm83.run();
    if (Serial.available() > 0)
    {
      
    // read the incoming byte:
    char c = Serial.read();
    switch (c)
    {
        case 'a':
            bm83.getStatus();
            break;
        case 'b':
            // Other Action, Please see BM83_Debug.h
            bm83.musicControl(MUSIC_CONTROL_PLAY);
            break;
        case 'c':
            // Other Action, Please see BM83_Debug.h
            bm83.musicControl(MUSIC_CONTROL_NEXT);
            break;
        case 'd':
            bm83.startBt();
            break;
        case 'e':
            // Other Action, Please see BM83_Debug.h
            bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING);
            break;
        case 'f':
            // Other Action, Please see BM83_Debug.h
            bm83.mmiAction(BM83_MMI_MUTE_MIC);
            break;
        case 'g':
            bm83.powerOff();
            break;
        case 'h':
            bm83.powerOn();
            break;
        case 'i':
            String number_s = "1234567890";
            bm83.makeCall(&number_s);
            break;
        case 'j':
            char number_c[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
            int len = 10;
            bm83.makeCall(number_c, len);
            break;
        }
    }
}
