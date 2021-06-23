#include <bq25895.h>



#include <Wire.h> 



//Sound processor init


bq25895 bq;


void setup(){
   Wire.begin();
   Wire.setClock(100000);

   charger();

}

void loop(){


  }
  
void charger(){

  bq.setVsys(1);//
  bq.setShip(0);// 0 - off, 1 - on

  }
  
