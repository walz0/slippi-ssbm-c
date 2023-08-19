#include "../../m-ex/MexTK/mex.h"
#include "../../Components/Button.h"
#include "../../Components/CharPickerDialog.h"
#include "../../Components/CharStageBoxSelector.h"
#include "../../Components/FlatTexture.h"
#include "../../Components/GameResult.h"
#include "../../Components/RightArrow.h"
#include "../../Components/TurnIndicator.h"
#include "../../ExiSlippi.h"

typedef struct PlayerCreateArgs {
  int character_id;
  u8 slot;
  u8 unk;
  u8 flags2;
  u8 unk1;
} PlayerCreateArgs;

typedef struct AnimationInfo {
  u8 lerp_frames;
  u8 phys_joint_bone_index;
} AnimationInfo;

typedef struct SubactionInfo {
    char *symbol;
    int animOffset;
    int animSize;
    char *script;
    int anim_flags;
    HSD_Archive *anim_data;
} SubactionInfo;

typedef struct ActionStateInfo {
  int subaction;
  int action_stat;
  u8 unk;
  u8 unk1;
  u8 unk2;
  u8 unk3;
  void *animation_handler;
  void *interrupt_handler;
  void *physics_handler;
  void *collision_handler;
  void *camera_handler;
} ActionStateInfo;

typedef struct StartMeleeRules
{
    u8 flags1;             // 0x0
    u8 flags2;                
    u8 flags3;                // 0x4, temp transfer buffer used during CARDReads. is 0x2000 bytes
    u8 flags4;                // 0x4, temp transfer buffer used during CARDReads. is 0x2000 bytes
    u8 flags5;                // 0x4, temp transfer buffer used during CARDReads. is 0x2000 bytes
    u8 flags6;                      // 0x8
    u8 xc;                      // 0xc
    u8 x10;                     // 0x10
    int is_teams;                     // 0x14
    u8 x20;                     // 0x20
    u8 x24;                     // 0x24
    u8 item_frequency;                     // 0x28
    u8 sd_penalty;                     // 0x28
    u8 unk3;                     // 0x28
    u16 stage_id;                     // 0x28
    int time_limit;
    u8 unk4;                     // 0x28
    u8 unk5;                     // 0x28
    u8 unk6;                     // 0x28
    u8 unk7;                     // 0x28
    u8 unk8;                     // 0x28
    u8 unk9;                     // 0x28
    u8 unk10;                     // 0x28
    u8 unk11;                     // 0x28
    u8 unk12;                     // 0x28
    u8 unk13;                     // 0x28
    u8 unk14;                     // 0x28
    u8 unk15;                     // 0x28
    long item_mask;                     // 0x28
    u8 unk16;                     // 0x28
    u8 unk17;                     // 0x28
    u8 unk18;                     // 0x28
    u8 unk19;                     // 0x28
    u8 unk20;                     // 0x28
    u8 unk21;                     // 0x28
    u8 unk22;                     // 0x28
    u8 unk23;                     // 0x28
    float damage_ratio;                     // 0x28
    float game_speed;                     // 0x28
    u8 unk24;                     // 0x28
    u8 unk25;                     // 0x28
    u8 unk26;                     // 0x28
    u8 unk27;                     // 0x28
    u8 unk28;                     // 0x28
    u8 unk29;                     // 0x28
    u8 unk30;                     // 0x28
    u8 unk31;                     // 0x28
    void* pause_check_callback;                     // 0x28
    u8 unk32;                     // 0x28
    u8 unk33;                     // 0x28
    u8 unk34;                     // 0x28
    u8 unk35;                     // 0x28
    u8 unk36;                     // 0x28
    u8 unk37;                     // 0x28
    u8 unk38;                     // 0x28
    u8 unk39;                     // 0x28
    u8 unk40;                     // 0x28
    u8 unk41;                     // 0x28
    u8 unk42;                     // 0x28
    u8 unk43;                     // 0x28
    void* match_end_callback;                     // 0x28
    int unk44;                     // 0x28
    u8 unk45;                     // 0x28
    u8 unk46;                     // 0x28
    u8 unk47;                     // 0x28
    u8 unk48;                     // 0x28
    u8 unk49;                     // 0x28
    u8 unk50;                     // 0x28
    u8 unk51;                     // 0x28
    u8 unk52;                     // 0x28
} StartMeleeRules;

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

typedef struct StartMeleeData {
  StartMeleeRules rules; 
  PlayerData *players;
} StartMeleeData;

int (*GetCharacterVictoryTheme)(CharacterKind kind) = (int *) 0x80160400;
int (*SFX_PlayVolPitch)(int sfx, int volume, int pitch) = (int *) 0x800237a8;
JOBJ* (*VictoryScreen_GetWinnerLogoJObj)(JOBJ *jobj, CharacterKind ckind, int is_quit_out) = (JOBJ *)0x80176bf0;
void* (*VictoryScreen_CreateWinnerLogo)(void) = (void *) 0x80176f60;
GOBJ* (*Match_SetupPlayerVictoryPose)(CharacterKind kind, int costume_id, uint slot) = (GOBJ *)0x8017a67c;
void* (*LoadEffects)(int unk) = 0x8006737c;
GOBJ* (*VictoryPose_Decide)(int *kind) = (GOBJ *) 0x800be7e0;
void* (*Player_Free)(FighterData *player) = (void *) 0x8006dabc;
void* (*Player_InitializeFromInfo)(GOBJ *playerGobj, PlayerCreateArgs *args) = (void *) 0x80068914;
void* (*Player_LoadEffects)(int param_1) = (void *) 0x8006737c;
void* (*SetupPlayerSlot)(int slot) = (void *) 0x80031ad0;

void* (*Match_Init)(StartMeleeData *data) = (void *) 0x8016dcc0;
void* (*InitializeStartMeleeData)(StartMeleeData *data) = (void *) 0x80167a64;

// ActionStateInfo* (COMMON_ACTION_STATES) = 0x803c2800;
// ActionStateInfo* (CHAR_ACTION_STATES) = 0x803c12e0;

FtState* (COMMON_ACTION_STATES) = 0x803c2800;
FtState** (CHAR_ACTION_STATES) = 0x803c12e0;

// SFX
int NARRATOR_WINNER_IS = 0xC355;
int BACKGROUND_NOISE = 0x48; // Cheering

// GLOBALS
int currentSFX = 0x7530;
int debugText;
char* debugString[20];
Text *text;
int framesLeft;

int charSFX;

int FRAMES_WINNER_IS = 100;

// #define VICTORY_SCREEN_DATA(offset) *(u8 *)(0x8046dbe8 + offset)
// u8 individual_winner = VICTORY_SCREEN_DATA(4);
// u8 team_winner = VICTORY_SCREEN_DATA(5);
// u8 winner = VICTORY_SCREEN_DATA(6);
// int pnlsce = VICTORY_SCREEN_DATA(16);
// int flmsce = VICTORY_SCREEN_DATA(20);
// int think_gobj = VICTORY_SCREEN_DATA(24);
// int cobj = VICTORY_SCREEN_DATA(28);
// JOBJ *winner_logo = VICTORY_SCREEN_DATA(32);
// int last_match = VICTORY_SCREEN_DATA(148);
// int players = VICTORY_SCREEN_DATA(152);

typedef struct CharThumbnail {
  FighterKind kind;
  int dir;
} CharThumbnail;

typedef struct CharIdConversion {
  CharacterKind ckind;
  FighterKind ftkind;
} CharIdConversion;

static CharIdConversion CONVERT_ID[26] = {
  {CKIND_FALCON, 0},
  {CKIND_DK, 3},
  {CKIND_FOX, 1},
  {CKIND_GAW, 24},
  {CKIND_KIRBY, 4},
  {CKIND_BOWSER, 5},
  {CKIND_LINK, 6},
  {CKIND_LUIGI, 17},
  {CKIND_MARIO, 0},
  {CKIND_MARTH, 18},
  {CKIND_MEWTWO, 16},
  {CKIND_NESS, 8},
  {CKIND_PEACH, 9},
  {CKIND_PIKACHU, 12},
  {CKIND_ICECLIMBERS, 10},
  {CKIND_JIGGLYPUFF, 15},
  {CKIND_SAMUS, 13},
  {CKIND_YOSHI, 14},
  {CKIND_ZELDA, 19},
  {CKIND_SHEIK, 7},
  {CKIND_FALCO, 22},
  {CKIND_YOUNGLINK, 20},
  {CKIND_DRMARIO, 21},
  {CKIND_ROY, 26},
  {CKIND_PICHU, 23},
  {CKIND_GANONDORF, 25},
};

static CharThumbnail THUMBNAILS[33] = {
  {CKIND_FALCON, -1},
  {CKIND_DK, -1},
  {CKIND_FOX, 1},
  {CKIND_GAW, -1},
  {CKIND_KIRBY, 1},
  {CKIND_BOWSER, -1},
  {CKIND_LINK, 1},
  {CKIND_LUIGI, 1},
  {CKIND_MARIO, -1},
  {CKIND_MARTH, 1},
  {CKIND_MEWTWO, -1},
  {CKIND_NESS, 1},
  {CKIND_PEACH, -1},
  {CKIND_PIKACHU, -1},
  {CKIND_ICECLIMBERS, -1},
  {CKIND_JIGGLYPUFF, 1},
  {CKIND_SAMUS, -1},
  {CKIND_YOSHI, -1},
  {CKIND_ZELDA, -1},
  {CKIND_SHEIK, -1},
  {CKIND_FALCO, 1},
  {CKIND_YOUNGLINK, -1},
  {CKIND_DRMARIO, 1},
  {CKIND_ROY, -1},
  {CKIND_PICHU, 1},
  {CKIND_GANONDORF, 1},
  {CKIND_MASTERHAND, 0},
  {CKIND_CRAZYHAND, 0},
  {CKIND_BOY, 0},
  {CKIND_GIRL, 0},
  {CKIND_GIGABOWSER, 0},
  {CKIND_SANDBAG, 0},
};

typedef struct CharNameSFX {
  CharacterKind kind;
  int id;
} CharNameSFX;

static CharNameSFX NARRATOR_NAMES[33] = {
  {CKIND_FALCON, 0x7C830},
  {CKIND_DK, 0x7C830 + 1},
  {CKIND_FOX, 0x7C830 + 5},
  {CKIND_GAW, 0x7C830 + 10},
  {CKIND_KIRBY, 0x7C830 + 15},
  {CKIND_BOWSER, 0x7C830 + 16},
  {CKIND_LINK, 0x7C830 + 18},
  {CKIND_LUIGI, 0x7C830 + 20},
  {CKIND_MARIO, 0x7C830 + 21},
  {CKIND_MARTH, 0x7C830 + 22},
  {CKIND_MEWTWO, 0x7C830 + 24},
  {CKIND_NESS, 0x7C830 + 26},
  {CKIND_PEACH, 0x7C830 + 27},
  {CKIND_PIKACHU, 0x7C830 + 28},
  {CKIND_ICECLIMBERS, 0x7C830 + 11},
  {CKIND_JIGGLYPUFF, 0x7C830 + 13},
  {CKIND_SAMUS, 0x7C830 + 30},
  {CKIND_YOSHI, 0x7C830 + 31},
  {CKIND_ZELDA, 0x7C830 + 33},
  {CKIND_SHEIK, 0x7C830 + 32},
  {CKIND_FALCO, 0x7C830 + 4},
  {CKIND_YOUNGLINK, 0x7C830 + 19},
  {CKIND_DRMARIO, 0x7C830 + 2},
  {CKIND_ROY, 0x7C830 + 12},
  {CKIND_PICHU, 0x7C830 + 28},
  {CKIND_GANONDORF, 0x7C830 + 6},
  {CKIND_MASTERHAND, 0x7C830 + 25},
  {CKIND_CRAZYHAND, 0},
  {CKIND_BOY, 0x7C830 + 3},
  {CKIND_GIRL, 0x7C830 + 3},
  {CKIND_GIGABOWSER, 0x7C830 + 8},
  {CKIND_SANDBAG, 0},
};

typedef enum MatchResult {
  NONE = 0x0,
  TIMEOUT = 0x1,
  ELIMINATION = 0x2,
  LRASTART = 0x7,
  RETRY = 0x8,
  UNK = 0x9
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
  CostumeModelInfo** costumes;
  u8 costume_count;
  u8 unk1;
  u8 unk2;
  u8 unk3
} CharacterModelInfo;

CharacterModelInfo* (MODEL_INFO) = 0x803c0ec0;

int TOTAL_CHARACTERS = 26;

GOBJ* (*Player_Create)(PlayerCreateArgs args) = 0x80068e98;
CharacterKind (*LoadCharacterJObjDesc)(CharacterKind kind, int costume) = (int *) 0x80085820;
void* (*LoadCharacterDat)(int character_id) = 0x8008572c;
void* (*LoadPlCo)(void) = 0x80067abc;
void* (*SoundTest_PlaySFX)(int SFX) = 0x80023b24;

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