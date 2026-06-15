#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#define GRAVITY 1
#define FRICTION 1.1

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
    bool contacts[4];
} Player;

typedef struct Block {
    Rectangle box;
    bool touchtype; // false = Touchable, but player can go down (One-Way Platform); true = Ground, this means that the player cant go down through this. 
    char textureid; // 0 = color
    Rectangle textureorigin;
    Color color; // if its not 0, we can still multiply our textures by this value.
    void *playerignored;
} Block;

typedef struct World {
    char id;
    Block foreground[16]; // we are limited to only use 32 blocks in our world. (because we dont need too many)
    Block background[32];
} World;


float gametime = 0;

Player CreatePlayer() {
    Player player = { 0 };
    player.position = (Vector2){0, 0};
    player.size = (Vector2){7, 10};
    player.speed = (Vector2){0, 0};
    player.directionx = true; // right 
    player.maxspeedx = 100;
    player.sprintmul = 2;
    player.movementspeed = 10;
    player.jumpspeed = 60;
    player.downspeed = 100;
    player.canjump = false;
    return player;
}

Texture *gameassets;
void SetupGameAssets() {
    LoadTexture("../asset/texture/market.png");
}

Player player;
Camera2D camera = { 0 };

char gameworldid = 0;
World market = { 0 };

bool ingame = false;
void SetupGameVariables(Vector2 window) {
    player = CreatePlayer();
    camera.target = (Vector2){0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 10.0f;
    
    market.id = 0; // we start with the market.
    market.foreground[0] = (Block){ {-90, 20, 300, 10}, true, 0, {0, 0, 0, 0}, GRAY };
    market.foreground[1] = (Block){ {-90, 10, 30, 4}, false, 0, {0, 0, 0, 0}, GRAY };
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
    if (abs(player->speed.x) > player->maxspeedx) {
        player->speed.x = (player->speed.x > 0 ? 1 : -1) * player->maxspeedx;
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
        else if ((blocks + i)->playerignored != NULL) (blocks + i)->playerignored = NULL; // if we arent touching this rectangle since were here we check if its pointing to the player
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
        else if ((blocks + i)->playerignored != NULL) (blocks + i)->playerignored = NULL; // if we arent touching this rectangle since were here we check if its pointing to the player
    }
}

void UpdateGame(Player *player, World world, Vector2 window, float delta) {
    gametime += delta;
    UpdatePlayerPosition(&*player, world.foreground, sizeof(world.foreground) / sizeof(world.foreground[0]), window, delta);
}

void RenderPlayer(Player player) {
    DrawRectangleV(player.position, player.size, RED);
}

void RenderWorld() {
    for (int i = 0; i < 16; i++) {
        DrawRectangleRec(market.foreground[i].box, market.foreground[i].color);
    }
}

void RenderGame(Player player) {
    char text[20];
    sprintf(text, "Time: %f", gametime);
    DrawText(text, 0, 20, 20, BLACK);
    BeginMode2D(camera);
            
        RenderWorld();
        RenderPlayer(player);
            
    EndMode2D();
}