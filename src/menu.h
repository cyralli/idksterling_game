#include "raylib.h"
#define TITLE "Peanut Butter Power Up"
#define playbutton_margin 10
#define playbutton_size 40
#define playbutton_length 2.5

Vector2 playbutton;
float ratio;

void RenderMenu(Font font, Vector2 window) {
    ratio = (float)window.x/(float)window.y;
    
    Vector2 titlesize = MeasureTextEx(font, TITLE, ratio*60, 1.0);
    playbutton = MeasureTextEx(font, "PLAY", ratio*playbutton_size, 1.0);
    
    DrawTextEx(font, TITLE, (Vector2){window.x/2 - titlesize.x/2, window.y/2.5 - titlesize.y}, ratio*60, 1.0, BLACK);
    
    Color reccolor = CheckCollisionPointRec(GetMousePosition(), (Rectangle){window.x/2 - (playbutton.x/2*playbutton_length) - playbutton_margin, window.y/1.5 - ratio*playbutton_size - playbutton_margin,
    playbutton.x*playbutton_length + playbutton_margin*2, ratio*playbutton_size + playbutton_margin*2}) ? (Color){255, 100, 100, 255} : (Color){255, 0, 0, 255};
    
    DrawRectangle(window.x/2 - (playbutton.x/2*playbutton_length) - playbutton_margin, window.y/1.5 - ratio*playbutton_size - playbutton_margin,
    playbutton.x*playbutton_length + playbutton_margin*2, ratio*playbutton_size + playbutton_margin*2, reccolor);
    
    DrawTextEx(font, "PLAY", (Vector2){window.x/2 - playbutton.x/2, window.y/1.5 - playbutton.y}, ratio*playbutton_size, 1.0, BLACK);
}

void UpdateMenu(bool *ingame, Vector2 window) {
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){window.x/2 - (playbutton.x/2*playbutton_length) - playbutton_margin, window.y/1.5 - ratio*playbutton_size - playbutton_margin,
    playbutton.x*playbutton_length + playbutton_margin*2, ratio*playbutton_size + playbutton_margin*2}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        *ingame = true;
    }
}