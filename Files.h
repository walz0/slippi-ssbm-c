#ifndef FILES_H
#define FILES_H

#include "./m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct GUI_GameSetup {
  JOBJSet **jobjs;
  COBJDesc **cobjs;
  void **lights;
  void **fog;
} GUI_GameSetup;

typedef struct GUI_RstScreen {
  JOBJSet **jobjs;
  COBJDesc **cobjs;
  void **lights;
  void **fog;
} GUI_RstScreen;

typedef struct TestChar {
  JOBJSet **jobjs;
} TestChar;

typedef enum GUI_GameSetup_JOBJ {
  GUI_GameSetup_JOBJ_Background,
  GUI_GameSetup_JOBJ_Panels,
  GUI_GameSetup_JOBJ_Button,
  GUI_GameSetup_JOBJ_CSBoxSelector,
  GUI_GameSetup_JOBJ_CSIcon,
  GUI_GameSetup_JOBJ_X,
  GUI_GameSetup_JOBJ_FlatTexture,
  GUI_GameSetup_JOBJ_RightArrow,
  GUI_GameSetup_JOBJ_Check,
  GUI_GameSetup_JOBJ_CharDialog,
  GUI_GameSetup_JOBJ_StockIcon,
  GUI_GameSetup_JOBJ_TurnIndicator,
  GUI_GameSetup_JOBJ_GameResult,
} GUI_GameSetup_JOBJ;

typedef enum GUI_RstScreen_JOBJ {
  GUI_RstScreen_JOBJ_CSIcon,
  GUI_RstScreen_JOBJ_CharNames,
  GUI_RstScreen_JOBJ_StockIcon,
  GUI_RstScreen_JOBJ_GameResult,
  GUI_RstScreen_JOBJ_Thumbnails,
  GUI_RstScreen_JOBJ_Winner,
} GUI_RstScreen_JOBJ;

typedef enum TestChar_JOBJ {
  CHAR_JOBJ
} TestChar_JOBJ;



#endif