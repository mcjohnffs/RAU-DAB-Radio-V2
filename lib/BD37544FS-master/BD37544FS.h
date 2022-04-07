// ALEXSANDER LIMAN
// liman324@yandex.ru
// rcl-radio.ru
// 08.11.2019

#ifndef BD37544FS_H
#define BD37544FS_H

#define BD37544FS_address 0b1000000

#define INPUT_SELECT        0x05
#define INPUT_GAIN          0x06
#define VOLUME_GAIN_1       0x20
#define FADER_GAIN_1        0x28
#define FADER_GAIN_2        0x29
#define BASS_GAIN           0x51
#define MIDDLE_GAIN         0x54
#define TREBLE_GAIN         0x57


#include <Arduino.h>
class BD37544FS
{
  public:
    BD37544FS();
    void setSelect(int in);// int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
    void setIn_gain(int in_gain); // int 0...7 === 0...20 dB    
    void setVol_1(int vol_1); // int 0...87 === 0...-87 dB
    void setFad_1(int Fad_1); // int 0...87 === 0...-87 dB
	void setFad_2(int Fad_2); // int 0...87 === 0...-87 dB
    void setBass(int bass); // int -7...0...+7 === -14...+14 dB
	void setMidd(int midd); // int -7...0...+7 === -14...+14 dB
    void setTreb(int treb); // int -7...0...+7 === -14...+14 dB
	
  private:
	void writeWire(char a, char b);
};
	
#endif //BD37544FS_H
