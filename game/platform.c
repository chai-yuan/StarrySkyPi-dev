#include "MicroCoreEngine/engine/Debug.h"
#include "MicroCoreEngine/engine/PlatformPorting.h"
#include "device/gpio.h"
#include "device/spi.h"
#include "device/timer.h"
#include <stdio.h>

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

void send_package(const uint8_t *data, uint16_t len);
void send_package_buf(const uint8_t *data, uint16_t len, int send_now);

static void            *wrapper_mem_alloc(size_t size);
static void             wrapper_mem_free(void *ptr);
static unsigned int     wrapper_get_ticks_ms(void);
static unsigned int     wrapper_get_button_state(void);
static platform_image_t wrapper_gfx_create_image(int width, int height, PixelFormat format, const void *data);
static void             wrapper_gfx_destroy_image(platform_image_t image);
static void             wrapper_gfx_set_render_target(platform_image_t image);
static void             wrapper_gfx_set_clip_rect(Rect rect);
static void             wrapper_gfx_clear(Color color);
static void             wrapper_gfx_present(void);
static void wrapper_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip);
static void wrapper_gfx_draw_rect(Rect rect, Color color);
static int  wrapper_display_get_width(void);
static int  wrapper_display_get_height(void);

void game_platform_init() {
    platform_api_t wrapper_api = {// Memory
                                  .mem_alloc = wrapper_mem_alloc,
                                  .mem_free  = wrapper_mem_free,
                                  // Time & Input (Direct mapping)
                                  .get_ticks_ms     = wrapper_get_ticks_ms,
                                  .get_button_state = wrapper_get_button_state,
                                  // Graphics Resource Management
                                  .gfx_create_image   = wrapper_gfx_create_image,
                                  .gfx_update_image   = NULL, // Not implemented for simplicity
                                  .gfx_update_palette = NULL,
                                  .gfx_destroy_image  = wrapper_gfx_destroy_image,
                                  // Rendering State
                                  .gfx_set_render_target = wrapper_gfx_set_render_target,
                                  .gfx_set_clip_rect     = wrapper_gfx_set_clip_rect,
                                  // Drawing
                                  .gfx_clear      = wrapper_gfx_clear,
                                  .gfx_present    = wrapper_gfx_present,
                                  .gfx_draw_image = wrapper_gfx_draw_image,
                                  .gfx_draw_rect  = wrapper_gfx_draw_rect,
                                  // Display Info
                                  .display_get_width  = wrapper_display_get_width,
                                  .display_get_height = wrapper_display_get_height};

    platform_register_api(wrapper_api);
}

static void *wrapper_mem_alloc(size_t size) {
    #define VMEM_SIZE 1024 * 8
    static uint8_t  vmem[VMEM_SIZE] = {0};
    static uint32_t vmem_ptr = 0;

    if (vmem_ptr + size >= VMEM_SIZE) {
        WARN("mem_alloc error : %d/%d", vmem_ptr, VMEM_SIZE);
    }
    void *ptr = vmem + vmem_ptr;
    vmem_ptr += size;
    return ptr;
}

static void wrapper_mem_free(void *ptr) {}

static unsigned int wrapper_get_ticks_ms(void) { return 0; }

static unsigned int wrapper_get_button_state(void) {
    unsigned int state = 0;
    if (gpio_digitalRead(GPIO1, 7))
        state |= buttonLeft;
    if (gpio_digitalRead(GPIO1, 10))
        state |= buttonRight;
    return state;
}

static platform_image_t wrapper_gfx_create_image(int width, int height, PixelFormat format, const void *data) {
    static uint8_t image_num = 1;
    if (data) {
        uint16_t cmd[] = {0, width, height, format};
        send_package((uint8_t*)cmd, sizeof(cmd));
        send_package(data, width * height / 2);
    } else {
        WARN("TODO");
    }

    return image_num++;
}

static void wrapper_gfx_destroy_image(platform_image_t image) { WARN("TODO"); }

static void wrapper_gfx_set_render_target(platform_image_t image) { WARN("TODO"); }

static void wrapper_gfx_set_clip_rect(Rect rect) { WARN("TODO"); }

static void wrapper_gfx_clear(Color color) { WARN("TODO"); }

static void wrapper_gfx_present(void) {
    uint16_t cmd[] = {11};
    send_package_buf((uint8_t*)cmd, sizeof(cmd),1);
}

static void wrapper_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    if(src_rect == NULL){

    }else{
        uint16_t cmd[] = {10, image, src_rect->x, src_rect->y, src_rect->w, src_rect->h , x, y, flip};
        send_package_buf((uint8_t*)cmd, sizeof(cmd),0);
    }
}

static void wrapper_gfx_draw_rect(Rect rect, Color color) { WARN("wrapper_gfx_draw_rect not implemented"); }

static int wrapper_display_get_width(void) { return DISPLAY_WIDTH; }

static int wrapper_display_get_height(void) { return DISPLAY_HEIGHT; }
