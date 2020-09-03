//<File !Start!>
// FILE: [displaytestnew_GSLC.h]
// Created by GUIslice Builder version: [0.15.0]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XCheckbox.h"
#include "elem/XRingGauge.h"
#include "elem/XSlider.h"
#include "elem/XTextbox.h"
#include "elem/XTogglebtn.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if !defined(DRV_DISP_TFT_ESPI)
  #error Project tab->Target Platform should be arduino
#endif
#include <TFT_eSPI.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG2};
enum {E_ELEM_BTN1,E_ELEM_BTN2,E_ELEM_BTN4,E_ELEM_BTN5,E_ELEM_BTN6
      ,E_ELEM_BTN7,E_ELEM_CHECK1,E_ELEM_CHECK2,E_ELEM_CHECK3
      ,E_ELEM_CHECK4,E_ELEM_RINGGAUGE1,E_ELEM_RINGGAUGE2,E_ELEM_TEXT1
      ,E_ELEM_TEXT10,E_ELEM_TEXT11,E_ELEM_TEXT12,E_ELEM_TEXT13
      ,E_ELEM_TEXT14,E_ELEM_TEXT2,E_ELEM_TEXT4,E_ELEM_TEXT5
      ,E_ELEM_TEXT6,E_ELEM_TEXT7,E_ELEM_TEXT8,E_ELEM_TEXT9
      ,E_ELEM_TEXTBOX2,E_ELEM_TOGGLE2,E_ELEM_TOGGLE3,E_ELEM_TOGGLE4
      ,E_ELEM_TOGGLE5,E_ELEM_TOGGLE6,E_ELEM_TOGGLE8,E_TXTSCROLL1};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_TI_5X8PT7B,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 27 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG2 7 // # Elems total on page
#define MAX_ELEM_PG2_RAM MAX_ELEM_PG2 // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG2_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG2];
gslc_tsXRingGauge               m_sXRingGauge1;
gslc_tsXRingGauge               m_sXRingGauge2;
gslc_tsXTogglebtn               m_asXToggle2;
gslc_tsXTogglebtn               m_asXToggle3;
gslc_tsXTogglebtn               m_asXToggle4;
gslc_tsXTogglebtn               m_asXToggle5;
gslc_tsXTogglebtn               m_asXToggle6;
gslc_tsXTogglebtn               m_asXToggle8;
gslc_tsXCheckbox                m_asXCheck1;
gslc_tsXCheckbox                m_asXCheck2;
gslc_tsXCheckbox                m_asXCheck3;
gslc_tsXCheckbox                m_asXCheck4;
gslc_tsXTextbox                 m_sTextbox2;
char                            m_acTextboxBuf2[168]; // NRows=6 NCols=28
gslc_tsXSlider                  m_sTextScroll2;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemTextbox1_2;
extern gslc_tsElemRef* m_pElemToggle2;
extern gslc_tsElemRef* m_pElemToggle2_3;
extern gslc_tsElemRef* m_pElemToggle2_3_4;
extern gslc_tsElemRef* m_pElemToggle2_3_4_5;
extern gslc_tsElemRef* m_pElemToggle2_3_4_5_6;
extern gslc_tsElemRef* m_pElemToggle2_8;
extern gslc_tsElemRef* m_pElemXRingGauge1;
extern gslc_tsElemRef* m_pElemXRingGauge2;
extern gslc_tsElemRef* m_pTextSlider1;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_TI_5X8PT7B,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG2,m_asPage2Elem,MAX_ELEM_PG2_RAM,m_asPage2ElemRef,MAX_ELEM_PG2);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  

  // Create ring gauge E_ELEM_RINGGAUGE1 
  static char m_sRingText1[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_RINGGAUGE1,E_PG_MAIN,&m_sXRingGauge1,
          (gslc_tsRect){110,30,70,70},
          (char*)m_sRingText1,11,E_TI_5X8PT7B);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 20);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 0); // Set initial value
  m_pElemXRingGauge1 = pElemRef;

  // Create ring gauge E_ELEM_RINGGAUGE2 
  static char m_sRingText2[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_RINGGAUGE2,E_PG_MAIN,&m_sXRingGauge2,
          (gslc_tsRect){10,30,70,70},
          (char*)m_sRingText2,11,E_TI_5X8PT7B);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 20);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 0); // Set initial value
  m_pElemXRingGauge2 = pElemRef;
  
  // Create E_ELEM_TEXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT1,E_PG_MAIN,(gslc_tsRect){10,10,78,10},
    (char*)"Volume (Enc1)",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_MAIN,(gslc_tsRect){108,10,30,10},
    (char*)"Menu (Enc 2)",0,E_TI_5X8PT7B);
  
  // create E_ELEM_BTN1 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN1,E_PG_MAIN,
    (gslc_tsRect){10,190,70,40},(char*)"Settings",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN2 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN2,E_PG_MAIN,
    (gslc_tsRect){85,190,70,40},(char*)"Bluetooth",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN4 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN4,E_PG_MAIN,
    (gslc_tsRect){235,190,70,40},(char*)"Next Page",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // Create toggle button E_ELEM_TOGGLE2
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE2,E_PG_MAIN,&m_asXToggle2,
    (gslc_tsRect){253,25,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2 = pElemRef;
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,E_PG_MAIN,(gslc_tsRect){250,10,60,10},
    (char*)"Enable ALL",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE3
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE3,E_PG_MAIN,&m_asXToggle3,
    (gslc_tsRect){28,135,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2_3 = pElemRef;
  
  // Create toggle button E_ELEM_TOGGLE4
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE4,E_PG_MAIN,&m_asXToggle4,
    (gslc_tsRect){103,135,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2_3_4 = pElemRef;
  
  // Create toggle button E_ELEM_TOGGLE5
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE5,E_PG_MAIN,&m_asXToggle5,
    (gslc_tsRect){178,135,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2_3_4_5 = pElemRef;
  
  // Create toggle button E_ELEM_TOGGLE6
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE6,E_PG_MAIN,&m_asXToggle6,
    (gslc_tsRect){253,135,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2_3_4_5_6 = pElemRef;
  
  // Create E_ELEM_TEXT5 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT5,E_PG_MAIN,(gslc_tsRect){30,115,30,10},
    (char*)"LED 1",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT6 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT6,E_PG_MAIN,(gslc_tsRect){105,115,30,10},
    (char*)"LED 2",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT7 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT7,E_PG_MAIN,(gslc_tsRect){180,115,30,10},
    (char*)"LED 3",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT8 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT8,E_PG_MAIN,(gslc_tsRect){255,115,30,10},
    (char*)"LED 4",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT9 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT9,E_PG_MAIN,(gslc_tsRect){250,70,60,10},
    (char*)"Main LED",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE8
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE8,E_PG_MAIN,&m_asXToggle8,
    (gslc_tsRect){253,85,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2_8 = pElemRef;
  
  // Create E_ELEM_TEXT10 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT10,E_PG_MAIN,(gslc_tsRect){20,165,18,10},
    (char*)"BT1",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT11 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT11,E_PG_MAIN,(gslc_tsRect){95,165,18,10},
    (char*)"BT2",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT12 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT12,E_PG_MAIN,(gslc_tsRect){170,165,18,10},
    (char*)"BT3",0,E_TI_5X8PT7B);
  
  // Create E_ELEM_TEXT13 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT13,E_PG_MAIN,(gslc_tsRect){245,165,18,10},
    (char*)"BT4",0,E_TI_5X8PT7B);
   
  // create checkbox E_ELEM_CHECK1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck1,
    (gslc_tsRect){40,160,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_ORANGE,false);
   
  // create checkbox E_ELEM_CHECK2
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK2,E_PG_MAIN,&m_asXCheck2,
    (gslc_tsRect){120,160,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_ORANGE,false);
   
  // create checkbox E_ELEM_CHECK3
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK3,E_PG_MAIN,&m_asXCheck3,
    (gslc_tsRect){190,160,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_ORANGE,false);
   
  // create checkbox E_ELEM_CHECK4
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK4,E_PG_MAIN,&m_asXCheck4,
    (gslc_tsRect){270,160,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_ORANGE,false);

  // -----------------------------------
  // PAGE: E_PG2
  
  
  // create E_ELEM_BTN5 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN5,E_PG2,
    (gslc_tsRect){10,190,70,40},(char*)"Settings",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN6 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN6,E_PG2,
    (gslc_tsRect){85,190,70,40},(char*)"Bluetooth",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN7 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN7,E_PG2,
    (gslc_tsRect){160,190,70,40},(char*)"Back",0,E_TI_5X8PT7B,&CbBtnCommon);
   
  // Create wrapping box for textbox E_ELEM_TEXTBOX2 and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG2,(gslc_tsRect){10,20,201,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create textbox
  pElemRef = gslc_ElemXTextboxCreate(&m_gui,E_ELEM_TEXTBOX2,E_PG2,&m_sTextbox2,
    (gslc_tsRect){10+2,20+4,201-23,150-7},E_TI_5X8PT7B,
    (char*)&m_acTextboxBuf2,6,28);
  gslc_ElemXTextboxWrapSet(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemTextbox1_2 = pElemRef;

  // Create vertical scrollbar for textbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_TXTSCROLL1,E_PG2,&m_sTextScroll2,
          (gslc_tsRect){10+201-21,20+4,20,150-8},0,100,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_GRAY);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pTextSlider1 = pElemRef;
  
  // Create E_ELEM_TEXT14 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT14,E_PG2,(gslc_tsRect){15,5,108,10},
    (char*)"Charger Board Data",0,E_TI_5X8PT7B);
//<InitGUI !End!>

//<Startup !Start!>
  gslc_GuiRotate(&m_gui, 3);
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
