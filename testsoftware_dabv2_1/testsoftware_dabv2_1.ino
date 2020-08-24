#include "esp_system.h"
#include "BM83.h"            // Bluetooth Module BM83 Library
#include <SoftwareSerial.h>  //SoftwareSerial Library for ESP32
#include <APA102.h>          //RGB LED's Library
#include <BD37544FS.h>       //Sound processor Library
#include "Wire.h"           //i2c library
#include <SparkFunBQ27441.h> //fuel gauge library


// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 9000; // e.g. 850mAh battery

BD37544FS bd;

#define RX_PIN 16
#define TX_PIN 17
#define TX_IND 5

const int mfbPin = 4;

int fuelgauge_init = 0;

const int button = 0;         //gpio to use to trigger delay
const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

SemaphoreHandle_t xSemaphore;
TaskHandle_t xHandle;

// constants won't change. They're used here to set pin numbers:
const int button1Pin = 15;    // the number of the pushbutton pin

int buttonState1 = 0;         // variable for reading the pushbutton status

void bm83_loop(void *pvParameters)
{
  while (1) {

    bm83.run();

    vTaskDelay(100);
  }




}
void loop() {}

void read_print_fuelgauge(void *pvParameters)
{
  while (1)
  {
    if (fuelgauge_init == 1)
    {
      // Read battery stats from the BQ27441-G1A
      unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
      unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
      int current = lipo.current(AVG);                 // Read average current (mA)
      unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
      unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
      int power = lipo.power();                        // Read average power draw (mW)
      int health = lipo.soh();                         // Read state-of-health (%)

      // Now print out those values:
      String toPrint = String(soc) + "% | ";
      toPrint += String(volts) + " mV | ";
      toPrint += String(current) + " mA | ";
      toPrint += String(capacity) + " / ";
      toPrint += String(fullCapacity) + " mAh | ";
      toPrint += String(power) + " mW | ";
      toPrint += String(health) + "%";

      Serial.println(toPrint);
    }


    vTaskDelay(1000);
  }

}
void bm83_setup(void *pvParameters)
{
  Serial.println("Setting up BM83");
  //BM83 start routine (required)1
  pinMode(mfbPin, OUTPUT);    // sets the MFB Pin 4 as output
  digitalWrite(mfbPin, HIGH); // sets the MFB Pin 4 "High" to power on BM83 over BAT_IN
  vTaskDelay(10);
  bm83.powerOn();                                    // Sends "power on" command over UART to BM83
  bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING); // Sends "enter Pairing" command over UART to BM83
  digitalWrite(mfbPin, LOW);                         // sets the MFB Pin 4 "LOW" (no longer needed after power on process)
  Serial.println("BM83 Setup complete...");
  Serial.print(SDA);
  Serial.print(SCL);
  vTaskDelay(100);
  vTaskDelete(NULL);
}

void sound_proc_setup(void *pvParameters)
{
while (1) {
buttonState1 = digitalRead(button1Pin);
  if (buttonState1 == HIGH){
  bd.setSelect(0);   // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
  bd.setIn_gain(10); // int 0...7 === 0...20 dB
  bd.setVol_1(1);    // int 0...87 === 0...-87 dB
  bd.setFad_1(1);    // int 0...87 === 0...-87 dB
  bd.setFad_2(1);    // int 0...87 === 0...-87 dB
  bd.setBass(0);     // int -7...0...+7 === -14...+14 dB
  bd.setMidd(0);     // int -7...0...+7 === -14...+14 dB
  bd.setTreb(0);     // int -7...0...+7 === -14...+14 dB

  Serial.println("Sound p Setup complete...");

  }
  vTaskDelay(100);
  
}
}

void onEventCallback(BM83_event_t *event)
{
  // handle Event
  Serial.print("[EVENT]: ");
  Serial.println(event->event_code, HEX);

  for (int i = 0; i < event->param_len; i++)
  {
    Serial.print(event->parameter[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  vTaskDelay(100);
}

void setup()
{
  Wire.begin(21, 22, 100000);
  swSerial.begin(115200);
  
  Serial.begin(9600);
  Serial.println();
  Serial.println("running setup");
  bm83.setCallback(onEventCallback);


  /* Create the semaphore to guard a shared resource.  As we are using
    the semaphore for mutual exclusion we create a mutex semaphore
    rather than a binary semaphore. */
  xSemaphore = xSemaphoreCreateBinary();

  // wen "CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE" bruchsch wird glaub de speicher selber zeugwise
  xTaskCreate(bm83_setup, "bm83_setup", 4000, NULL, 4, &xHandle);
  xTaskCreate(bm83_loop, "bm83_loop", 4000, NULL, 1, &xHandle);
  xTaskCreate(read_print_fuelgauge, "read_print_fuelgauge", 2048, NULL, 2, &xHandle);
  xTaskCreate(sound_proc_setup, "sound_proc_setup", 2048, NULL, 3, &xHandle);



  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  

  



  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
    //  If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    while (1)
      ;
  }
  Serial.println("Connected to BQ27441!");
  int fuelgauge_init = 1;


  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}
