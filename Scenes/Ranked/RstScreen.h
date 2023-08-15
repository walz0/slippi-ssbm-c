#include "../../m-ex/MexTK/mex.h"
#include "../../Components/Button.h"
#include "../../Components/CharPickerDialog.h"
#include "../../Components/CharStageBoxSelector.h"
#include "../../Components/FlatTexture.h"
#include "../../Components/GameResult.h"
#include "../../Components/RightArrow.h"
#include "../../Components/TurnIndicator.h"
#include "../../ExiSlippi.h"

// This struct is defined in asm via directives and has no padding, so we need to use pack(1)
#pragma pack(1)
typedef struct RstScreen_SceneData {
  u8 max_games;
  u16 cur_game;
  u8 score_by_player[2];
  u8 prev_winner;
  u8 tiebreak_game_num;
  u8 game_results[9];
  u16 last_stage_win_by_player[2];
  u8 color_ban_active;
  u8 color_ban_char;
  u8 color_ban_color;
  u8 last_game_end_mode;
  u8 (*get_winner_idx)();
} RstScreen_SceneData;
#pragma pack()

typedef struct PlayerCreateArgs {
    int character_id;
    u8 slot;
    u8 unk;
    u8 flags2;
    u8 unk1;
} PlayerCreateArgs;

typedef struct CostumeModelInfo {
  JOBJDesc *desc;
  void *matanim_data;
  u8 unk0;
  u8 unk1;
  u8 unk2;
  u8 unk3;
  u8 unk4;
  u8 unk5;
  u8 unk6;
  u8 unk7;
  u8 unk8;
  u8 unk9;
  u8 unk10;
  u8 unk11;
  void *joint_data;
} CostumeModelInfo;

typedef struct CharacterModelInfo {
  CostumeModelInfo **costumes;
  u8 costume_count;
  u8 unk1;
  u8 unk2;
  u8 unk3
} CharacterModelInfo;

GOBJ* (*Player_Create)(PlayerCreateArgs args) = 0x80068e98;
JOBJDesc* (*LoadCharacterJObjDesc)(int character_id, int costume) = 0x80068e98;
void* (*LoadCharacterDat)(int character_id) = 0x8008572c;
// void* (*Player_GXProc)(int character_id) = 0x8008572c;

typedef struct FtCostume {
  JOBJ *jobj;
  MatAnimJointDesc *mat;
} FtCostume;

typedef struct RstScreen_State {
  int selector_idx;
  int prev_selector_idx;
  u8 selected_values_count;
  int btn_hover_idx;
  int step_idx;
  u8 is_complete;
  u8 should_terminate;
  u32 terminate_counter;
  u8 is_disconnect;
} GameSetup_State;

typedef struct RstScreen_Data {
  GameSetup_State state;
  int step_count;
  Button *buttons[2];
  int button_count;
  FlatTexture *description;
  FlatTexture *disconnect_msg;
  TurnIndicator *turn_indicators[2];
  GameResult **game_results;
  int game_result_count;
  Text *text;
  int timer_subtext_id;
  int timer_frames;
  int disconnect_frames;
  RstScreen_SceneData *scene_data;

  ExiSlippi_MatchState_Response *match_state;
  ExiSlippi_FetchStep_Query *fetch_query;
  ExiSlippi_FetchStep_Response *fetch_resp;
  ExiSlippi_CompleteStep_Query *complete_query;
} RstScreen_Data;

void CObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);
void HandleOpponentStep();
void InitAllSelectorJobjs();
void InitSelectorJobjs(CSIcon_Material *iconMats, CSBoxSelector **selectors, int count);
void InitState();
void InitStrikingSteps();
void InitCounterpickingSteps();
void InitSteps();
void InitHeader();
void ResetButtonState(u8 is_visible);
void CompleteCurrentStep(int committed_count);
void SetMatchSelections(u8 char_id, u8 char_color, u8 char_option, u16 stage_id, u8 stage_option);
void PrepareCurrentStep();
void CompleteGamePrep();
void ShowDisconnectedMessage();

void OnCharSelectionComplete(CharPickerDialog *cpd, u8 is_selection);
u8 GetNextColor(u8 charId, u8 colorId, int incr);
u8 IsColorAllowed(u8 charId, u8 colorId, u8 playerIdx);

void IncrementSelectorIndex();
void DecrementSelectorIndex();
void ResetSelectorIndex();

u8 UpdateTimer();  // Returns bool indicating if time is elapsed
void UpdateHoverDisplays();
void UpdateTimeline();