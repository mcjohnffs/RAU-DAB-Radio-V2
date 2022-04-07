#include "BM64.h"

#define TX_IND 7
BM64 bm64(Serial, TX_IND);

void onEventCallback(BM64_event_t *event){
    // handle event
    switch(event->event_code)
    {
    case BM64_EVENT_CALL_STATUS:
        if(event->parameter[1] == INCOMING_CALL){
            // Incoming Call
        }else if(event->parameter[1] == IDLE){
            // End call event
        }
        break;
    case BM64_EVENT_DEVICE_STATE:
        if(event->parameter[0] == 0x00){
            // Power Off Event
        }else if(event->parameter[0] == 0x02){
            // Power On Event
        }
        break;
    }
}


void setup() {
    Serial.begin(19200);
    bm64.setCallback(onEventCallback);
}

void loop() {
    //Check UART Event
    bm64.run();
}
