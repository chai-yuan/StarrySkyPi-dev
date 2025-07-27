#include "MicroCoreEngine/engine/Debug.h"
#include "MicroCoreEngine/engine/PlatformPorting.h"
#include "device/gpio.h"
#include "device/timer.h"
#include "device/spi.h"
#include <stdio.h>

typedef struct {
    int      width;
    int      height;
    uint8_t *pixels;
} InternalImage;

#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 96
static uint16_t       vmem_display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
static InternalImage *current_render_target = NULL;
static Rect           current_clip_rect;
static uint16_t       default_palette[16] = {0x0,    0xef18, 0xb9c9, 0x7db6, 0x49ea, 0x6d2d, 0x2a0d, 0xddcb,
                                             0xb46d, 0x42cb, 0xbb09, 0x31a6, 0x73ae, 0x8d4c, 0x3df9, 0xbdd7};

// 从指定图片位置获得像素
static inline uint16_t getImagePixel(InternalImage *image, int x, int y);
// 向目标绘制一个像素
static inline void drawPixel(int x, int y, uint16_t pixel);

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
    current_clip_rect.x = 0;
    current_clip_rect.y = 0;
    current_clip_rect.w = DISPLAY_WIDTH;
    current_clip_rect.h = DISPLAY_HEIGHT;

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

#define VMEM_SIZE 1024 * 16
static uint8_t  vmem[VMEM_SIZE];
static uint32_t vmem_ptr = 0;

static void *wrapper_mem_alloc(size_t size) {
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
    unsigned int state          = 0;
    if (gpio_digitalRead(GPIO1, 7))
        state |= buttonLeft;
    if (gpio_digitalRead(GPIO1, 10))
        state |= buttonRight;
    return state;
}

static platform_image_t wrapper_gfx_create_image(int width, int height, PixelFormat format, const void *data) {
    InternalImage *img = (InternalImage *)wrapper_mem_alloc(sizeof(InternalImage));
    if (!img)
        return NULL;

    int size = 0;
    if (format == pixelFormatPalette) {
        size = width * height / 2;
    } else {
        ERROR("TODO");
    }

    img->width  = width;
    img->height = height;
    img->pixels = (uint8_t *)wrapper_mem_alloc(size);
    if (!img->pixels) {
        WARN("Not enough space for pixel data");
        return NULL;
    }

    if (data) {
        for (int i = 0; i < size; i++)
            img->pixels[i] = ((const uint8_t *)data)[i];
    }

    return (platform_image_t)img;
}

static void wrapper_gfx_destroy_image(platform_image_t image) {
    // No-op
}

static void wrapper_gfx_set_render_target(platform_image_t image) { current_render_target = (InternalImage *)image; }

static void wrapper_gfx_set_clip_rect(Rect rect) { current_clip_rect = rect; }

static void wrapper_gfx_clear(Color color) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            drawPixel(x, y, color);
        }
    }
}

static void wrapper_gfx_present(void) {
    while (gpio_digitalRead(GPIO0, 27) == 0){
        timer_softDelay(10000);
        printf("waiting\n");
    }
    printf("send\n");

    uint16_t len = DISPLAY_HEIGHT * DISPLAY_WIDTH * 2;
    spi_transfer(len & 0xff);
    spi_transfer(len >> 8);
    timer_softDelay(10000);

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint16_t pixel_color = vmem_display[y][x];
            spi_transfer(pixel_color & 0xff);
            spi_transfer(pixel_color >> 8);
        }
    }
}

static void wrapper_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    InternalImage *img = (InternalImage *)image;
    if (img == NULL || img->pixels == NULL) {
        return;
    }

    // 确定源矩形
    Rect source_rect;
    if (src_rect == NULL) {
        // 如果 src_rect 为 NULL，则使用整个图像
        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = img->width;
        source_rect.h = img->height;
    } else {
        source_rect = *src_rect;
    }

    // 计算最终要绘制的区域 (blit: block image transfer)
    int blit_w = source_rect.w;
    int blit_h = source_rect.h;

    int dest_x       = x;
    int dest_y       = y;
    int src_offset_x = 0;
    int src_offset_y = 0;

    for (int j = 0; j < blit_h; ++j) {
        for (int i = 0; i < blit_w; ++i) {
            int src_x;
            int src_y;

            int read_i = i;
            if (flip == imageFlippedX || flip == imageFlippedXY) {
                read_i = (blit_w - 1) - i;
            }

            int read_j = j;
            if (flip == imageFlippedY || flip == imageFlippedXY) {
                read_j = (blit_h - 1) - j;
            }

            src_x = source_rect.x + src_offset_x + read_i;
            src_y = source_rect.y + src_offset_y + read_j;

            uint16_t pixel_color = getImagePixel(img, src_x, src_y);

            int screen_x = dest_x + i;
            int screen_y = dest_y + j;

            drawPixel(screen_x, screen_y, pixel_color);
        }
    }
}

static void wrapper_gfx_draw_rect(Rect rect, Color color) { WARN("wrapper_gfx_draw_rect not implemented"); }

static int wrapper_display_get_width(void) { return DISPLAY_WIDTH; }

static int wrapper_display_get_height(void) { return DISPLAY_HEIGHT; }

static inline uint16_t getImagePixel(InternalImage *image, int x, int y) {
    if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
        return 0;
    }
    uint8_t idx = image->pixels[(x + y * image->width) / 2];
    if (x % 2) {
        return default_palette[idx >> 4];
    } else {
        return default_palette[idx & 0xf];
    }
}

static inline void drawPixel(int x, int y, uint16_t pixel) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        return;
    }
    vmem_display[y][x] = pixel;
}
