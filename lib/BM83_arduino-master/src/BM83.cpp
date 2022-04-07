#include "BM83.h"

USE_DEBUG

void BM83::getStatus(){
    sendCommand(BM83_GET_UART_VERSION_CMD, 0x00);
}

void BM83::musicControl(BM83_music_control_act act){
    sendCommand(BM83_MUSIC_CONTROL_CMD, 0, act);
}

void BM83::generateTone(uint8_t param){
    sendCommand(BM83_BTM_UTILITY_FUNCTION_CMD, 0x02, param);
}

void BM83::startBt(){
    sendCommand(BM83_BTM_UTILITY_FUNCTION_CMD, 0x03);
}

void BM83::mmiAction(BM83_mmi_cmd act){
    sendCommand(BM83_MMI_CMD, 0, act);
}

void BM83::powerOn(){
    digitalWrite(_txInd, HIGH);
    delay(400);
    mmiAction(BM83_MMI_POWERON_BUTTON_PRESS);
    mmiAction(BM83_MMI_POWERON_BUTTON_RELEASE);
}

void BM83::powerOff(){
    mmiAction(BM83_MMI_DISCONNECT_ALL_LINK);
    mmiAction(BM83_MMI_POWEROFF_BT);
    // mmiAction(BM83_MMI_POWEROFF_BUTTON_PRESS);
    // mmiAction(BM83_MMI_POWEROFF_BUTTON_RELEASE);
}

void BM83::setCallback(BM83_callback callback){
    _callback = callback;
}

void BM83::makeCall(const char *number, uint8_t len){
    sendCommand(BM83_MAKE_CALL_CMD, 0, (uint8_t*)number, len);
}

void BM83::makeCall(String *number){
    const char *c_number = number->c_str();
    makeCall(c_number, number->length() + 1);
}

BM83_call_status BM83::getCallStatus(){
    return _call_status;
}

void BM83::run(){
    if(_serial.available() > 0){
        uint8_t b = _serial.read();
        if(b != 0xAA){
            return;
        }
        DPRINTLN("[Received]");
        DPRINTLN("AA found");
        
        uint8_t len;
        if(_serial.available() == 0) delay(2);
        if(_serial.available() > 0){
            uint8_t b_0 = _serial.read();
            if(b_0 != 0) return;
            len = _serial.read();
        }

        DPRINT("len found: ");
        DPRINTLN(len, HEX);

        if(_serial.available() < len){
            delay(len);
        }
        uint8_t eventCode = _serial.read();

        uint8_t *cmd = (uint8_t *)malloc(sizeof(uint8_t) * (len - 1));
        BM83_event_t event = {(BM83_EVENT)eventCode, cmd, len - 1};

        for(int i=0; i<len-1; i++){
            if(_serial.available() > 0){
                event.parameter[i] = _serial.read();
            }
        }

        DPRINT("event found: ");
        DPRINTLN(eventCode, HEX);

        for(int i=0; i<event.param_len; i++){
            DPRINT(event.parameter[i], HEX);
            DPRINT(" ");
        }
        DPRINTLN("");

        if(_serial.available() < 1){
            delay(3);
        }
        uint8_t v_checksum = _serial.read();

        DPRINT("Checksum: ");
        DPRINTLN(v_checksum, HEX);

        uint8_t sum = len + eventCode + v_checksum;
        for(int i=0; i < event.param_len; i++){
            sum += event.parameter[i];
        }
        if(sum == 0){
            DPRINTLN("Checksum OK");
            handleEvent(&event);
        }else{
            DPRINTLN("Checksum Failed");
        }
        free(event.parameter);
    }
}

void BM83::handleEvent(BM83_event_t *event){
    switch (event->event_code)
    {
    case BM83_EVENT_ACK:
        DPRINTLN("Ack Event");
        _prev_ack_op_code = event->parameter[0];
        break;
    case BM83_EVENT_CALL_STATUS:
        DPRINTLN("Call Status Event");
        DPRINT("Call Status: ");
        DPRINTLN(event->parameter[1]);
        _call_status = (BM83_call_status)event->parameter[1];
        break;
    default:
        break;
    }

    if(event->event_code != BM83_EVENT_ACK){
        // Send ACK
        _callback(event);
        sendCommand(BM83_MCU_SEND_EVENT_ACK_CMD, (uint8_t *)&event->event_code, 2, false);
    }
}

bool BM83::checkAck(uint8_t opCode){
    DPRINT("opCode: ");
    DPRINTLN(opCode, HEX);
    DPRINT("prevAck: ");
    DPRINTLN(_prev_ack_op_code);
    if(_prev_ack_op_code == opCode){
        _prev_ack_op_code = -1;
        return true;
    }else{
        return false;
    }
}

void BM83::sendCommand(uint8_t opCode, uint8_t *parameter, uint8_t len, bool isCheckAck){
    uint8_t command[len+4];
    digitalWrite(_txInd, HIGH);
    _buildCommand(command, opCode, parameter, len);
    DPRINTLN("[Send]");
    for(int i=0; i<len+4; i++){
        DPRINT(command[i], HEX);
        DPRINT(" ");
    }
    DPRINTLN("");
    delay(2);
    _serial.write(command, len+4);
    delay(1);
    digitalWrite(_txInd, LOW);
    unsigned long time = millis();
    if(isCheckAck){
        while(!checkAck(opCode) && abs(millis() - time) < 200){ 
            run();
        }
    }

}

void BM83::sendCommand(uint8_t opCode, uint8_t parameter){
    sendCommand(opCode, &parameter, 2);
}

void BM83::sendCommand(uint8_t opCode, int link_index, uint8_t parameter){
    uint8_t params[2] = {link_index, parameter};
    sendCommand(opCode, params, 3);
}

void BM83::sendCommand(uint8_t opCode, int link_index, uint8_t *parameter, uint8_t len){
    uint8_t param[len+1];
    param[0] = link_index;
    memcpy(&param[1], parameter, len);
    sendCommand(opCode, param, len+1);
}

uint8_t* BM83::_buildCommand(uint8_t* command, uint8_t opCode, uint8_t* parameter, uint8_t length){
    uint8_t init[4] = {0xAA, 0x00, length, opCode};
    memcpy(command, init, 4);
    memcpy(&command[4], parameter, length);
    command[length + 3] = _checksum(&command[2], length+1);

    return command;
}

uint8_t BM83::_checksum(uint8_t *start, uint8_t len){
    uint8_t checksum = 0;
    for(int i=0; i<len; i++){
        checksum += start[i];
    }
    checksum = ~checksum + 1;
    return checksum;
}
