#include "raylib.h"
#include "raymath.h"
#include "useful.h"
#include "gamedef.h"
#include <stdio.h>
#include <stdlib.h>
#define GRAVITY 100
#define FRICTION 2.6

float gametime = 0;

Player CreatePlayer() {
    Player player = { 0 };
    player.position = (Vector2){0, 0};
    player.size = (Vector2){7, 10};
    player.speed = (Vector2){0, 0};
    player.directionx = true; // right 
    player.maxspeedx = 100;
    player.sprintmul = 1.6;
    player.movementspeed = 350;
    player.jumpspeed = 60;
    player.downspeed = 100;
    player.canjump = false;
    player.worldid = 0; // market
    return player;
}

World *worlds[16]; // create array of pointers that point to a world variable
World market = { 0 };

Spritesheet gameassets[2]; // create array of spritesheets (just one spritesheet for right now)
void SetupGameAssets() {

    // --- player ---

    // --- market ---
    // set market (world) spritesheetindex to the texture id of market.png
    market.spritesheetindex = 1;

    gameassets[market.spritesheetindex].texture = LoadTexture("../asset/texture/market.png"); // load spritesheet for the market (1.3 MB in VRAM)
    gameassets[market.spritesheetindex].origin = malloc(sizeof(Rectangle) * 6); // <--- Count to how many assets are in the spritesheet. (also remember to free when the game ends.)
    gameassets[market.spritesheetindex].origin[0] = (Rectangle){0, 0, 200, 200}; // markettile
    gameassets[market.spritesheetindex].origin[1] = (Rectangle){200, 0, 157, 165}; // light
    gameassets[market.spritesheetindex].origin[2] = (Rectangle){357, 0, 600, 340}; // counter
    gameassets[market.spritesheetindex].origin[3] = (Rectangle){957, 0, 227, 400}; // peanutbutter
    gameassets[market.spritesheetindex].origin[4] = (Rectangle){1184, 0, 576, 614}; // shelf
    gameassets[market.spritesheetindex].origin[5] = (Rectangle){1760, 0, 576, 614}; // peanutshelf
}

// Item Functions --------------------------------------------------------------

    void PeanutbutterSpeed(Player *player, int idx, float delta) {
        static float time = 0;
        if (IsKeyPressed(49 + idx) && !time) {
            time = 5;
            player->maxspeedx *= 1.6;
            // remove item since we used it
            player->holdingitems[idx] = NULL;
            player->holding--;
        }
        if (time > 0) {
            time -= delta;
            if (time < 0) {
                time = 0;
                player->maxspeedx /= 1.6;
            }
        }
    }

// -----------------------------------------------------------------------------

// setting every item in the game.
Item peanutbutter = {NULL, 0, "Peanut Butter", PeanutbutterSpeed};

// Interactive Functions -------------------------------------------------------

    void PeanutShelf(Player *player, Rectangle box, float delta) {
        if (IsKeyPressed(KEY_E) && CheckCollisionRecs((Rectangle){player->position.x, player->position.y, player->size.x, player->size.y}, box)) {
            for (int i = 0; i < MAX_INVENTORY; i++) {
                if (player->holdingitems[i] != NULL) continue;
                player->holdingitems[i] = &peanutbutter;
                player->holding++;
                break;
            }
        }
    };

// -----------------------------------------------------------------------------

bool ingame = false;

Player player;
Camera2D camera = { 0 };

void SetupGameVariables(Vector2 window) {
    player = CreatePlayer();
    camera.target = (Vector2){0, 0};
    camera.rotation = 0.0f;

    peanutbutter.spritesheet = &gameassets[market.spritesheetindex];
    peanutbutter.spriteid = 3;
    
    worlds[0] = &market;
    market.cameralimit = (Rectangle){-100, -300, 200, 100};

    market.foreground[0] = (Block){ {-90, 20, 200, 100}, true,  0, 0.05,  WHITE };
    // x, y, width, height, touchtype, textureorigin, tilesize, color
    
    market.background[0] = (Block){ {-57, 6.2, 26, 14},  false, 2, false, DARKGRAY }; // back counter
    market.background[1] = (Block){ {-60, 5.2, 27, 15},  false, 2, false, WHITE }; // counter

    market.background[2] = (Block){ {0, -4, 22, 24},     false, 4, false, WHITE }; // shelf
    market.background[3] = (Block){ {22, -4, 22, 24},     false, 4, false, WHITE }; // shelf
    market.background[4] = (Block){ {44, -4, 22, 24},     false, 4, false, WHITE }; // shelf
    market.background[5] = (Block){ {66, -4, 22, 24},     false, 5, false, WHITE }; // peanut shelf

    market.interact[0] = (Interactive){&market.background[5].box, false, PeanutShelf};
}

// ------------------------------------------------------- UPDATE -------------------------------------------------------

    void UpdatePlayerPosition(Player *player, Block *blocks, unsigned char length, Vector2 window, float delta) {
    
        signed char acceleratex = 0;
        bool requestgodown = false;
        if (IsKeyDown(KEY_A)) acceleratex--;
        if (IsKeyDown(KEY_D)) acceleratex++;
        if (acceleratex != 0) player->directionx = (acceleratex > 0); 
        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) && player->canjump) {
            player->speed.y = -player->jumpspeed;
            player->canjump = false;
        }
        if (IsKeyDown(KEY_S)) {
            requestgodown = true;
            player->speed.y = player->downspeed;
        }
    
        // X axis 
    
        player->speed.x += acceleratex * player->movementspeed * delta;
        if (abs(player->speed.x) > player->maxspeedx  * (IsKeyDown(KEY_LEFT_SHIFT) ? player->sprintmul : 1)) {
            player->speed.x = (player->speed.x > 0 ? 1 : -1) * player->maxspeedx * (IsKeyDown(KEY_LEFT_SHIFT) ? player->sprintmul : 1);
        }
        if (acceleratex == 0) player->speed.x /= FRICTION * delta + 1;
    
        player->position.x += player->speed.x * delta;
    
        for (int i = 0; i < length; i++) {
            if ((blocks + i)->box.width == 0 || (blocks + i)->box.height == 0) continue;
        
            if (!(blocks + i)->touchtype) continue; // if its one-way then we dont detect x axis collision in this block.
            
            if (CheckCollisionRecs((Rectangle){player->position.x, player->position.y, player->size.x, player->size.y}, (blocks + i)->box)) {
                if (player->speed.x > 0) {
                    // moving right
                    player->position.x = (blocks + i)->box.x - player->size.x;
                } else if (player->speed.x < 0) {
                    // moving left
                    player->position.x = (blocks + i)->box.x + (blocks + i)->box.width;
                }
                player->speed.x = 0;
            }
        }
    
        // Y axis
        
        player->canjump = false;

        float prevPlayerBottom = player->position.y + player->size.y;
    
        player->speed.y += GRAVITY * delta; // Gravity
        player->position.y += player->speed.y * delta;
    
        for (int i = 0; i < length; i++) {
            if ((blocks + i)->box.width == 0 || (blocks + i)->box.height == 0) continue;
        
            if (CheckCollisionRecs((Rectangle){player->position.x, player->position.y, player->size.x, player->size.y}, (blocks + i)->box)) {
                if (!(blocks + i)->touchtype) {
                    if (player->speed.y < 0 || prevPlayerBottom > (blocks + i)->box.y + 1.0f || requestgodown) {
                        continue; 
                    }
                }
                if (player->speed.y > 0) {
                    // falling down
                    player->position.y = (blocks + i)->box.y - player->size.y;
                    player->canjump = true;
                    player->speed.y = 0;
                } else if (player->speed.y < 0) {
                    player->position.y = (blocks + i)->box.y + (blocks + i)->box.height;
                    player->speed.y = 0;
                } 
            }
        }
    }
    
    void UpdateCamera2D(Camera2D *camera, Player *player, float delta, Rectangle limit)
    {
        camera->target = Vector2Add(player->position, Vector2Scale(player->size, 0.5));
        if (camera->target.x < limit.x) camera->target.x = limit.x;
        if (camera->target.x > limit.x + limit.width) camera->target.x = limit.x + limit.width;
        if (camera->target.y < limit.y) camera->target.y = limit.y;
        if (camera->target.y < limit.y + limit.height) camera->target.y = limit.y + limit.height;
    
    }
    
    void UpdateInteractive(Player *player, World world, float delta) {
        for (int i = 0; i < sizeof(world.interact) / sizeof(Interactive); i++) {
            if (world.interact[i].function == NULL) continue; 
            // if we dont have a function then thats obviously not interactable.
            if (Vector2Distance((Vector2){world.interact[i].box->x, world.interact[i].box->y}, player->position) > 64) continue; 
            // if its too far then dont even update. not worth it
            
            world.interact[i].function(player, *world.interact[i].box, delta);
        }
    }
    
    void UpdateItem(Player *player, float delta) {
        for (int i = 0; i < MAX_INVENTORY; i++) {
            Item *item = (Item *)player->holdingitems[i];
            if (item == NULL) continue;
        
            item->function(player, i, delta);
        }
    }
    
    void UpdateGame(Player *player, Camera2D *camera, Vector2 window, float delta) {
        gametime += delta;
        World world = *worlds[player->worldid];
        UpdateInteractive(player, world, delta);
        UpdateItem(player, delta);
        UpdatePlayerPosition(player, world.foreground, sizeof(world.foreground) / sizeof(world.foreground[0]), window, delta);
        UpdateCamera2D(camera, player, delta, world.cameralimit);
    }

// ------------------------------------------------------- RENDERING -------------------------------------------------------

    void RenderPlayer(Player player) {
        DrawRectangleV(player.position, player.size, RED);
    }

    void RenderInventory(Player player) {
        for (int i = 0; i < 8; i++) {
            Item *item = (Item *)player.holdingitems[i];

            if (item == NULL) continue; // if player isnt holding a item

            DrawTexturePro(item->spritesheet->texture, item->spritesheet->origin[item->spriteid], (Rectangle){20+i*200, 20, 200, 200}, (Vector2){0,0}, 0, WHITE);
        }
    }

    void RenderWorld(Player player) {
        if (worlds[player.worldid] == NULL) return;

        World currentworld = *worlds[player.worldid];
        // render background
        for (int i = 0; i < sizeof(currentworld.background) / sizeof(Block); i++) {
            if (currentworld.background[i].box.width == 0 || currentworld.background[i].box.height == 0) continue;

            if (currentworld.background[i].originindex < 0) {
                DrawRectangleRec(currentworld.background[i].box, currentworld.background[i].color);
            } else {
                if (currentworld.background[i].tile) {
                    DrawTextureTiled(
                        gameassets[currentworld.spritesheetindex].texture,
                        gameassets[currentworld.spritesheetindex].origin[currentworld.background[i].originindex],
                        currentworld.background[i].box,
                        (Vector2){0, 0},
                        0.0f,
                        currentworld.background[i].tile,
                        currentworld.background[i].color
                    );
                } else {
                    DrawTexturePro(
                        gameassets[currentworld.spritesheetindex].texture,
                        gameassets[currentworld.spritesheetindex].origin[currentworld.background[i].originindex],
                        currentworld.background[i].box,
                        (Vector2){0, 0},
                        0.0f,
                        currentworld.background[i].color
                    );
                }
            }
        }
        // render foreground
        for (int i = 0; i < sizeof(currentworld.foreground) / sizeof(Block); i++) {
            if (currentworld.foreground[i].box.width == 0 || currentworld.foreground[i].box.height == 0) continue;

            if (currentworld.foreground[i].originindex < 0) {
                DrawRectangleRec(currentworld.foreground[i].box, currentworld.foreground[i].color);
            } else {
                if (currentworld.foreground[i].tile) {
                    DrawTextureTiled(
                        gameassets[currentworld.spritesheetindex].texture,
                        gameassets[currentworld.spritesheetindex].origin[currentworld.foreground[i].originindex],
                        currentworld.foreground[i].box,
                        (Vector2){0, 0},
                        0.0f,
                        currentworld.foreground[i].tile,
                        currentworld.foreground[i].color
                    );
                } else {
                    DrawTexturePro(
                        gameassets[currentworld.spritesheetindex].texture,
                        gameassets[currentworld.spritesheetindex].origin[currentworld.foreground[i].originindex],
                        currentworld.foreground[i].box,
                        (Vector2){0, 0},
                        0.0f,
                        currentworld.foreground[i].color
                    );
                }
            }
        }
    }

    void RenderGame(Player player) {
        char text[20];
        sprintf(text, "Time: %f", gametime);
        DrawText(text, 0, 20, 20, BLACK);
        BeginMode2D(camera);

            RenderWorld(player);
            RenderPlayer(player);

        EndMode2D();
        RenderInventory(player);
    }

void ExitGame() {
    // we free our textures origins since we used malloc
    for (int i = 0; i < sizeof(gameassets) / sizeof(gameassets[0]); i++) {
        if (gameassets[i].origin != NULL) {
            free(gameassets[i].origin);
            gameassets[i].origin = NULL;
        }
    }
}