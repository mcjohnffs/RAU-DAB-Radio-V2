#include <BD37544FS.h>

#include <Wire.h> 



//Sound processor init

BD37544FS bd;



void setup(){
   Wire.begin();
   Wire.setClock(100000);
   audio();


}

void loop(){


  }

void audio(){
    bd.setSelect(0);// int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
    bd.setIn_gain(0); // int 0...7 === 0...20 dB    
    bd.setVol_1(0); // int 0...87 === 0...-87 dB
    bd.setFad_1(0); // int 0...87 === 0...-87 dB
    bd.setFad_2(0); // int 0...87 === 0...-87 dB
    bd.setBass(0); // int -7...0...+7 === -14...+14 dB
    bd.setMidd(0); // int -7...0...+7 === -14...+14 dB
    bd.setTreb(0); // int -7...0...+7 === -14...+14 dB
  }
