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
uint8_t          mapData[8][8]  = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 7, 7, 0, 0}, {0, 0, 0, 0, 6, 7, 0, 0},
                                   {0, 0, 0, 0, 6, 7, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 11, 0, 0, 0, 12},
                                   {1, 1, 1, 1, 1, 1, 3, 3}, {2, 2, 2, 3, 3, 4, 4, 4}};

void sprite_collidefunc(SpriteHandle self, SpriteHandle other, CollisionInfo info) {
    if (info.normal.x != 0)
        *info.dx = 0;
    if (info.normal.y != 0)
        *info.dy = 0;
}

void game_init(void) {
    g_test_image   = graphics_loadImageTable(12, 16, 16, pixelFormatPalette, tiles_data);
    g_sprite_image = graphics_loadImageTable(6, 16, 20, pixelFormatPalette, my_sprite_data);
    g_tilemap      = tilemap_newTilemap();
    g_sprite       = sprite_newSprite();
    tilemap_setImageTable(g_tilemap, g_test_image);
    tilemap_setTiles(g_tilemap, (uint8_t *)mapData, 8, 8);
    tilemap_addTilemap(g_tilemap);

    sprite_setImage(g_sprite, graphics_getTableImage(g_sprite_image, 0), imageUnflipped);
    sprite_setCollideRect(g_sprite, (Rect){0, 0, 40, 32});
    sprite_setCollisionResponseFunction(g_sprite, sprite_collidefunc);
    sprite_addSprite(g_sprite);
    sprite_moveTo(g_sprite, 0, 0);
    INFO("Game initialized!\n");
}

void game_loop(void) {
    // --- 输入处理 ---
    unsigned int buttons = system_getButtonState();
    if (buttons & buttonLeft) {
        sprite_moveWithCollisions(g_sprite, -4, 0);
        sprite_setImageFlip(g_sprite, imageFlippedX);
    }
    if (buttons & buttonRight) {
        sprite_moveWithCollisions(g_sprite, 4, 0);
        sprite_setImageFlip(g_sprite, imageUnflipped);
    }
    if (buttons & buttonUp)
        sprite_moveWithCollisions(g_sprite, 0, -4);
    if (buttons & buttonDown)
        sprite_moveWithCollisions(g_sprite, 0, 4);

    // --- 渲染 ---
    graphics_clear(0);
    tilemap_drawAtPoint(g_tilemap, 0, 0, 0);
    sprite_updateAndDrawSprites();
    graphics_display();
}
