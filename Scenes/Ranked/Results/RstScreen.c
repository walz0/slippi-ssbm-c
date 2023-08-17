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
    // int char_id = 0x0;
    // LoadCharacterDat(char_id);
    // JOBJDesc *player_jobj = LoadCharacterJObjDesc(char_id, 0);
    // Load file
    gui_archive = Archive_LoadFile("RstScreen_gui.dat");
    // gui_archive = Archive_LoadFile("GmTou1p.dat");
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
        CKIND_FOX,
        CKIND_SHEIK 
    };

    CharacterKind winnerChar = fighters[1];
    const char* username = "username";

    int victoryTheme = GetCharacterVictoryTheme(winnerChar);

    /*
        TODO ::
            create all necessary values from match controller struct

        u8 slot = R13_U8(-0x5108);
        GOBJ *player = Match_SetupPlayerVictoryPose(
                            (int)(char)match->players[0].character_kind,
                            match->players[0].field3_0x3 >> 2,
                            slot
                        );
    */

    int charSFX = getCharNameSFX(winnerChar);
    SFX_PlayRaw(charSFX, 0xFF, 127, 0, 0);

    // Use the sound test menu function to play this sound (not sure why)
    // SoundTest_PlaySFX(NARRATOR_WINNER_IS);
    // SFX_PlayRaw(NARRATOR_WINNER_IS,-2,-0x80,0,7);

    GOBJ *winnerTextGobj = GObj_Create(0x4, 0x5, 0x80);

    // Load PLCo
    LoadPlCo();

    // JOBJ *winnerText = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_Winner]->jobj);
    // JOBJ *winnerText = JOBJ_LoadJoint(cubeModel);

    // winnerText->trans.Y = 7.75f;
    // winnerText->trans.X = -7.5f;

    // winnerText->scale.X = 3.5f;
    // winnerText->scale.Y = 3.7f;

    // JOBJ_AddSetAnim(winnerText, gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames], 0x0);
    // // JOBJ_AddSetAnim(winnerText, &cubeModel, 0x0);
    // JOBJ_AnimAll(winnerText);

    // GObj_AddObject(winnerTextGobj, 0x4, winnerText);
    // GObj_AddGXLink(winnerTextGobj, GXLink_Common, 1, 129);

// ================== CHARACTER NAME ====================
    /*GOBJ *charNameGobj = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *charName = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames]->jobj);

    // Set character name
    JOBJ_AddSetAnim(charName, gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames], 0x0);
    JOBJ_ReqAnimAll(charName, (u32) (winnerChar));
    JOBJ_AnimAll(charName);

    GObj_AddObject(charNameGobj, 0x4, charName);
    GObj_AddGXLink(charNameGobj, GXLink_Common, 1, 129);*/

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
    /*
        0 - empty
        1 - ?
        2 - battlefield
        3 - yoshis
        4 - dreamland
        5 - final destination
        6 - fod
        7 - pokemon stadium
    */

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

// ================== STAGE ICONS ====================
    FighterKind char_id = getFighterKind(winnerChar);
    // Put character model in memory
    LoadCharacterJObjDesc(char_id, 0);
    JOBJDesc *test_char = &(MODEL_INFO[char_id].costumes[0]->desc);
    JOBJ *test_char_jobj = JOBJ_LoadJoint(test_char);

    test_char_jobj->trans.Y = -5.0f;
    // JOBJ *test_char_jobj = JOBJ_LoadJoint(cfalc_joint);

    // JOBJ_AddSetAnim(test_char_jobj, gui_assets->jobjs[GUI_RstScreen_JOBJ_GameResult], 0x0);
    JOBJ_ReqAnimAll(test_char_jobj, (u32) 0);
    JOBJ_AnimAll(test_char_jobj);

    GOBJ *charGobj = GObj_Create(0x4, 0x5, 0x80);
    GObj_AddObject(charGobj, *objkind_jobj, test_char_jobj);
    GObj_AddGXLink(charGobj, GXLink_Common, 1, 129);
/*

*/
    GXColor white = (GXColor){255,255,255,255};
    // Create text object for rank info
    text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;


    // sprintf(debugString, "%x", &MODEL_INFO[0].costumes[0]->desc);
    sprintf(debugString, "%d", getFighterKind(winnerChar));
    // sprintf(debugString, "%s", fuck->costumes[0].desc->class_name);

    // Create Subtext objects
    // int debugText = Text_AddSubtext(text, -1400, -600, username);
    debugText = Text_AddSubtext(text, -1400, -600, debugString);
    Text_SetScale(text, debugText, 4.5f, 4.5f);

    // Set color
    Text_SetColor(text, debugText, &white);
}

void minor_think() {
    /*u8 port = R13_U8(-0x5108);
    u64 downInputs = Pad_GetDown(port);
    u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent

    if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
        currentSFX++;
        SFX_PlayRaw(currentSFX, 0xFF, 0, 0, 0);
        sprintf(debugString, "%x", currentSFX);
        Text_SetText(text, debugText, debugString);
    }

    if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
        currentSFX--;
        SFX_PlayRaw(currentSFX, 0xFF, 0, 0, 0);
        sprintf(debugString, "%x", currentSFX);
        Text_SetText(text, debugText, debugString);
    }*/
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