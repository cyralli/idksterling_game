#include "raylib.h"
#include "raymath.h"
#include "useful.h"
#include <stdio.h>
#include <stdlib.h>
#define GRAVITY 1
#define FRICTION 1.1

typedef struct Spritesheet {
    Texture texture;
    Rectangle *origin; // create array
} Spritesheet;

typedef struct Player {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    bool directionx; // left = false, right = true. to get a good result just do this: (float)directionx - 0.5. this variable is for when rendering we know when we flip
    float maxspeedx;
    float sprintmul;
    float movementspeed;
    float jumpspeed;
    float downspeed;
    bool canjump;
    int worldid; // world that the player is at.
} Player;

typedef struct Block {
    Rectangle box;
    bool touchtype; // false = Touchable, but player can go down (One-Way Platform); true = Ground, this means that the player cant go down through this. 
    int originindex; // if less than 0 then we are only using color.
    float tile; // if 0 then this doesnt use tiles.
    Color color; // if its not 0, we can still multiply our textures by this value.
} Block;

typedef struct Portal {
    Rectangle box;
    char id; // if player collides with the portal then we enter the desired world id.
} Portal;

typedef struct World {
    char id;
    unsigned int spritesheetindex; // texture id
    Block foreground[16]; // we are limited to only use 16 blocks in our world. (because we dont need too many)
    Block background[32];
    Portal portals[4];
    Rectangle cameralimit;
} World;

float gametime = 0;

Player CreatePlayer() {
    Player player = { 0 };
    player.position = (Vector2){0, 0};
    player.size = (Vector2){7, 10};
    player.speed = (Vector2){0, 0};
    player.directionx = true; // right 
    player.maxspeedx = 70;
    player.sprintmul = 1.6;
    player.movementspeed = 7;
    player.jumpspeed = 60;
    player.downspeed = 100;
    player.canjump = false;
    player.worldid = 0; // market
    return player;
}

World *worlds[16]; // create array of pointers that point to a world variable
World market = { 0 };

Spritesheet gameassets[1]; // create array of spritesheets (just one spritesheet for right now)
void SetupGameAssets() {
    // --- market ----
    // set market (world) spritesheetindex to the texture id of market.png
    market.spritesheetindex = 0;

    gameassets[market.spritesheetindex].texture = LoadTexture("../asset/texture/market.png"); // load spritesheet for the market (1.3 MB in VRAM)
    gameassets[market.spritesheetindex].origin = malloc(sizeof(Rectangle) * 6); // <--- Count to how many assets are in the spritesheet. (also remember to free when the game ends.)
    gameassets[market.spritesheetindex].origin[0] = (Rectangle){0, 0, 200, 200}; // markettile
    gameassets[market.spritesheetindex].origin[1] = (Rectangle){200, 0, 157, 165}; // light
    gameassets[market.spritesheetindex].origin[2] = (Rectangle){357, 0, 600, 340}; // counter
    gameassets[market.spritesheetindex].origin[3] = (Rectangle){957, 0, 227, 400}; // peanutbutter
    gameassets[market.spritesheetindex].origin[4] = (Rectangle){1184, 0, 576, 614}; // shelf
    gameassets[market.spritesheetindex].origin[5] = (Rectangle){1760, 0, 576, 614}; // peanutshelf
}

Player player;
Camera2D camera = { 0 };

bool ingame = false;
void SetupGameVariables(Vector2 window) {
    player = CreatePlayer();
    camera.target = (Vector2){0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 15.0f;
    
    worlds[0] = &market;
    market.cameralimit = (Rectangle){-100, -100, 200, 100};

    market.foreground[0] = (Block){ {-90, 20, 200, 100}, true,  0, 0.05,  WHITE };
    // x, y, width, height, touchtype, textureorigin, tilesize, color
    
    market.background[0] = (Block){ {-57, 6.2, 26, 14},  false, 2, false, DARKGRAY }; // back counter
    market.background[1] = (Block){ {-60, 5.2, 27, 15},  false, 2, false, WHITE }; // counter

    market.background[2] = (Block){ {0, 0, 18, 20},      false, 4, false, WHITE }; // shelf
    market.background[3] = (Block){ {18, 0, 18, 20},     false, 4, false, WHITE }; // shelf
    market.background[4] = (Block){ {36, 0, 18, 20},     false, 4, false, WHITE }; // shelf
    market.background[5] = (Block){ {54, 0, 18, 20},     false, 5, false, WHITE }; // peanut shelf
}

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
    // ----------------------------------------------------------------------------------------------------------
    // -------------------------------------------------- X axis ------------------------------------------------

    player->speed.x += acceleratex * player->movementspeed;
    if (abs(player->speed.x) > player->maxspeedx  * (IsKeyDown(KEY_LEFT_SHIFT) ? player->sprintmul : 1)) {
        player->speed.x = (player->speed.x > 0 ? 1 : -1) * player->maxspeedx * (IsKeyDown(KEY_LEFT_SHIFT) ? player->sprintmul : 1);
    }
    if (acceleratex == 0) player->speed.x /= FRICTION;

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

    // ----------------------------------------------------------------------------------------------------------
    // -------------------------------------------------- Y axis ------------------------------------------------

    float prevPlayerBottom = player->position.y + player->size.y;

    player->speed.y += GRAVITY; // Gravity
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
    camera->target = player->position;
    if (camera->target.x < limit.x) camera->target.x = limit.x;
    if (camera->target.x > limit.x + limit.width) camera->target.x = limit.x + limit.width;
    if (camera->target.y < limit.y) camera->target.y = limit.y;
    if (camera->target.y < limit.y + limit.height) camera->target.y = limit.y + limit.height;

}

void UpdateGame(Player *player, Camera2D *camera, Vector2 window, float delta) {
    gametime += delta;
    World world = *worlds[player->worldid];
    UpdatePlayerPosition(player, world.foreground, sizeof(world.foreground) / sizeof(world.foreground[0]), window, delta);
    UpdateCamera2D(camera, player, delta, world.cameralimit);
}

void RenderPlayer(Player player) {
    DrawRectangleV(player.position, player.size, RED);
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
}

void ExitGame() {
    // we free our textures origins since we used malloc
    for (int i = 0; i < sizeof(gameassets) / sizeof(gameassets[0]); i++) {
        free(gameassets[i].origin);
    }
}