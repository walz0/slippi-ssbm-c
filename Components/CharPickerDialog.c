#include "CharPickerDialog.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

static void _SetPos(CharPickerDialog *cpd, Vec3 pos) {
  cpd->root_jobj->trans = pos;
  JOBJ_SetMtxDirtySub(cpd->root_jobj);
}

static void _InputsThink(GOBJ *gobj) {
  CharPickerDialog *cpd = gobj->userdata;
  if (!cpd->state.is_open) {
    return;
  }

  u8 port = R13_U8(-0x5108);
  u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent
  u64 downInputs = Pad_GetDown(port);

  if (downInputs & HSD_BUTTON_A) {
    cpd->on_select(cpd);
    return;
  }

  if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
    // Handle a right input
    if (cpd->state.char_selection_idx >= CKIND_GANONDORF) {
      cpd->state.char_selection_idx = CKIND_YOUNGLINK;
    } else if ((cpd->state.char_selection_idx + 1) % 7 == 0) {
      cpd->state.char_selection_idx -= 6;
    } else {
      cpd->state.char_selection_idx++;
    }
  } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
    // Handle a left input
    if (cpd->state.char_selection_idx == CKIND_YOUNGLINK) {
      cpd->state.char_selection_idx = CKIND_GANONDORF;
    } else if (cpd->state.char_selection_idx % 7 == 0) {
      cpd->state.char_selection_idx += 6;
    } else {
      cpd->state.char_selection_idx--;
    }
  } else if (scrollInputs & (HSD_BUTTON_DOWN | HSD_BUTTON_DPAD_DOWN)) {
    // Handle a down input
    if (cpd->state.char_selection_idx % 7 >= 5 && cpd->state.char_selection_idx >= CKIND_SHEIK) {
      cpd->state.char_selection_idx -= 14;
    } else if (cpd->state.char_selection_idx >= CKIND_YOUNGLINK) {
      cpd->state.char_selection_idx -= 21;
    } else {
      cpd->state.char_selection_idx += 7;
    }
  } else if (scrollInputs & (HSD_BUTTON_UP | HSD_BUTTON_DPAD_UP)) {
    // Handle a up input
    if (cpd->state.char_selection_idx % 7 >= 5 && cpd->state.char_selection_idx <= CKIND_LINK) {
      cpd->state.char_selection_idx += 14;
    } else if (cpd->state.char_selection_idx <= CKIND_LINK) {
      cpd->state.char_selection_idx += 21;
    } else {
      cpd->state.char_selection_idx -= 7;
    }
  }

  // Update which icon shows up selected
  for (int i = CKIND_FALCON; i <= CKIND_GANONDORF; i++) {
    CSIcon_Select_State state = CSIcon_Select_State_NotSelected;
    if (i == cpd->state.char_selection_idx) {
      state = CSIcon_Select_State_Hover;

      OSReport("Total: %f, Next: %f\n", JOBJ_GetJointAnimFrameTotal(cpd->char_icons[i]->root_jobj), JOBJ_GetJointAnimNextFrame(cpd->char_icons[i]->root_jobj));
    }

    CSIcon_SetSelectState(cpd->char_icons[i], state);
  }
}

CharPickerDialog *CharPickerDialog_Init(GUI_GameSetup *gui, void *on_select) {
  CharPickerDialog *cpd = calloc(sizeof(CharPickerDialog));

  // Init char picker dialog jobj
  cpd->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_CharDialog];
  cpd->gobj = JOBJ_LoadSet(0, cpd->jobj_set, 0, 0, 3, 1, 0, GObj_Anim);
  cpd->root_jobj = cpd->gobj->hsd_object;

  // Store callback
  cpd->on_select = on_select;

  // Connect CharStageIcons for each character to the dialog
  JOBJ *cur_joint = cpd->root_jobj->child->child->sibling->child;
  for (int i = CKIND_FALCON; i <= CKIND_GANONDORF; i++) {
    cpd->char_icons[i] = CSIcon_Init(gui);
    CSIcon_SetMaterial(cpd->char_icons[i], CSIcon_ConvertCharToMat(i));

    // Attach icon to cur joint and move to next joint
    JOBJ_AddChild(cur_joint, cpd->char_icons[i]->root_jobj);
    cur_joint = cur_joint->sibling;
  }

  // Create GOBJ to handle inputs
  GOBJ *input_handler_gobj = GObj_Create(4, 0, 128);
  GObj_AddUserData(input_handler_gobj, 4, HSD_Free, cpd);
  GObj_AddProc(input_handler_gobj, _InputsThink, 0);

  // Initialize state
  CharPickerDialog_SetPos(cpd, (Vec3){0, 0, 0});
  CharPickerDialog_CloseDialog(cpd);

  return cpd;
}

void CharPickerDialog_Free(CharPickerDialog *cpd) {
  // Not implemented because we only initialize a very limited amount of these
}

void CharPickerDialog_SetPos(CharPickerDialog *cpd, Vec3 pos) {
  cpd->state.pos = pos;
  if (!cpd->state.is_open) {
    return;
  }

  _SetPos(cpd, pos);
}

void CharPickerDialog_OpenDialog(CharPickerDialog *cpd, u8 start_char_idx, u8 start_char_color) {
  cpd->state.is_open = true;
  cpd->state.char_selection_idx = start_char_idx;
  cpd->state.char_color_idx = start_char_color;
  _SetPos(cpd, cpd->state.pos);
}

void CharPickerDialog_CloseDialog(CharPickerDialog *cpd) {
  cpd->state.is_open = false;
  _SetPos(cpd, (Vec3){0, 1000, 0});
}