#include "raylib.h"

typedef struct Spritesheet {
    Texture texture;
    Rectangle *origin; // create array
} Spritesheet;

#define MAX_INVENTORY 8
typedef struct Player {
    // properties
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

    // texture
    int spritesheetid;

    // items
    void *holdingitems[MAX_INVENTORY]; // create pointers that point to items, unfortunately since we are defining the item later, we have to use a void pointer
    char holding; // 
} Player;

typedef struct Item {
    Spritesheet *spritesheet;
    int spriteid;
    char *name;
    void (*function)(Player *, int, float); // (int = player holding index, so if idx is 1, then if player presses 1 this function runs)
} Item;

typedef struct Block {
    Rectangle box;
    bool touchtype; // false = Touchable, but player can go down (One-Way Platform); true = Ground, this means that the player cant go down through this. 
    int originindex; // if less than 0 then we are only using color.
    float tile; // if 0 then this doesnt use tiles.
    Color color; // if its not 0, we can still multiply our textures by this value.
} Block;

typedef struct Interactive {
    Rectangle *box;
    // "why a pointer?" if we want to make a floating item so that the player picks it up, then we use malloc
    // if we want to make some object interactive then we just assign the pointer to this variable.
    bool floatanimation;
    void (*function)(struct Player *, Rectangle, float); // make pointer that points to a function so that the function runs something
} Interactive;

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
    Interactive interact[16];
    Rectangle cameralimit;
} World;