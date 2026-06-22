#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define margin 20
#define wwidth 700
#define vdsett_height 300
#define adsett_height 150

void InitSettingMenu() {
    GuiLoadStyleDefault();
    GuiLoadStyle("dark.rgs");
}

int FPSTarget = 60; // default
bool FPSValueboxedit = false;
int CommonFPSValues[] = {30, 60, 75, 120, 144, 165, 180, 200, 240};
int CommonResolutions[][3] = {
    {640, 360},
    {854, 480},
    {1280, 720},
    {1366, 768},
    {1440, 900},
    {1536, 864},
    {1600, 900},
    {1920, 1080},
    {2560, 1440},
    {3840, 2160}
};

void RenderSettingMenu(bool *insetting, Vector2 window, const Vector2 monitorsize) {
    
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    int groupwidth = window.x>wwidth+margin?wwidth:window.x-margin*2;
    GuiGroupBox((Rectangle){margin, margin, groupwidth, vdsett_height}, "Video Settings");
    GuiGroupBox((Rectangle){margin, margin*2 + vdsett_height, groupwidth, adsett_height}, "Audio Settings");
    // ---------------------------------------------------------------------------------------------------------------
    // Video Settings
    // ---------------------------------------------------------------------------------------------------------------
    
    GuiLabel((Rectangle){margin+10, margin+20, 100, 20}, "Framerate Limit");
    float FPSTargetfloat = (float)FPSTarget;
    if (GuiSlider((Rectangle){margin+10 + GuiGetTextWidth("30.00 "), margin+40, 250, 20}, "30.00", "Unlimited", &FPSTargetfloat, 30, 300) && !FPSValueboxedit) {
        FPSTarget = (int)FPSTargetfloat;
        for (short i = 0; i < 9; i++) {
            if (FPSTarget < CommonFPSValues[i] + 10 && FPSTarget > CommonFPSValues[i] - 10) {
                FPSTarget = CommonFPSValues[i];
                break;
            }
        }
    }
    char str[10];
    sprintf(str, "%d", FPSTarget);
    if (FPSTarget >= 300) strcpy(str, "Unlimited");
    GuiDrawText(str, (Rectangle){margin+10 + GuiGetTextWidth("30.00 "), margin+40 + 20/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2, 200, GuiGetStyle(DEFAULT, TEXT_SIZE)}, TEXT_ALIGN_CENTER, WHITE);

    static bool fullscbutton;
    GuiCheckBox((Rectangle){margin+10, margin+70, 20, 20}, "Fullscreen", &fullscbutton);

    static int activeRes = 0;
    static bool resDropEdit = false;
    if (fullscbutton) {
        char dropdStr[256];
        sprintf(dropdStr, "%dx%d (Monitor)", (int)monitorsize.x, (int)monitorsize.y);
        for (int i = 0; i < 10; i++) {
            char temp[32];
            sprintf(temp, ";%dx%d", CommonResolutions[i][0], CommonResolutions[i][1]);
            strcat(dropdStr, temp);
        }
        if (GuiDropdownBox((Rectangle){margin+110, margin+70, 140, 20}, dropdStr, &activeRes, resDropEdit)) resDropEdit = !resDropEdit;
    }

    static bool vsyncbutton;
    GuiCheckBox((Rectangle){margin+10, margin+100, 20, 20}, "Vertical Sync", &vsyncbutton);

    // ---------------------------------------------------------------------------------------------------------------
    // Audio Settings
    // ---------------------------------------------------------------------------------------------------------------
    
    static float musicVolume = 1.0f;
    if (GuiSliderBar((Rectangle){margin+10 + GuiGetTextWidth("Music "), margin*2 + vdsett_height + 20, 200, 20}, "Music", NULL, &musicVolume, 0, 1)) {}


    // ---------------------------------------------------------------------------------------------------------------
    bool buttonok = GuiButton((Rectangle){groupwidth+margin-(70*3)-10, margin*3 + vdsett_height + adsett_height, 70, 30}, "#112#OK");
    if (GuiButton((Rectangle){groupwidth+margin-(70*2)-5, margin*3 + vdsett_height + adsett_height, 70, 30}, "#113#Cancel") || buttonok) *insetting = false;
    if (GuiButton((Rectangle){groupwidth+margin-70, margin*3 + vdsett_height + adsett_height, 70, 30}, "Apply") || buttonok || IsKeyPressed(KEY_ENTER)) {
        SetTargetFPS(FPSTarget>=300?0:FPSTarget);
        if (fullscbutton) {
            if (activeRes>0) SetWindowSize(CommonResolutions[activeRes-1][0], CommonResolutions[activeRes-1][1]);
            else SetWindowSize(monitorsize.x, monitorsize.y);
        }
        SetWindowState((FLAG_FULLSCREEN_MODE * fullscbutton) | (FLAG_VSYNC_HINT * vsyncbutton));
        ClearWindowState((FLAG_FULLSCREEN_MODE * !fullscbutton) | (FLAG_VSYNC_HINT * !vsyncbutton));
    }
}