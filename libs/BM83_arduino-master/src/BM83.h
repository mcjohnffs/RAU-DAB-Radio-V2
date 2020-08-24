#ifndef BM83_h
#define BM83_h
#include "BM83_enum.h"
#include <Arduino.h>
#include "BM83_Debug.h"

typedef struct {
    BM83_EVENT event_code;
    uint8_t *parameter;
    int param_len;
} BM83_event_t;

typedef void (*BM83_callback)(BM83_event_t *event);

class BM83 {
public:
    BM83(Stream &serial, int txInd) : _serial(serial), _txInd(txInd) {
        pinMode(txInd, OUTPUT);
    };
    void getStatus();
    void musicControl(BM83_music_control_act act);
    void generateTone(uint8_t param);
	void startBt();
    void mmiAction(BM83_mmi_cmd act);
    void powerOn();
    void powerOff();
    void makeCall(String *number);
    void makeCall(const char *number, uint8_t len);
    BM83_call_status getCallStatus();
    void run();
    void setCallback(BM83_callback callback);
private:
    // Stream *_swSerial;
    Stream &_serial;
    int _txInd;
    uint8_t _prev_ack_op_code = -1;
    BM83_callback _callback;
    BM83_call_status _call_status = IDLE;
    void handleEvent(BM83_event_t *event);
    uint8_t* _buildCommand(uint8_t* command, uint8_t opCode, uint8_t* parameter, uint8_t len);
    uint8_t _checksum(uint8_t* start, uint8_t len);
    bool checkAck(uint8_t opCode);
    void sendCommand(uint8_t opCode, uint8_t *parameter, uint8_t len, bool isCheckAck = true);
    void sendCommand(uint8_t opCode, uint8_t parameter);
    void sendCommand(uint8_t opCode, int link_index, uint8_t parameter);
    void sendCommand(uint8_t opCode, int link_index, uint8_t *parameter, uint8_t len);
};

#endif
