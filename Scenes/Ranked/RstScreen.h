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
  u8 score_by_player[2];
  u8 tiebreak_game_num;
  u8 game_results[9];
  u16 last_stage_win_by_player[2];
  u8 (*get_winner_idx)();
} RstScreen_SceneData;
#pragma pack()

int (*GetCharacterVictoryTheme)(CharacterKind kind) = (int *) 0x80160400;
int (*SFX_PlayVolPitch)(int sfx, int volume, int pitch) = (int *) 0x800237a8;
JOBJ* (*VictoryScreen_GetWinnerLogoJObj)(JOBJ *jobj, CharacterKind ckind, int is_quit_out) = (JOBJ *)0x80176bf0;
void* (*VictoryScreen_CreateWinnerLogo)(void) = (void *) 0x80176f60;

// SFX
int NARRATOR_WINNER_IS = 0xC355;

int currentSFX = 0x7530;

int debugText;
char* debugString[20];
Text *text;

typedef struct CharThumbnail {
  FighterKind kind;
  int dir;
} CharThumbnail;

static CharThumbnail THUMBNAILS[33] = {
  {FTKIND_MARIO, -1},
  {FTKIND_FOX, 1},
  {FTKIND_FALCON, -1},
  {FTKIND_DK, -1},
  {FTKIND_KIRBY, 1},
  {FTKIND_BOWSER, -1},
  {FTKIND_LINK, 1},
  {FTKIND_SHEIK, -1},
  {FTKIND_NESS, 1},
  {FTKIND_PEACH, -1},
  {FTKIND_POPO, -1},
  {FTKIND_NANA, 0},
  {FTKIND_PIKACHU, -1},
  {FTKIND_SAMUS, -1},
  {FTKIND_YOSHI, -1},
  {FTKIND_JIGGLYPUFF, 1},
  {FTKIND_MEWTWO, -1},
  {FTKIND_LUIGI, 1},
  {FTKIND_MARTH, 1},
  {FTKIND_ZELDA, -1},
  {FTKIND_YOUNGLINK, -1},
  {FTKIND_DRMARIO, 1},
  {FTKIND_FALCO, 1},
  {FTKIND_PICHU, 1},
  {FTKIND_GAW, -1},
  {FTKIND_GANONDORF, 1},
  {FTKIND_ROY, -1},
  {FTKIND_MASTERHAND, 0},
  {FTKIND_CRAZYHAND, 0},
  {FTKIND_BOY, 0},
  {FTKIND_GIRL, 0},
  {FTKIND_GIGABOWSER, 0},
  {FTKIND_SANDBAG, 0},
};

typedef struct CharNameSFX {
  FighterKind kind;
  int id;
} CharNameSFX;

static CharNameSFX NARRATOR_NAMES[33] = {
  {FTKIND_MARIO, 0x7C830 + 21},
  {FTKIND_FOX, 0x7C830 + 5},
  {FTKIND_FALCON, 0x7C830},
  {FTKIND_DK, 0x7C830 + 1},
  {FTKIND_KIRBY, 0x7C830 + 15},
  {FTKIND_BOWSER, 0x7C830 + 16},
  {FTKIND_LINK, 0x7C830 + 18},
  {FTKIND_SHEIK, 0x7C830 + 32},
  {FTKIND_NESS, 0x7C830 + 26},
  {FTKIND_PEACH, 0x7C830 + 27},
  {FTKIND_POPO, 0x7C830 + 11},
  {FTKIND_NANA, 0x7C830 + 11},
  {FTKIND_PIKACHU, 0x7C830 + 28},
  {FTKIND_SAMUS, 0x7C830 + 30},
  {FTKIND_YOSHI, 0x7C830 + 31},
  {FTKIND_JIGGLYPUFF, 0x7C830 + 13},
  {FTKIND_MEWTWO, 0x7C830 + 24},
  {FTKIND_LUIGI, 0x7C830 + 20},
  {FTKIND_MARTH, 0x7C830 + 22},
  {FTKIND_ZELDA, 0x7C830 + 33},
  {FTKIND_YOUNGLINK, 0x7C830 + 19},
  {FTKIND_DRMARIO, 0x7C830 + 2},
  {FTKIND_FALCO, 0x7C830 + 4},
  {FTKIND_PICHU, 0x7C830 + 28},
  {FTKIND_GAW, 0x7C830 + 10},
  {FTKIND_GANONDORF, 0x7C830 + 6},
  {FTKIND_ROY, 0x7C830 + 12},
  {FTKIND_MASTERHAND, 0x7C830 + 25},
  {FTKIND_CRAZYHAND, 0},
  {FTKIND_BOY, 0x7C830 + 3},
  {FTKIND_GIRL, 0x7C830 + 3},
  {FTKIND_GIGABOWSER, 0x7C830 + 8},
  {FTKIND_SANDBAG, 0},
};

typedef struct MatchResult {
  int test;
} MatchResult;

typedef struct MatchTeamData {
  int test;
} MatchTeamData;

typedef struct MatchPlayerData {
  int test;
} MatchPlayerData;

typedef struct MatchControllerPlayerUnknown {
  int test;
} MatchControllerPlayerUnknown;

typedef struct MatchController {
  int last_update_time;
  MatchResult result;
  u8 unk;
  u8 is_teams;
  u8 unk2;
  int frame_count;
  u8 unk3;
  u8 winner_count;
  u8 team_winner_count;
  u8 biggest_loser;
  u8 winners[6];
  u8 team_winners[5];
  u8 unk4;
  MatchTeamData teams[5];
  MatchPlayerData players[6];
  u8 unk5[4];
  MatchControllerPlayerUnknown player_unknown[4];
  u8 unk6[2571];
} MatchController;

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