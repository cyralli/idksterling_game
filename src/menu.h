#include "raylib.h"
#include "settings.h"

#define TITLE "Peanut Butter Power Up"
#define title_fsize 60
#define playbutton_padding 10
#define playbutton_size 40
#define button_length 2.5
#define buttons_margin 10

Vector2 playbuttontx;
Rectangle playbutton;
Rectangle settingsbutton;
bool insetting = false;

void RenderMenu(Font font, Vector2 window) {
    ClearBackground(RAYWHITE);
    
    Vector2 titlesize = MeasureTextEx(font, TITLE, title_fsize, 1.0);
    playbuttontx = MeasureTextEx(font, "Play", playbutton_size, 1.0);
    playbutton = (Rectangle){window.x/2 - (playbuttontx.x/2*button_length) - playbutton_padding, window.y/1.8 - playbutton_size - playbutton_padding,
    playbuttontx.x*button_length + playbutton_padding*2, playbutton_size + playbutton_padding*2};
    
    DrawTextEx(font, TITLE, (Vector2){window.x/2 - titlesize.x/2, window.y/2.5 - titlesize.y}, title_fsize, 1.0, BLACK);
    
    Color reccolorplay = CheckCollisionPointRec(GetMousePosition(), playbutton) ? (Color){255, 100, 100, 255} : (Color){255, 0, 0, 255};
    
    DrawRectangleRec(playbutton, reccolorplay);
    DrawTextEx(font, "Play", (Vector2){window.x/2 - playbuttontx.x/2, window.y/1.8 - playbuttontx.y}, playbutton_size, 1.0, BLACK);
    
    Vector2 settingsbuttontx = MeasureTextEx(font, "Settings", playbutton_size, 1.0);

    settingsbutton = playbutton;
    settingsbutton.y += playbutton.height + buttons_margin;

    Color reccolorsettings = CheckCollisionPointRec(GetMousePosition(), settingsbutton) ? (Color){255, 100, 100, 255} : (Color){255, 0, 0, 255};

    DrawRectangleRec(settingsbutton, reccolorsettings);
    DrawTextEx(font, "Settings", (Vector2){window.x/2 - playbuttontx.x/2 - (settingsbuttontx.x - playbuttontx.x) / 2, window.y/1.8 - playbuttontx.y + playbutton.height + buttons_margin}, playbutton_size, 1.0, BLACK);
}

void UpdateMenu(bool *ingame, Vector2 window) {
    if (CheckCollisionPointRec(GetMousePosition(), playbutton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        *ingame = true;
    }
    if (CheckCollisionPointRec(GetMousePosition(), settingsbutton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        insetting = true;
    }
}