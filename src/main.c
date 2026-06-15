#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "menu.h"
#include "game.h"

int windowWidth = 500;
int windowHeight = 500;
bool fullsc = false;

int main(void) {
    
    SetConfigFlags(0x00000104);
    InitWindow(windowWidth, windowHeight, "idksterling invasion");
    
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    
    Vector2 monitorsize = (Vector2){GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor())};
    
    Font fo1 = LoadFontEx("../asset/font/framd.ttf", monitorsize.y/10, 0, 0);
    
    while (!WindowShouldClose()) {
        // fullscreen
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
            if (IsWindowFullscreen()) {
                SetWindowSize(monitorsize.x, monitorsize.y);
            }
        }
        
        // update window size
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        
        double deltaTime = GetFrameTime();
        
        if (ingame) {
            if (gametime == 0) {
                SetupGameVariables((Vector2){windowWidth, windowHeight});
            }
            camera.offset = (Vector2){windowWidth/2.0, windowHeight/2.0};
            UpdateGame(&player, market, (Vector2){windowWidth, windowHeight}, deltaTime);
        }
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            if (!ingame) {
                RenderMenu(fo1, (Vector2){windowWidth, windowHeight});
            } else {
                RenderGame(player);
            }
            DrawFPS(0, 0);
        
        EndDrawing();
        
        if (!ingame) UpdateMenu(&ingame, (Vector2){windowWidth, windowHeight});
        
    }
    
    return 0;
}