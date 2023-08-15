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

    int totalMatches = 2;
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
            stagePos = (Vec2) {-3.0f, -18.5f};
            indicatorPos = (Vec2) {-3.0f, -15.5f};
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
        JOBJ_AddSetAnim(indicator, gui_assets->jobjs[GUI_RstScreen_JOBJ_GameResult], 0x0);
        JOBJ_ReqAnimAll(indicator, (u32) (0x1));
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
    Text *text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;

    char* debugString[20];
    sprintf(debugString, "%0.1f", test_char->jobj->scale.X);
    // sprintf(debugString, "%s", fuck->costumes[0].desc->class_name);

    // Create Subtext objects
    int debugText = Text_AddSubtext(text, -1100, 0, debugString);
    Text_SetScale(text, debugText, 5, 5);

    // Set color
    Text_SetColor(text, debugText, &white);
    // JOBJ *charact = JOBJ_LoadJoint(char_assets->jobjs[CHAR_JOBJ]->jobj);
}

void minor_think() {

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