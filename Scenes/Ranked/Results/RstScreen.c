#include "RstScreen.h"
#include "../../Components/Button.h"
#include "../../Components/CharStageBoxSelector.h"
#include "../../Components/CharStageIcon.h"
#include "../../Components/StockIcon.h"
#include "../../Game/Characters.h"
#include "../../Game/Sounds.h"
#include "../../Files.h"
#include "../MexTK/include/fighter.h"

static HSD_Archive *gui_archive;
static GUI_RstScreen *gui_assets;
static RstScreen_Data *data;

int getCharNameSFX(CharacterKind kind) {
    CharNameSFX sfx = NARRATOR_NAMES[kind];
    return sfx.id;
}

int getFighterKind(CharacterKind kind) {
    return CONVERT_ID[kind].ftkind;
}

void minor_load() {


    // Load file
    gui_archive = Archive_LoadFile("RstScreen_gui.dat");
    gui_assets = Archive_GetPublicAddress(gui_archive, "ScGamTour_scene_data");

    GOBJ *cam_gobj = GObj_Create(2, 3, 128);
    COBJ *cam_cobj = COBJ_LoadDesc(gui_assets->cobjs[0]);
    GObj_AddObject(cam_gobj, 1, cam_cobj);
    GOBJ_InitCamera(cam_gobj, CObjThink, 0);
    GObj_AddProc(cam_gobj, MainMenu_CamRotateThink, 5);

    // Indicates which gx_links to render
    cam_gobj->cobj_links = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);

    // store cobj to static pointer, needed for MainMenu_CamRotateThink
    void **stc_cam_cobj = (R13 + (-0x4ADC));
    *stc_cam_cobj = gui_assets->cobjs[0];

    // create fog
    GOBJ *fog_gobj = GObj_Create(14, 2, 0);
    HSD_Fog *fog = Fog_LoadDesc(gui_assets->fog[0]);
    GObj_AddObject(fog_gobj, 4, fog);
    GObj_AddGXLink(fog_gobj, GXLink_Fog, 0, 128);

    // create lights
    GOBJ *light_gobj = GObj_Create(3, 4, 128);
    LOBJ *lobj = LObj_LoadAll(gui_assets->lights);
    GObj_AddObject(light_gobj, 2, lobj);
    GObj_AddGXLink(light_gobj, GXLink_LObj, 0, 128);

// ================== INIT ====================
    CharacterKind fighters[2] = {
        CKIND_MARIO,
        CKIND_LINK 
    };

    CharacterKind winnerChar = fighters[0];
    const char* username = "username";

    u8 port = R13_U8(-0x5108);
    // Pad_Rumble(port, 3, 0x20, 0);

    int victoryTheme = GetCharacterVictoryTheme(winnerChar);
    charSFX = getCharNameSFX(winnerChar);
    // Play "This game's winner is..."
    framesLeft = FRAMES_WINNER_IS;
    SoundTest_PlaySFX(NARRATOR_WINNER_IS);

    // Normally this loops over all 4 slots
    // GOBJ* playergobj = Match_SetupPlayerVictoryPose(winnerChar,0, port);
    // LoadCharacterDat(0);
    // SetupPlayerSlot(0);

    // StartMeleeData* data;
    // InitializeStartMeleeData(data);
    // Match_Init(data);

    // Load skeleton data and other common structs
    // LoadPlCo();
    // SetupPlayerClass();
    // Load character data
    // InitCharacterArrays();
    // CameraInfo_Init(6);

    // PlayerInitData *init_data;
    // init_data->character_kind = FTKIND_MARIO;
    // init_data->slot_type = 0x2;
    // int slot = 0;
    // Match_SetupPlayer(slot, init_data);
    // int costume_id = 0;

    // PlayerBlock_SetCharacterKind(slot, FTKIND_MARIO);
    // PlayerBlock_SetColor(slot, costume_id);
    // PlayerBlock_SetPort(slot, slot);
    // PlayerBlock_SetSlotType(slot, 2);
    // PlayerBlock_SetSpawnDirection(0.0, slot);

    // char *id_mem[5];
    // evil bit hack
    // id_mem[0] = (char) getFighterKind(winnerChar);
    // id_mem[1] = 0x0;
    // id_mem[2] = 0x0;
    // id_mem[3] = 0x0;
    // id_mem[4] = 0x0;

    // VictoryScreen_SetupRenderTarget(slot);
    // GOBJ* playerGobj = VictoryPose_Decide((int*) id_mem);
    // GOBJ* playergobj = Match_SetupPlayerVictoryPose((int) CKIND_MARIO, costume_id, slot);



    // SetupPlayerSlot(0);
    // PlayerCreateArgs createArgs;

    // GOBJ* playerGobj = GObj_Create(0x4, 0x8, 0);
    // GObj_AddGXLink(playerGobj, GXLink_Fighter, 5, 0);
    // HSD_ObjAllocData playerAlloc;
    // FighterData *player = (FighterData *)HSD_ObjAlloc(&playerAlloc);
    // GObj_AddUserData(playerGobj, 0x4, Player_Free, player);

    // LoadCharacterDat(winnerChar);

    // createArgs.character_id = winnerChar;
    // createArgs.slot = *(byte *)(winnerChar + 1);
    // createArgs.flags2 = (*(byte *)(winnerChar + 3) & 0x40) << 1 | createArgs.flags2 & ~127;
    // Player_InitializeFromInfo((GOBJ *) playerGobj, &createArgs);

    // player->common_state_num = 0xe;
    // player->ftstates_common = COMMON_ACTION_STATES;
    // player->ftstates_special = CHAR_ACTION_STATES[player->kind];
    // player->ftaction = *(SubactionInfo **)&player->ftData->x14;
    // player->pointer_to_next_linked_list = *(AnimationInfo **)&player->ftData->x18;

    // uint *CharacterEffectTableIndices = (uint *) 0x803c26fc;
    // Player_LoadEffects(CharacterEffectTableIndices[player->kind]);

    // LoadCharacterJObjDesc(player->kind, player->costume_id);

    // int unk1c = 0xd;
    // int unk18[4] = { 0xd, 0, 0, 0 };




    // Load character effects;
    // LoadEffects(0);


    // Use the sound test menu function to play this sound (not sure why)
    // SFX_PlayRaw(NARRATOR_WINNER_IS,-2,-0x80,0,7);


    // Load PLCo
    // LoadPlCo();

    GOBJ *winnerTextGobj = GObj_Create(0x4, 0x5, 0x80);

    JOBJ *winnerText = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_Winner]->jobj);
    // JOBJ *winnerText = JOBJ_LoadJoint(&cubeModel);

    winnerText->trans.Y = 7.75f;
    winnerText->trans.X = -7.5f;

    winnerText->scale.X = 3.5f;
    winnerText->scale.Y = 3.7f;

    // JOBJ_AddSetAnim(winnerText, gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames], 0x0);
    // JOBJ_AddSetAnim(winnerText, &cubeModel, 0x0);
    JOBJ_AnimAll(winnerText);

    GObj_AddObject(winnerTextGobj, 0x4, winnerText);
    GObj_AddGXLink(winnerTextGobj, GXLink_Common, 1, 129);

// ================== CHARACTER NAME ====================
    // GOBJ *charNameGobj = GObj_Create(0x4, 0x5, 0x80);
    // JOBJ *charName = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames]->jobj);

    // // Set character name
    // JOBJ_AddSetAnim(charName, gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames], 0x0);
    // JOBJ_ReqAnimAll(charName, (u32) (winnerChar));
    // JOBJ_AnimAll(charName);

    // GObj_AddObject(charNameGobj, 0x4, charName);
    // GObj_AddGXLink(charNameGobj, GXLink_Common, 1, 129);

// ================== CHARACTER THUMBNAILS ====================
    for (int i = 0; i < 2; i++) {
        // Get thumbnail data
        CharThumbnail thumbnail = THUMBNAILS[fighters[i]];

        GOBJ *charThumbGobj = GObj_Create(0x4, 0x5, 0x80);
        JOBJ *charThumb = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_Thumbnails]->jobj);

        charThumb->trans.Y = -13.0f;

        float scale = 0.8f;
        charThumb->scale.X = scale;
        charThumb->scale.Y = scale;
        switch (i) {
            case 0:
                // Flip thumbnail if it faces left
                if (thumbnail.dir == -1) {
                    charThumb->scale.X = -scale;
                }
                charThumb->trans.X = -1.8f;
                break;
            case 1:
                // Flip thumbnail if it faces right
                if (thumbnail.dir == 1) {
                    charThumb->scale.X = -scale;
                }
                charThumb->trans.X = 1.8f;
                break;
        }

        // Set character thumbnail
        JOBJ_AddSetAnim(charThumb, gui_assets->jobjs[GUI_RstScreen_JOBJ_Thumbnails], 0x0);
        JOBJ_ReqAnimAll(charThumb, (u32) thumbnail.kind);
        JOBJ_AnimAll(charThumb);

        GObj_AddObject(charThumbGobj, 0x4, charThumb);
        GObj_AddGXLink(charThumbGobj, GXLink_Common, 1, 129);
    }

// ================== STAGE ICONS ====================
        // 0 - empty
        // 1 - ?
        // 2 - battlefield
        // 3 - yoshis
        // 4 - dreamland
        // 5 - final destination
        // 6 - fod
        // 7 - pokemon stadium

    int matchResults[] = { 0, 1, 1 };
    int totalMatches = 3;

    for (int i = 0; i < totalMatches; i++) {
        GOBJ *stageGobj = GObj_Create(0x4, 0x5, 0x80);
        GOBJ *indicatorGobj = GObj_Create(0x4, 0x5, 0x80);
        // Load JOBJ
        JOBJ *stageIcon = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_CSIcon]->jobj);
        JOBJ *indicator = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_GameResult]->jobj);
        // JOBJ *stageIcon = JOBJ_LoadJoint(gui_assets->jobjs[1]->jobj);

        Vec2 stagePos;
        Vec2 indicatorPos;
        if (totalMatches == 3) {
            stagePos = (Vec2) {-5.5f, -18.5f};
            indicatorPos = (Vec2) {-5.5f, -15.5f};
        }
        else {
            stagePos = (Vec2) {-2.7f, -18.5f};
            indicatorPos = (Vec2) {-2.7f, -15.5f};
        }

        stageIcon->trans.X = stagePos.X + (i * 5.4f);
        stageIcon->trans.Y = stagePos.Y;

        indicator->trans.X = indicatorPos.X + (i * 5.4f);
        indicator->trans.Y = indicatorPos.Y;
        indicator->scale.X = 0.8f;
        indicator->scale.Y = 0.8f;

        JOBJ_AddSetAnim(stageIcon, gui_assets->jobjs[GUI_RstScreen_JOBJ_CSIcon], 0x0);
        // Set stage
        JOBJ_ReqAnimAll(stageIcon, (u32) (i + 0x2));
        JOBJ_AnimAll(stageIcon);

        GObj_AddObject(stageGobj, 0x4, stageIcon);
        GObj_AddGXLink(stageGobj, GXLink_Common, 1, 129);

        // Set win / loss
        int result = matchResults[i];
        JOBJ_AddSetAnim(indicator, gui_assets->jobjs[GUI_RstScreen_JOBJ_GameResult], 0x0);
        JOBJ_ReqAnimAll(indicator, (u32) ((result == 0) ? 2 : 1));
        JOBJ_AnimAll(indicator);

        GObj_AddObject(indicatorGobj, 0x4, indicator);
        GObj_AddGXLink(indicatorGobj, GXLink_Common, 1, 129);
    }

// ================== CHARACTER MODEL ====================
    FighterKind char_id = getFighterKind(winnerChar);
    int costume_id = 0;
    // Put character model in memory
    LoadCharacterJObjDesc(char_id, costume_id);
    JOBJDesc *charDesc = &(MODEL_INFO[char_id].costumes[costume_id]->desc);
    JOBJ *charJobj = JOBJ_LoadJoint(charDesc);

    charJobj->trans.X = 2.0f;
    charJobj->trans.Y = -6.0f;

    JOBJ_ReqAnimAll(charJobj, (u32) 0);
    JOBJ_AnimAll(charJobj);

    GOBJ *charGobj = GObj_Create(0x4, 0x5, 0x80);
    GObj_AddObject(charGobj, *objkind_jobj, charJobj);
    GObj_AddGXLink(charGobj, GXLink_Common, 1, 129);

    // VictoryScreen_CreateWinnerLogo();

    GXColor white = (GXColor){255,255,255,255};
    // Create text object for rank info
    text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;


    // sprintf(debugString, "%x", &MODEL_INFO[0].costumes[0]->desc);
    // sprintf(debugString, "%x", CharacterEffectTableIndices[player->kind]);
    // sprintf(debugString, "%s", fuck->costumes[0].desc->class_name);

    // Create Subtext objects
    // int debugText = Text_AddSubtext(text, -1400, -600, username);
    debugText = Text_AddSubtext(text, -1400, -600, debugString);
    Text_SetScale(text, debugText, 4.5f, 4.5f);

    // Set color
    Text_SetColor(text, debugText, &white);
}

void minor_think() {
    if (framesLeft == 1) {
        SFX_PlayRaw(charSFX, 0xFF, 127, 0, 0);
    }
    if (framesLeft > 0) {
        // framesLeft--;
    }
    // sprintf(debugString, "%d", framesLeft);
    // Text_SetText(text, debugText, debugString);

    u8 port = R13_U8(-0x5108);
    u64 downInputs = Pad_GetDown(port);
    u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent

    if (scrollInputs & (HSD_BUTTON_X | HSD_BUTTON_Y)) {
        // open stats menu
    }
}

void minor_exit() {

}

void CObjThink(GOBJ *gobj) {
  COBJ *cobj = gobj->hsd_object;

  if (!CObj_SetCurrent(cobj)) {
    return;
  }

  // HSD_Fog *fog = stc_fog;
  CObj_SetEraseColor(0, 0, 0, 255);
  CObj_EraseScreen(cobj, 1, 0, 1);
  CObj_RenderGXLinks(gobj, 7);
  CObj_EndCurrent();
}