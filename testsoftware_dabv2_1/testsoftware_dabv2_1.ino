#include "esp_system.h"
#include "BM83.h"            // Bluetooth Module BM83 Library
#include <SoftwareSerial.h>  //SoftwareSerial Library for ESP32
#include <APA102.h>          //RGB LED's Library
#include <BD37544FS.h>       //Sound processor Library
#include "Wire.h"            //i2c library
#include <SparkFunBQ27441.h> //fuel gauge library
#include <DFRobot_MCP23017.h>
#include "AiEsp32RotaryEncoder.h"
#include "displaytestnew_GSLC.h"

DFRobot_MCP23017 mcp(Wire,0x20);

#define ROTARY_ENCODER1_A_PIN 39
#define ROTARY_ENCODER1_B_PIN 36
#define ROTARY_ENCODER1_BUTTON_PIN 34

#define ROTARY_ENCODER2_A_PIN 32
#define ROTARY_ENCODER2_B_PIN 35
#define ROTARY_ENCODER2_BUTTON_PIN 33

AiEsp32RotaryEncoder rotaryEncoder1 = AiEsp32RotaryEncoder(ROTARY_ENCODER1_A_PIN, ROTARY_ENCODER1_B_PIN, ROTARY_ENCODER1_BUTTON_PIN, -1);
AiEsp32RotaryEncoder rotaryEncoder2 = AiEsp32RotaryEncoder(ROTARY_ENCODER2_A_PIN, ROTARY_ENCODER2_B_PIN, ROTARY_ENCODER2_BUTTON_PIN, -1);

// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 9000; // e.g. 850mAh battery

BD37544FS bd;

#define RX_PIN 16
#define TX_PIN 17
#define TX_IND 5

const int mfbPin = 4;

int fuelgauge_init = 0;

const int button = 0;        //gpio to use to trigger delay
const int wdtTimeout = 3000; //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

SemaphoreHandle_t xSemaphore;
TaskHandle_t xHandle;

// constants won't change. They're used here to set pin numbers:

#define led2Pin 18

int buttonState2 = 0;

#define led3Pin 19   
int buttonState3 = 0;


#define led4Pin 26    
int buttonState4 = 0;

#define led5Pin 27    
int buttonState5 = 0;

// setting PWM properties
const int freq = 2000;
const int ledChannel = 0;
const int resolution = 8;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemToggle2    = NULL;
gslc_tsElemRef* m_pElemToggle2_3  = NULL;
gslc_tsElemRef* m_pElemToggle2_3_4= NULL;
gslc_tsElemRef* m_pElemToggle2_3_4_5= NULL;
gslc_tsElemRef* m_pElemToggle2_3_4_5_6= NULL;
gslc_tsElemRef* m_pElemToggle2_8  = NULL;
gslc_tsElemRef* m_pElemXRingGauge1= NULL;
gslc_tsElemRef* m_pElemXRingGauge2= NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }



// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_BTN1:

        break;
      case E_ELEM_BTN2:
      bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING);
        break;
      case E_ELEM_BTN4:
        break;
      case E_ELEM_TOGGLE2:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2)) {
          ;
        }
        break;
      case E_ELEM_TOGGLE3:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3)) {
          ;
        }
        break;
      case E_ELEM_TOGGLE4:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4)) {
          ;
        }
        break;
      case E_ELEM_TOGGLE5:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4_5)) {
          ;
        }
        break;
      case E_ELEM_TOGGLE6:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4_5_6)) {
          ;
        }
        break;
      case E_ELEM_TOGGLE8:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_8)) {
          ;
        }
        break;
      case E_ELEM_BTN5:
        break;
      case E_ELEM_BTN6:
        break;
      case E_ELEM_BTN7:
        break;

//<Button Enums !End!>
    default:
      break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>




void bm83_loop(void *pvParameters)
{
  while (1)
  {

    bm83.run();

    vTaskDelete(NULL);
  }
}





void mcp23017_buttons(void *pvParameters){
  while(1){

    gslc_Update(&m_gui);
    uint8_t value2 = mcp.digitalRead(/*pin = */mcp.eGPA2);
    uint8_t value3 = mcp.digitalRead(/*pin = */mcp.eGPA3);
    uint8_t value4 = mcp.digitalRead(/*pin = */mcp.eGPA4);
    uint8_t value5 = mcp.digitalRead(/*pin = */mcp.eGPA5);
  /*Read  level of Group GPIOA pins*/
  if(value2 == LOW){
      Serial.println("Button 2 pressed!");
    bd.setSelect(0);  // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
    bd.setIn_gain(0); // int 0...7 === 0...20 dB
    bd.setVol_1(0); // int 0...87 === 0...-87 dB
    bd.setFad_1(0);   // int 0...87 === 0...-87 dB
    bd.setFad_2(0);   // int 0...87 === 0...-87 dB
    bd.setBass(0);    // int -7...0...+7 === -14...+14 dB
    bd.setMidd(0);    // int -7...0...+7 === -14...+14 dB
    bd.setTreb(0);    // int -7...0...+7 === -14...+14 dB
    Serial.println("Setup p complete!");
      delay(200);
  }else{
      //Serial.println("Button release!");
  }

  if(value3 == LOW){
      Serial.println("Button 3 pressed!");
      delay(200);
  }else{
      //Serial.println("Button release!");
  }

    if(value4 == LOW){
      Serial.println("Button 4 pressed!");
      delay(200);
  }else{
      //Serial.println("Button release!");
  }

    if(value5 == LOW){
      Serial.println("Button 5 pressed!");
      delay(200);
  }else{
      //Serial.println("Button release!");
  }
  vTaskDelay(50);
  }
  }

void ledtest(void *pvParameters)
{
  while (1)
  {

    // configure LED PWM functionalitites
    ledcSetup(ledChannel, freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(led2Pin, ledChannel);
    ledcAttachPin(led3Pin, ledChannel);
    ledcAttachPin(led4Pin, ledChannel);
    ledcAttachPin(led5Pin, ledChannel);

    // increase the LED brightness
    for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
    {
      // changing the LED brightness with PWM
      ledcWrite(ledChannel, dutyCycle);
      delay(15);
    }

    // decrease the LED brightness
    for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
    {
      // changing the LED brightness with PWM
      ledcWrite(ledChannel, dutyCycle);
      delay(15);
    }

    vTaskDelay(1);
  }
}

void encoder_loop(void *pvParameters)
{

  while (1)
  {
    //first lets handle rotary encoder button click
    if (rotaryEncoder1.currentButtonState() == BUT_RELEASED || rotaryEncoder2.currentButtonState() == BUT_RELEASED)
    {
      //we can process it here or call separate function like:
    }

    //lets see if anything changed
    int8_t encoderDelta = rotaryEncoder1.encoderChanged();
    int8_t encoderDelta2 = rotaryEncoder2.encoderChanged();

    //optionally we can ignore whenever there is no change
    //if (encoderDelta == 0) return;

    //for some cases we only want to know if value is
    //increased or decreased (typically for menu items)
    if (encoderDelta > 0 || encoderDelta2 > 0)
    {

      Serial.print("+");
    }
    if (encoderDelta < 0 || encoderDelta2 < 0)
    {

      Serial.print("-");

      //for other cases we want to know what is current value.
      //Additionally often we only want if something changed
      //example: when using rotary encoder to set termostat temperature, or sound volume etc
    }

    int16_t encoderValue = rotaryEncoder1.readEncoder();
    int16_t encoderValue2 = rotaryEncoder2.readEncoder();
    //if value is changed compared to our last read
    if (encoderDelta != 0)
    {
      //now we need current value

      //process new value. Here is simple output.
      Serial.print("Value 1:  ");
      Serial.println(encoderValue);

      gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge1, encoderValue);
      bd.setVol_1(encoderValue);
      vTaskDelay(3);
      
    }

    if (encoderDelta2 != 0)
    {

      Serial.print("Value 2: ");
      Serial.println(encoderValue2);

      gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge2, encoderValue2);
      vTaskDelay(3);
    }

    
    vTaskDelay(1);
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
  bm83.powerOn(); // Sends "power on" command over UART to BM83

  digitalWrite(mfbPin, LOW); // sets the MFB Pin 4 "LOW" (no longer needed after power on process)
  Serial.println("BM83 Setup complete...");
  vTaskDelay(100);
  vTaskDelete(NULL);
}

void sound_proc_setup(void *pvParameters)
{
  while (1)
  {


    if (buttonState5 == HIGH)
    {
  

      Serial.println("Sound p Setup complete...");
    }
    vTaskDelay(10);
  }
}

void setup()
{
  Wire.begin(21, 22, 400000);
  swSerial.begin(115200);

  Serial.begin(9600);
  Serial.println();
  Serial.println("running setup");

  gslc_InitDebug(&DebugOut);
  InitGUIslice_gen();

  // possible use of "CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE"
  // xTaskCreatePinnedToCore Core Priority Auswahl
  
  xTaskCreatePinnedToCore(bm83_setup, "bm83_setup", 4000, NULL, 3, &xHandle,0);
  xTaskCreatePinnedToCore(bm83_loop, "bm83_loop", 4000, NULL, 2, &xHandle, 1);
  xTaskCreatePinnedToCore(read_print_fuelgauge, "read_print_fuelgauge", 2048, NULL, 2, &xHandle, 1);
  xTaskCreatePinnedToCore(sound_proc_setup, "sound_proc_setup", 2048, NULL, 3, &xHandle, 0);
  xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 4096, NULL, 1, &xHandle, 1);
  xTaskCreatePinnedToCore(ledtest, "ledtest", 4096, NULL, 1, &xHandle, 1);
  xTaskCreatePinnedToCore(mcp23017_buttons, "mcp23017_buttons", 4096, NULL, 3, &xHandle, 1);


  rotaryEncoder1.begin();
  rotaryEncoder1.setup([] { rotaryEncoder1.readEncoder_ISR(); });
  rotaryEncoder1.setBoundaries(0, 20, false);

  rotaryEncoder2.begin();
  rotaryEncoder2.setup([] { rotaryEncoder2.readEncoder_ISR(); });
  rotaryEncoder2.setBoundaries(0, 20, false);

  // initialize the pushbuttons and leds as inputs | outputs:



  while(mcp.begin() != 0){
    Serial.println("Initialization of the chip failed, please confirm that the chip connection is correct!");
    delay(1000);
  }

  mcp.pinMode(/*pin = */mcp.eGPA2, /*mode = */INPUT);
  mcp.pinMode(/*pin = */mcp.eGPA3, /*mode = */INPUT);
  mcp.pinMode(/*pin = */mcp.eGPA4, /*mode = */INPUT);
  mcp.pinMode(/*pin = */mcp.eGPA5, /*mode = */INPUT);

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
