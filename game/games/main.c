#include "Debug.h"
#include "GameEngine.h"
#include "sprite.img.h"
#include "tiles.img.h"

ImageTableHandle g_test_image   = NULL;
ImageTableHandle g_sprite_image = NULL;
int              g_x            = 0;
int              g_y            = 0;
TileMapHandle    g_tilemap      = NULL;
SpriteHandle     g_sprite       = NULL;
uint8_t          mapData[]      = {
    2,   3,   47,  48, 49, 49, 48, 49, 50, 3,  4,   48,  49,  48, 4,  5,  11,  12,  57,  58, 57, 57,  58, 57, 57,  57,
    57,  57,  57,  57, 13, 14, 40, 21, 66, 67, 66,  66,  67,  66, 66, 66, 66,  66,  66,  66, 22, 40,  49, 30, 74,  74,
    77,  77,  77,  77, 77, 77, 77, 77, 77, 77, 31,  49,  39,  39, 40, 38, 39,  40,  41,  42, 75, 10,  76, 10, 19,  20,
    46,  48,  39,  48, 49, 7,  8,  9,  50, 51, 75,  10,  76,  10, 28, 29, 55,  41,  41,  57, 67, 16,  17, 18, 59,  60,
    76,  10,  76,  76, 6,  10, 64, 41, 40, 66, 58,  25,  26,  27, 68, 69, 155, 155, 84,  85, 86, 155, 55, 40, 38,  30,
    74,  34,  35,  36, 77, 78, 76, 6,  93, 94, 95,  6,   55,  39, 41, 51, 6,   6,   6,   6,  6,  6,   15, 6,  102, 103,
    104, 6,   55,  48, 39, 60, 6,  6,  15, 6,  111, 112, 113, 6,  6,  6,  15,  6,   55,  39, 40, 69,  6,  6,  6,   75,
    120, 121, 122, 6,  75, 6,  6,  6,  55, 40, 41,  60,  6,   6,  76, 6,  129, 130, 131, 6,  6,  6,   6,  6,  64,  49,
    50,  69,  6,   6,  6,  76, 6,  15, 6,  6,  6,   15,  6,   6,  46, 47, 59,  60,  6,   6,  6,  6,   75, 6,  6,   6,
    6,   6,   6,   6,  55, 56, 68, 69, 6,  6,  76,  6,   6,   6,  6,  75, 6,   6,   6,   6,  64, 65};

void sprite_collidefunc(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    if (info.normal.x != 0)
        *info.dx = 0;
    if (info.normal.y != 0)
        *info.dy = 0;
}

void game_init(void) {
    g_test_image   = graphics_loadImageTable(162, 16, 16, pixelFormatPaletteRLE, tiles_data);
    g_sprite_image = graphics_loadImageTable(48, 12, 17, pixelFormatPalette, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImageTable(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 16, 16);
    tilemap_addTilemap(g_tilemap);

    sprite_setImageTable(g_sprite, g_sprite_image);
    sprite_setCollideRect(g_sprite, (Rect){0, 0, 40, 32});
    sprite_setCollisionResponseFunction(g_sprite, sprite_collidefunc);
    sprite_addSprite(g_sprite);
    sprite_moveTo(g_sprite, 0, 0);
    INFO("Game initialized!\n");
}

uint8_t dir = 0;

void game_loop(void) {
    // --- 输入处理 ---
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonLeft) {
        sprite_moveWithCollisions(g_sprite, -2, 0);
        dir = 2;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonRight) {
        sprite_moveWithCollisions(g_sprite, 2, 0);
        dir = 1;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonUp) {
        sprite_moveWithCollisions(g_sprite, 0, -2);
        dir = 3;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons & buttonDown) {
        sprite_moveWithCollisions(g_sprite, 0, 2);
        dir = 0;
        sprite_setAnimation(g_sprite, dir * 6 + 24, dir * 6 + 29, 5);
    }
    if (buttons == 0)
        sprite_setAnimation(g_sprite, dir * 6, dir * 6 + 5, 5);

    // --- 渲染 ---
    graphics_clear(0x2233);
    tilemap_drawAtPoint(g_tilemap, 0, 0, 0);
    sprite_updateAndDrawSprites();
    graphics_display();
}
