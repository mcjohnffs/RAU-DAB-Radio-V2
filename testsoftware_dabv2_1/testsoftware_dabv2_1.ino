#include "esp_system.h"           // ESP Funkionen
#include "BM83.h"                 //Bluetooth Module BM83 Library
#include <SoftwareSerial.h>       //Funktionierende SoftwareSerial Library für ESP32
#include <BD37544FS.h>            //Sound Prozessor Library
#include "Wire.h"                 //I2C Standart library
#include <SparkFunBQ27441.h>      //Fuel Gauge library
#include <DFRobot_MCP23017.h>     // MCP23017 GPIO Port Expander Library
#include "AiEsp32RotaryEncoder.h" //Encoder Library für ESP32
#include "displaytestnew_GSLC.h"  //Guislice Konfigurations Header

BD37544FS bd;                     // Sound Processor Haupt-Funktion Initialisierung
DFRobot_MCP23017 mcp(Wire, 0x20); // MCP23017 Port Expander Initialisierung

// FreeRTOS Handles
SemaphoreHandle_t xSemaphore;
TaskHandle_t xHandle;

//Encoder 1+2 - Pins und Initialisierung
#define ROTARY_ENCODER1_A_PIN 39
#define ROTARY_ENCODER1_B_PIN 36
#define ROTARY_ENCODER1_BUTTON_PIN 34

#define ROTARY_ENCODER2_A_PIN 32
#define ROTARY_ENCODER2_B_PIN 35
#define ROTARY_ENCODER2_BUTTON_PIN 33

AiEsp32RotaryEncoder rotaryEncoder1 = AiEsp32RotaryEncoder(ROTARY_ENCODER1_A_PIN, ROTARY_ENCODER1_B_PIN, ROTARY_ENCODER1_BUTTON_PIN, -1);
AiEsp32RotaryEncoder rotaryEncoder2 = AiEsp32RotaryEncoder(ROTARY_ENCODER2_A_PIN, ROTARY_ENCODER2_B_PIN, ROTARY_ENCODER2_BUTTON_PIN, -1);

// Setzen der gesamten Kapazität des Akkus/der Batterie mithilfe "BATTERY_CAPACITY" in mAh
const unsigned int BATTERY_CAPACITY = 9000; // e.g. 850mAh battery

//BM83 Uart Pins
#define RX_PIN 16 //SWSerial RX_PIN
#define TX_PIN 17 //SWSerial TX_PIN
#define TX_IND 5  //SWSerial TX_IND

const int mfbPin = 4; //BM83 "Enable" Pin

int fuelgauge_init = 0; // Fuel Gauge Init Variable

// BM83 Uart SoftwareSerial Initialisierung
SoftwareSerial swSerial(RX_PIN, TX_PIN);
BM83 bm83(swSerial, TX_IND);

#define led2Pin 18    // S2 LED Pin
int buttonState2 = 0; // S2 Button Pin Status Variable
#define led3Pin 19    // S3 LED Pin
int buttonState3 = 0; // S3 Button Pin Status Variable
#define led4Pin 26    // S4 LED Pin
int buttonState4 = 0; // S4 Button Pin Status Variable
#define led5Pin 27    // S5 LED Pin
int buttonState5 = 0; // S5 Button Pin Status Variable

// Display GUI Initialisierung START

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef *m_pElemTextbox3 = NULL;
gslc_tsElemRef *m_pElemToggle2 = NULL;
gslc_tsElemRef *m_pElemToggle2_3 = NULL;
gslc_tsElemRef *m_pElemToggle2_3_4 = NULL;
gslc_tsElemRef *m_pElemToggle2_3_4_5 = NULL;
gslc_tsElemRef *m_pElemToggle2_3_4_5_6 = NULL;
gslc_tsElemRef *m_pElemToggle2_8 = NULL;
gslc_tsElemRef *m_pElemToggle2_8_9_10 = NULL;
gslc_tsElemRef *m_pElemXRingGauge1 = NULL;
gslc_tsElemRef *m_pElemXRingGauge2 = NULL;
gslc_tsElemRef *m_pTextSlider3 = NULL;
//<Save_References !End!>

// Message Debug Funktion
static int16_t DebugOut(char ch)
{
  if (ch == (char)'\n')
    Serial.println("");
  else
    Serial.write(ch);
  return 0;
}

// ------------------------------------------------
// Callback Methoden
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void *pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui *pGui = (gslc_tsGui *)(pvGui);
  gslc_tsElemRef *pElemRef = (gslc_tsElemRef *)(pvElemRef);
  gslc_tsElem *pElem = gslc_GetElemFromRef(pGui, pElemRef);

  if (eTouch == GSLC_TOUCH_UP_IN)
  {
    // Mithilfe von element's ID können wir übeprüfen welcher Button gedrückt worden ist
    switch (pElem->nId)
    {
      //<Button Enums !Start!>
    case E_ELEM_BTN1:

      break;
    case E_ELEM_BTN2:
      bm83.mmiAction(BM83_MMI_STANDBY_ENTERING_PAIRING);
      break;
    case E_ELEM_BTN4:

      gslc_SetPageCur(&m_gui, E_PG2);
      break;
    case E_ELEM_TOGGLE2:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2))
      {
        ;
      }
      break;
    case E_ELEM_TOGGLE3:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3) == 1)
      {
        ledcWrite(0, 1024);
      }
      ledcWrite(0, 0);
      break;
    case E_ELEM_TOGGLE4:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4) == 1)
      {
        ledcWrite(1, 1024);
      }
      ledcWrite(1, 0);
      break;
    case E_ELEM_TOGGLE5:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4_5) == 1)
      {
        ledcWrite(2, 1024);
      }
      ledcWrite(2, 0);
      break;
    case E_ELEM_TOGGLE6:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_3_4_5_6) == 1)
      {
        ledcWrite(3, 1024);
      }
      ledcWrite(3, 0);
      break;
    case E_ELEM_TOGGLE8:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_8))
      {
        ;
      }

      break;
    case E_ELEM_BTN5:
      break;
    case E_ELEM_BTN6:
      break;
    case E_ELEM_BTN7:
      gslc_SetPageCur(&m_gui, E_PG_MAIN);
      break;
    case E_ELEM_TOGGLE10:
      if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_8_9_10))
      {

        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
      }
      ledcWrite(0, 1024);
      ledcWrite(1, 1024);
      ledcWrite(2, 1024);
      ledcWrite(3, 1024);
      break;
    case E_ELEM_BTN11:
      gslc_SetPageCur(&m_gui, E_PG3);
      break;
    case E_ELEM_BTN8:
      break;
    case E_ELEM_BTN9:
      break;
    case E_ELEM_BTN10:
      gslc_SetPageCur(&m_gui, E_PG2);
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
//<Listbox Enums !Start!>
//<Listbox Enums !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Enums !Start!>
//<Slider Enums !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

// Display GUI Initialisierung ENDE

void bm83_loop(void *pvParameters)
{
  while (1)
  {

    bm83.run();

    vTaskDelete(NULL);
  }
}

void mcp23017_buttons(void *pvParameters)
{
  while (1)
  {
    gslc_Update(&m_gui);

    uint8_t value2 = mcp.digitalRead(/*pin = */ mcp.eGPA2);
    uint8_t value3 = mcp.digitalRead(/*pin = */ mcp.eGPA3);
    uint8_t value4 = mcp.digitalRead(/*pin = */ mcp.eGPA4);
    uint8_t value5 = mcp.digitalRead(/*pin = */ mcp.eGPA5);

    if (value2 == LOW)
    {
      Serial.println("Button 2 pressed. Configuring Sound Processor!");

      bd.setSelect(0);  // int 0...7 === A B C D E F INPUT_SHORT INPUT_MUTE
      bd.setIn_gain(0); // int 0...7 === 0...20 dB
      bd.setVol_1(0);   // int 0...87 === 0...-87 dB
      bd.setFad_1(0);   // int 0...87 === 0...-87 dB
      bd.setFad_2(0);   // int 0...87 === 0...-87 dB
      bd.setBass(0);    // int -7...0...+7 === -14...+14 dB
      bd.setMidd(0);    // int -7...0...+7 === -14...+14 dB
      bd.setTreb(0);    // int -7...0...+7 === -14...+14 dB
      Serial.println("Setup p complete!");

      ledcWrite(0, 1024);
      delay(3);
      ledcWrite(0, 0);
      delay(3);
      ledcWrite(0, 1024);
      delay(3);
      ledcWrite(0, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }

    if (value3 == LOW)
    {
      Serial.println("Button 3 pressed!");

      ledcWrite(1, 1024);
      delay(3);
      ledcWrite(1, 0);
      delay(3);
      ledcWrite(1, 1024);
      delay(3);
      ledcWrite(1, 0);
      delay(200);
    }

    else
    {
      //Serial.println("Button release!");
    }

    if (value4 == LOW)
    {
      Serial.println("Button 4 pressed!");

      ledcWrite(2, 1024);
      delay(3);
      ledcWrite(2, 0);
      delay(3);
      ledcWrite(2, 1024);
      delay(3);
      ledcWrite(2, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }

    if (value5 == LOW)
    {
      Serial.println("Button 5 pressed!");

      ledcWrite(3, 1024);
      delay(3);
      ledcWrite(3, 0);
      delay(3);
      ledcWrite(3, 1024);
      delay(3);
      ledcWrite(3, 0);
      delay(200);
    }
    else
    {
      //Serial.println("Button release!");
    }
    vTaskDelay(10);
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
    }

    if (encoderDelta2 != 0)
    {

      Serial.print("Value 2: ");
      Serial.println(encoderValue2);

      gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge2, encoderValue2);
    }

    vTaskDelay(2);
  }
}
void loop() {

  
  
}

void read_print_fuelgauge(void *pvParameters)
{
  while (1)
  {
      xSemaphoreTake(xSemaphore, 30);
    Serial.println("read_print_fuelgauge: Using Semaphore to read Charger Board Values....");

    // Auslesen der Daten des LiPo Charger IC's - BQ27441-G1A
    unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
    unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
    int current = lipo.current(AVG);                 // Read average current (mA)
    unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
    unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
    int power = lipo.power();                        // Read average power draw (mW)
    int health = lipo.soh();                         // Read state-of-health (%)

    // Multiple Chars zu einem String hinzufügen
    //String toPrint = String(soc) + "% | ";
    //toPrint += String(volts) + " mV | ";
    //toPrint += String(current) + " mA | ";
    //toPrint += String(capacity) + " / ";
    //toPrint += String(fullCapacity) + " mAh | ";
    //toPrint += String(power) + " mW | ";
    //toPrint += String(health) + "%";

    char str1[20];
    char str2[20];
    char str3[20];
    char str4[20];
    char str5[20];
    char str6[20];

    sprintf(str1, "%d\n", soc);
    sprintf(str2, "%d\n", volts);
    sprintf(str3, "%d\n", current);
    sprintf(str4, "%d", capacity);
    sprintf(str5, "%d\n", fullCapacity);
    sprintf(str6, "%d\n", power);

    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str1);
    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str2);
    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str3);
    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str4);
    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str5);
    gslc_ElemXTextboxAdd(&m_gui, m_pElemTextbox3, str6);

    gslc_Update(&m_gui);

    Serial.println("read_print_fuelgauge: Reading complete...returning Semaphore");
    xSemaphoreGive(xSemaphore);
    vTaskDelay(20000);
  }
}
void bm83_setup(void *pvParameters)
{
  xSemaphoreTake(xSemaphore, 30);
  Serial.println("bm83_setup: Using Semaphore");

  Serial.println("Setting up BM83");

  //BM83 start routine (required)

  pinMode(mfbPin, OUTPUT);    // sets the MFB Pin 4 as output
  digitalWrite(mfbPin, HIGH); // sets the MFB Pin 4 "High" to power on BM83 over BAT_IN
  delay(10);
  bm83.powerOn(); // Sends "power on" command over UART to BM83

  digitalWrite(mfbPin, LOW); // sets the MFB Pin 4 "LOW" (no longer needed after power on process)
  Serial.println("bm83_setup: BM83 Setup complete...");
  xSemaphoreGive(xSemaphore);
  Serial.println("bm83_setup: Returning Semaphore...");
  vTaskDelete(NULL);
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
  // xTaskCreatePinnedToCore Core Auswahl

  xSemaphore = xSemaphoreCreateBinary();

  // Setup Tasks
  xTaskCreatePinnedToCore(bm83_setup, "bm83_setup", 4000, NULL, 3, &xHandle, 0);

  //Loop Tasks
  xTaskCreatePinnedToCore(bm83_loop, "bm83_loop", 4000, NULL, 2, &xHandle, 0);
  xTaskCreatePinnedToCore(read_print_fuelgauge, "read_print_fuelgauge", 6048, NULL, 1, &xHandle, 1);
  xTaskCreatePinnedToCore(encoder_loop, "encoder_loop", 4096, NULL, 3, &xHandle, 1);
  xTaskCreatePinnedToCore(mcp23017_buttons, "mcp23017_buttons", 4096, NULL, 4, &xHandle, 1);

  // Konfiguration der PWM Channels
  for (int i = 0; i < 4; i++)
  {
    ledcSetup(i, 2000, 8); //2 kHz , 8 bit
  }
  ledcAttachPin(led2Pin, 0);
  ledcAttachPin(led3Pin, 1);
  ledcAttachPin(led4Pin, 2);
  ledcAttachPin(led5Pin, 3);

  ledcWrite(0, 0); // 0 = LED On ; 1024 = LED Off
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);

  rotaryEncoder1.begin();
  rotaryEncoder1.setup([] { rotaryEncoder1.readEncoder_ISR(); });
  rotaryEncoder1.setBoundaries(0, 20, false);

  rotaryEncoder2.begin();
  rotaryEncoder2.setup([] { rotaryEncoder2.readEncoder_ISR(); });
  rotaryEncoder2.setBoundaries(0, 20, false);

  // Initialisierung der Buttons als Inputs

  while (mcp.begin() != 0)
  {
    Serial.println("Fehler: Initialisierung des Port Expanders(MCP23017) fehlgeschlagen, bitte ueberpruefen Sie die Verbinungen zum IC!");
  }

  mcp.pinMode(/*pin = */ mcp.eGPA2, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA3, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA4, /*mode = */ INPUT);
  mcp.pinMode(/*pin = */ mcp.eGPA5, /*mode = */ INPUT);



  // Initialisierung des Charger-Boards mithilfe lipo.begin()
  if (!lipo.begin()) // begin() = true wenn Verbindung zum IC erfolgreich war
  {
    //  If communication fails, print an error message and loop forever.
    Serial.println("Fehler: Initialisierung des Ladechips(BQ27441-G1A) fehlgeschlagen, bitte ueberpruefen Sie die Verbinungen zum IC!");
    while (1)
      ;
  }
  Serial.println("Verbindung zum Ladechip(BQ27441-G1A) erfolgreich");
  int fuelgauge_init = 1;

  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}
