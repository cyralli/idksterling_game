#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "menu.h"
#include "game.h"

int windowWidth = 700;
int windowHeight = 700;

int main(void) {
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Peanut Butter Power Up");

    SetExitKey(KEY_NULL);

    InitSettingMenu(); // this loads the setting style
    SetupGameAssets();
    
    const Vector2 monitorsize = (Vector2){GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor())};
    
    Font fo1 = LoadFontEx("../asset/font/framd.ttf", monitorsize.y/4, 0, 0);

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    
    while (!WindowShouldClose()) {
        
        // update window size
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        
        double deltaTime = GetFrameTime();
        
        if (ingame) {
            if (gametime == 0) {
                SetupGameVariables((Vector2){windowWidth, windowHeight});
            }
            camera.offset = (Vector2){windowWidth/2.0, windowHeight/2.0};
            float targetHeight = 1080.0f; // The height your game design is optimized for
            camera.zoom = (float)windowHeight / targetHeight * 10;
            UpdateGame(&player, &camera, (Vector2){windowWidth, windowHeight}, deltaTime);
        }
        
        BeginDrawing();
            
            if (!ingame) {
                if (!insetting) {
                    RenderMenu(fo1, (Vector2){windowWidth, windowHeight});
                } else {
                    RenderSettingMenu(&insetting, (Vector2){windowWidth, windowHeight}, monitorsize);
                }
            } else {
                ClearBackground(RAYWHITE);
                RenderGame(player);
            }
            DrawFPS(0, 0);
        
        EndDrawing();
        
        if (!ingame && !insetting) {
            UpdateMenu(&ingame, (Vector2){windowWidth, windowHeight});

            if (ingame) SetupGameVariables((Vector2){windowWidth, windowHeight});
        }
        if (IsKeyPressed(KEY_ESCAPE)) ingame = false;
    }
    ExitGame();
    UnloadFont(fo1);

    CloseWindow();
    
    return 0;
}