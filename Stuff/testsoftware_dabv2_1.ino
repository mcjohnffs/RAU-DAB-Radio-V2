#include "BM83.h" // Bluetooth Module BM83 Library
#include <SoftwareSerial.h> //SoftwareSerial Library for ESP32
#include <APA102.h>  //RGB LED's Library
#include <BD37544FS.h>
#include <Wire.h>

#include <SparkFunBQ27441.h>

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 9000; // e.g. 850mAh battery

BD37544FS bd;

//----------------------------------------------------------------------RGB LEDS INIT START----



//----------------------------------------------------------------------RGB LEDS INIT END------


//----------------------------------------------------------------------BM83 INIT START----
#define RX_PIN 16
#define TX_PIN 17
#define TX_IND 5

#define I2C_SCL 22
#define I2C_SDA 21

const int mfbPin =  4;

SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);



//----------------------------------------------------------------------BM83 INIT END--------

void setup() {




//----------------------------------------------------------------------BM83 SETUP START(MFB-> HIGH, powerOn Command, enter Pairing, MFB-> LOW --------

  pinMode(mfbPin, OUTPUT);    // sets the MFB Pin 4 as output
    Serial.begin(9600);
    
    swSerial.begin(115200);


  digitalWrite(mfbPin, HIGH); // sets the MFB Pin 4 "High" to power on BM83 over BAT_IN
  delay(500);     


    bm83.run();
    bm83.setCallback(onEventCallback);
    bm83.powerOn(); // Sends "power on" command over UART to BM83
    bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING); // Sends "enter Pairing" command over UART to BM83

    digitalWrite(mfbPin, LOW); // sets the MFB Pin 4 "LOW" (no longer needed after power on process)

//----------------------------------------------------------------------BM83 SETUP END--------

Wire.begin(I2C_SDA, I2C_SCL);
Wire.setClock(100000);
audio();
setupBQ27441();
}

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

void loop() {

  bm83.run();
    
     //BM83 start routine (required)

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

        }
    }

//----------------------------------------------------------------------RGB LED Code LOOP--------

//----------------------------------------------------------------------RGB LED Code LOOP END--------
printBatteryStats();
  delay(500);




}

void audio(){
    bd.setSelect(0);// int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
    bd.setIn_gain(0); // int 0...7 === 0...20 dB    
    bd.setVol_1(-25); // int 0...87 === 0...-87 dB
    bd.setFad_1(0); // int 0...87 === 0...-87 dB
    bd.setFad_2(0); // int 0...87 === 0...-87 dB
    bd.setBass(0); // int -7...0...+7 === -14...+14 dB
    bd.setMidd(0); // int -7...0...+7 === -14...+14 dB
    bd.setTreb(0); // int -7...0...+7 === -14...+14 dB
  }

void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
  // If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    while (1) ;
  }
  Serial.println("Connected to BQ27441!");
  
  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}

void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)
  unsigned int volts = lipo.voltage(); // Read battery voltage (mV)
  int current = lipo.current(AVG); // Read average current (mA)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  int power = lipo.power(); // Read average power draw (mW)
  int health = lipo.soh(); // Read state-of-health (%)

  // Now print out those values:
  String toPrint = String(soc) + "% | ";
  toPrint += String(volts)+ " mV Voltage | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";
  
  Serial.println(toPrint);
}