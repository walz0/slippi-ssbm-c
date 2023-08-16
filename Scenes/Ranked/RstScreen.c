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

int getCharNameSFX(FighterKind kind) {
    for (int i = 0; i < sizeof(NARRATOR_NAMES) / sizeof(CharNameSFX); i++) {
        CharNameSFX sfx = NARRATOR_NAMES[i];
        if (sfx.kind == kind) {
            return sfx.id;
        }
    }
    return 0;
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
    FighterKind fighters[2] = {
        FTKIND_JIGGLYPUFF,
        FTKIND_SHEIK
    };

    FighterKind winnerChar = fighters[1];
    const char* username = "username";

    int victoryTheme = GetCharacterVictoryTheme(CKIND_YOUNGLINK);

    int charSFX = getCharNameSFX(winnerChar);
    GOBJ *dummy = GObj_Create(0x4, 0x5, 0x80);
    JOBJ* jobj = (JOBJ*)(0x804d6504);
    JOBJ_LoadJoint(jobj);

    GObj_AddObject(dummy, 0x4, jobj);
    GObj_AddGXLink(dummy, GXLink_Common, 1, 129);
    // VictoryScreen_CreateWinnerLogo();
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

    // SFX_PlayRaw(charSFX, 0xFF, 0, 0, 0);

    // JOBJ *logo = VictoryScreen_GetWinnerLogoJObj(logo, CKIND_BOWSER, 0);
    // JOBJ_LoadJoint(logo);

    GOBJ *winnerTextGobj = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *winnerText = JOBJ_LoadJoint(gui_assets->jobjs[GUI_RstScreen_JOBJ_Winner]->jobj);

    winnerText->trans.Y = 7.75f;
    winnerText->trans.X = -7.5f;

    winnerText->scale.X = 3.5f;
    winnerText->scale.Y = 3.7f;

    JOBJ_AddSetAnim(winnerText, gui_assets->jobjs[GUI_RstScreen_JOBJ_CharNames], 0x0);
    JOBJ_AnimAll(winnerText);

    GObj_AddObject(winnerTextGobj, 0x4, winnerText);
    GObj_AddGXLink(winnerTextGobj, GXLink_Common, 1, 129);

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

    // HSD_Archive *test_char = Archive_LoadFile("PlMt.dat");
    // ftData *char_assets = Archive_GetPublicAddress(test_char, "ftDataMewtwo");

    // GOBJ *mtgobj= GObj_Create(0x4, 0x5, 0x80);
    GOBJ *mtgobj= GObj_Create(0x8, 0x4, 0x0);
    // HSD_Archive *test_char = Archive_LoadFile("PlMrNr.dat");
    JOBJSet *test_char = (JOBJSet *) Archive_LoadFile("PlMrNr.dat");
    // JOBJSet *char_assets = Archive_GetPublicAddress(test_char, "");

    // LoadCharacterDat(FTKIND_MEWTWO);

    // u8 char_id = FTKIND_BOWSER;
    // u8 offset = (char_id * 0x8);
    // CharacterModelInfo *fuck = (CharacterModelInfo *) 0x803c0ec0 + offset;
    // CharacterModelInfo *fuck = (CharacterModelInfo *) 0x803c0f40;
    // JOBJ_LoadJoint(fuck->costumes[0]->desc->child);
    // JOBJ_LoadJoint(test_char->jobj);
    
    // JOBJ_LoadSet(0, fuck->costumes[0].desc, 0, 0, 3, 1, 1, GObj_Anim);
    // JOBJ_LoadSet(0, test_char, 0, 0, 3, 1, 1, GObj_Anim);

    // GObj_AddObject(mtgobj, 0x4, test_char->jobj);
    // GObj_AddGXLink(mtgobj, GXLink_Common, 1, 129);


    // JOBJSet *char_assets = Archive_GetPublicAddress(test_char, "");

    // char_assets->scale = (Vec3) {1.0f, 1.0f, 1.0f};

    /*char_assets->trans.X = 0.0f;
    char_assets->trans.Y = 0.0f;

    JOBJ_LoadJoint(char_assets);
    // JOBJ_SetMtxDirtySub(instance);
    // JOBJ_LoadSet(0, char_assets->jobj, 0, 0, 3, 1, 1, GObj_Anim);

    GObj_AddObject(mtgobj, 0x4, char_assets);
    GObj_AddGXLink(mtgobj, GXLink_Common, 1, 129);*/
    // GOBJ *char_gobj = JOBJ_LoadSet(0, char_assets->costume_jobjdesc, 0, 0, 3, 1, 1, GObj_Anim);
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

    sprintf(debugString, "%x", victoryTheme);
    // sprintf(debugString, "%s", fuck->costumes[0].desc->class_name);

    // Create Subtext objects
    // int debugText = Text_AddSubtext(text, -1400, -600, username);
    debugText = Text_AddSubtext(text, -1400, -600, debugString);
    Text_SetScale(text, debugText, 4.5f, 4.5f);

    // Set color
    Text_SetColor(text, debugText, &white);
}

void minor_think() {
    u8 port = R13_U8(-0x5108);
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