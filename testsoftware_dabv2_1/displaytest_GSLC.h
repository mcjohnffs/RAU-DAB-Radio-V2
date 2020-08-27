//<File !Start!>
// FILE: [displaytest_GSLC.h]
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
#include "elem/XListbox.h"
#include "elem/XRingGauge.h"
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
enum {E_PG_MAIN,E_PG3};
enum {E_ELEM_BTN1,E_ELEM_BTN10,E_ELEM_BTN11,E_ELEM_BTN12,E_ELEM_BTN2
      ,E_ELEM_BTN3,E_ELEM_BTN4,E_ELEM_BTN9,E_ELEM_LISTBOX2
      ,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_RADIO3,E_ELEM_RADIO4
      ,E_ELEM_RINGGAUGE3,E_ELEM_RINGGAUGE4,E_ELEM_TEXT2,E_ELEM_TEXT3
      ,E_ELEM_TEXT4,E_ELEM_TEXT5,E_ELEM_TEXT6,E_ELEM_TEXT7,E_ELEM_TEXT8
      ,E_ELEM_TEXT9,E_ELEM_TEXTBOX2,E_ELEM_TOGGLE2,E_ELEM_TOGGLE3
      ,E_ELEM_TOGGLE4,E_ELEM_TOGGLE5,E_ELEM_TOGGLE6};
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

#define MAX_ELEM_PG_MAIN 24 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG3 5 // # Elems total on page
#define MAX_ELEM_PG3_RAM MAX_ELEM_PG3 // # Elems in RAM
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
gslc_tsElem                     m_asPage3Elem[MAX_ELEM_PG3_RAM];
gslc_tsElemRef                  m_asPage3ElemRef[MAX_ELEM_PG3];
gslc_tsXTogglebtn               m_asXToggle2;
gslc_tsXRingGauge               m_sXRingGauge3;
gslc_tsXRingGauge               m_sXRingGauge4;
gslc_tsXTogglebtn               m_asXToggle3;
gslc_tsXTogglebtn               m_asXToggle4;
gslc_tsXTogglebtn               m_asXToggle5;
gslc_tsXTogglebtn               m_asXToggle6;
gslc_tsXCheckbox                m_asXRadio1;
gslc_tsXCheckbox                m_asXRadio2;
gslc_tsXCheckbox                m_asXRadio3;
gslc_tsXCheckbox                m_asXRadio4;
gslc_tsXTextbox                 m_sTextbox2;
char                            m_acTextboxBuf2[4]; // NRows=1 NCols=4
gslc_tsXListbox                 m_sListbox2;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf2[41 + XLISTBOX_BUF_OH_R];

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemListbox1_2;
extern gslc_tsElemRef* m_pElemTextbox2;
extern gslc_tsElemRef* m_pElemToggle2;
extern gslc_tsElemRef* m_pElemToggle3;
extern gslc_tsElemRef* m_pElemToggle3_4;
extern gslc_tsElemRef* m_pElemToggle3_5;
extern gslc_tsElemRef* m_pElemToggle3_6;
extern gslc_tsElemRef* m_pElemXRingGauge3;
extern gslc_tsElemRef* m_pElemXRingGauge3_4;
extern gslc_tsElemRef* m_pListSlider1;
extern gslc_tsElemRef* m_pTextSlider2;
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
  gslc_PageAdd(&m_gui,E_PG3,m_asPage3Elem,MAX_ELEM_PG3_RAM,m_asPage3ElemRef,MAX_ELEM_PG3);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // create E_ELEM_BTN1 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN1,E_PG_MAIN,
    (gslc_tsRect){10,190,70,40},(char*)"Settings",0,E_TI_5X8PT7B,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK3,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  
  // create E_ELEM_BTN2 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN2,E_PG_MAIN,
    (gslc_tsRect){160,190,70,40},(char*)"Back",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN3 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN3,E_PG_MAIN,
    (gslc_tsRect){85,190,70,40},(char*)"Bluetooth",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN4 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN4,E_PG_MAIN,
    (gslc_tsRect){235,190,70,40},(char*)"Next Page",0,E_TI_5X8PT7B,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  
  // Create toggle button E_ELEM_TOGGLE2
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE2,E_PG_MAIN,&m_asXToggle2,
    (gslc_tsRect){250,30,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    false,false,&CbBtnCommon);
  m_pElemToggle2 = pElemRef;
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_MAIN,(gslc_tsRect){240,10,60,10},
    (char*)"Enable ALL",0,E_TI_5X8PT7B);

  // Create ring gauge E_ELEM_RINGGAUGE3 
  static char m_sRingText3[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_RINGGAUGE3,E_PG_MAIN,&m_sXRingGauge3,
          (gslc_tsRect){10,30,70,70},
          (char*)m_sRingText3,11,E_TI_5X8PT7B);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 20);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 0); // Set initial value
  m_pElemXRingGauge3 = pElemRef;
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,E_PG_MAIN,(gslc_tsRect){10,10,78,10},
    (char*)"Volume (Enc1)",0,E_TI_5X8PT7B);

  // Create ring gauge E_ELEM_RINGGAUGE4 
  static char m_sRingText4[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_RINGGAUGE4,E_PG_MAIN,&m_sXRingGauge4,
          (gslc_tsRect){90,30,70,70},
          (char*)m_sRingText4,11,E_TI_5X8PT7B);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 20);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 0); // Set initial value
  m_pElemXRingGauge3_4 = pElemRef;
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,E_PG_MAIN,(gslc_tsRect){108,10,30,10},
    (char*)"Enc 2",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE3
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE3,E_PG_MAIN,&m_asXToggle3,
    (gslc_tsRect){28,155,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    true,false,&CbBtnCommon);
  m_pElemToggle3 = pElemRef;
  
  // Create E_ELEM_TEXT5 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT5,E_PG_MAIN,(gslc_tsRect){29,129,18,10},
    (char*)"SW2",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE4
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE4,E_PG_MAIN,&m_asXToggle4,
    (gslc_tsRect){103,155,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    true,false,&CbBtnCommon);
  m_pElemToggle3_4 = pElemRef;
  
  // Create E_ELEM_TEXT6 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT6,E_PG_MAIN,(gslc_tsRect){104,129,18,10},
    (char*)"SW3",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE5
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE5,E_PG_MAIN,&m_asXToggle5,
    (gslc_tsRect){178,155,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    true,false,&CbBtnCommon);
  m_pElemToggle3_5 = pElemRef;
  
  // Create E_ELEM_TEXT7 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT7,E_PG_MAIN,(gslc_tsRect){179,129,18,10},
    (char*)"SW4",0,E_TI_5X8PT7B);
  
  // Create toggle button E_ELEM_TOGGLE6
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE6,E_PG_MAIN,&m_asXToggle6,
    (gslc_tsRect){253,155,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK1,GSLC_COL_GRAY_LT3,
    true,false,&CbBtnCommon);
  m_pElemToggle3_6 = pElemRef;
  
  // Create E_ELEM_TEXT8 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT8,E_PG_MAIN,(gslc_tsRect){254,129,18,10},
    (char*)"SW5",0,E_TI_5X8PT7B);
  
  // Create radio button E_ELEM_RADIO1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,&m_asXRadio1,
    (gslc_tsRect){70,150,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  
  // Create radio button E_ELEM_RADIO2
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,&m_asXRadio2,
    (gslc_tsRect){145,150,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_GREEN_LT1,false);
  
  // Create radio button E_ELEM_RADIO3
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO3,E_PG_MAIN,&m_asXRadio3,
    (gslc_tsRect){295,150,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_YELLOW,false);
  
  // Create radio button E_ELEM_RADIO4
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO4,E_PG_MAIN,&m_asXRadio4,
    (gslc_tsRect){220,150,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_MAGENTA,false);
   
  // Create textbox
  pElemRef = gslc_ElemXTextboxCreate(&m_gui,E_ELEM_TEXTBOX2,E_PG_MAIN,&m_sTextbox2,
    (gslc_tsRect){260,90,57,23},E_TI_5X8PT7B,
    (char*)&m_acTextboxBuf2,1,4);
  gslc_ElemXTextboxWrapSet(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemTextbox2 = pElemRef;
  
  // Create E_ELEM_TEXT9 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT9,E_PG_MAIN,(gslc_tsRect){260,70,30,10},
    (char*)"Strom",0,E_TI_5X8PT7B);

  // -----------------------------------
  // PAGE: E_PG3
  
  
  // create E_ELEM_BTN9 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN9,E_PG3,
    (gslc_tsRect){5,190,70,40},(char*)"test1",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN10 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN10,E_PG3,
    (gslc_tsRect){85,190,70,40},(char*)"test2",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN11 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN11,E_PG3,
    (gslc_tsRect){245,190,70,40},(char*)"test3",0,E_TI_5X8PT7B,&CbBtnCommon);
  
  // create E_ELEM_BTN12 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN12,E_PG3,
    (gslc_tsRect){165,190,70,40},(char*)"test4",0,E_TI_5X8PT7B,&CbBtnCommon);
   
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_ELEM_LISTBOX2,E_PG3,&m_sListbox2,
    (gslc_tsRect){10,10,300,180},E_TI_5X8PT7B,
    (uint8_t*)&m_acListboxBuf2,sizeof(m_acListboxBuf2),0);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 5, 1); // 5 rows, 1 columns
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Bluetooth");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Volume");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Einstellungen");
  gslc_ElemXListboxAddItem(&m_gui, pElemRef, "Next Page");
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemListbox1_2 = pElemRef;
//<InitGUI !End!>

//<Startup !Start!>
  gslc_GuiRotate(&m_gui, 3);
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
