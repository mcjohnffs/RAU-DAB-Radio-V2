#include <Arduino.h>
#include <Wire.h>
#include "BD37544FS.h"

BD37544FS::BD37544FS(){
	Wire.begin();
}

void BD37544FS::setSelect(int in){
     writeWire(INPUT_SELECT, in); 
}

void BD37544FS::setIn_gain(int in_gain){
     switch(in_gain){
       case 0: in_gain = 0b00000000;break;
       case 1: in_gain = 0b00000001;break;
       case 2: in_gain = 0b00000010;break;
       case 3: in_gain = 0b00000011;break;
       case 4: in_gain = 0b00000100;break;
       case 5: in_gain = 0b00000101;break;
       case 6: in_gain = 0b00000110;break;
       case 7: in_gain = 0b00000111;break;
	   case 8: in_gain = 0b00001000;break;
       case 9: in_gain = 0b00001001;break;
       case 10: in_gain = 0b00001010;break;
       case 11: in_gain = 0b00001011;break;
       case 12: in_gain = 0b00001100;break;
       case 13: in_gain = 0b00001101;break;
       case 14: in_gain = 0b00001110;break;
       case 15: in_gain = 0b00001111;break;
	   case 16: in_gain = 0b00010000;break;
       case 17: in_gain = 0b00010001;break;
       case 18: in_gain = 0b00010010;break;
       case 19: in_gain = 0b00010011;break;
       case 20: in_gain = 0b00010100;break;
}
     writeWire(INPUT_GAIN, in_gain); 
}

void BD37544FS::setVol_1(int vol_1){
     writeWire(VOLUME_GAIN_1, vol_1 + 0b10000000); 
}





void BD37544FS::setFad_1(int fad_1){
     writeWire(FADER_GAIN_1, fad_1 + 0b10000000); 
}

void BD37544FS::setFad_2(int fad_2){
     writeWire(FADER_GAIN_2, fad_2 + 0b10000000); 
}

void BD37544FS::setBass(int bass){
     switch(bass){
       case 0: bass = 0b00000000;break;
       case 1: bass = 0b00000001;break;
       case 2: bass = 0b00000010;break;
       case 3: bass = 0b00000011;break;
       case 4: bass = 0b00000100;break;
       case 5: bass = 0b00000101;break;
       case 6: bass = 0b00000110;break;
       case 7: bass = 0b00000111;break;
	   case 8: bass = 0b00001000;break;
       case 9: bass = 0b00001001;break;
       case 10: bass = 0b00001010;break;
       case 11: bass = 0b00001011;break;
       case 12: bass = 0b00001100;break;
       case 13: bass = 0b00001101;break;
       case 14: bass = 0b00001110;break;
       case 15: bass = 0b00001111;break;
	   case 16: bass = 0b00010000;break;
	   case 17: bass = 0b00010001;break;
	   case 18: bass = 0b00010010;break;
	   case 19: bass = 0b00010011;break;
	   case 20: bass = 0b00010100;break;
       case -1: bass = 0b10010101;break;
       case -2: bass = 0b10000100;break;
       case -3: bass = 0b10000110;break;
       case -4: bass = 0b10001000;break;
       case -5: bass = 0b10001010;break;
       case -6: bass = 0b10001100;break;
       case -7: bass = 0b10001110;break;
}
     writeWire(BASS_GAIN, bass); 
}

void BD37544FS::setMidd(int midd){
     switch(midd){
       case 0: midd = 0b00000000;break;
       case 1: midd = 0b00000001;break;
       case 2: midd = 0b00000010;break;
       case 3: midd = 0b00000011;break;
       case 4: midd = 0b00000100;break;
       case 5: midd = 0b00000101;break;
       case 6: midd = 0b00000110;break;
       case 7: midd = 0b00000111;break;
	   case 8: midd = 0b00001000;break;
       case 9: midd = 0b00001001;break;
       case 10: midd = 0b00001010;break;
       case 11: midd = 0b00001011;break;
       case 12: midd = 0b00001100;break;
       case 13: midd = 0b00001101;break;
       case 14: midd = 0b00001110;break;
       case 15: midd = 0b00001111;break;
	   case 16: midd = 0b00010000;break;
	   case 17: midd = 0b00010001;break;
	   case 18: midd = 0b00010010;break;
	   case 19: midd = 0b00010011;break;
	   case 20: midd = 0b00010100;break;
       case -1: midd = 0b10000010;break;
       case -2: midd = 0b10000100;break;
       case -3: midd = 0b10000110;break;
       case -4: midd = 0b10001000;break;
       case -5: midd = 0b10001010;break;
       case -6: midd = 0b10001100;break;
       case -7: midd = 0b10001110;break;
}
     writeWire(MIDDLE_GAIN, midd); 
}

void BD37544FS::setTreb(int treb){
     switch(treb){
       case 0: treb = 0b00000000;break;
       case 1: treb = 0b00000001;break;
       case 2: treb = 0b00000010;break;
       case 3: treb = 0b00000011;break;
       case 4: treb = 0b00000100;break;
       case 5: treb = 0b00000101;break;
       case 6: treb = 0b00000110;break;
       case 7: treb = 0b00000111;break;
	   case 8: treb = 0b00001000;break;
       case 9: treb = 0b00001001;break;
       case 10: treb = 0b00001010;break;
       case 11: treb = 0b00001011;break;
       case 12: treb = 0b00001100;break;
       case 13: treb = 0b00001101;break;
       case 14: treb = 0b00001110;break;
       case 15: treb = 0b00001111;break;
	   case 16: treb = 0b00010000;break;
	   case 17: treb = 0b00010001;break;
	   case 18: treb = 0b00010010;break;
	   case 19: treb = 0b00010011;break;
	   case 20: treb = 0b00010100;break;
       case -1: treb = 0b10000010;break;
       case -2: treb = 0b10000100;break;
       case -3: treb = 0b10000110;break;
       case -4: treb = 0b10001000;break;
       case -5: treb = 0b10001010;break;
       case -6: treb = 0b10001100;break;
       case -7: treb = 0b10001110;break;
}
     writeWire(TREBLE_GAIN, treb); 
}



void BD37544FS::writeWire(char a, char b){
  Wire.beginTransmission(BD37544FS_address);
  Wire.write (a);
  Wire.write (b);
  Wire.endTransmission();
}
