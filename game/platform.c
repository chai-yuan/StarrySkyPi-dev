#include "MicroCoreEngine/engine/Debug.h"
#include "MicroCoreEngine/engine/PlatformPorting.h"
#include <stdio.h>

#define VMEM_SIZE 1024 * 20
static uint8_t vmem[VMEM_SIZE];
static size_t  vmem_ptr = 0;

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

#define STUB_FUNC_PRINT() printf("STUB: %s() is not implemented.\n", __func__)

static void *stub_mem_alloc(size_t size) {
    size = (size + 3) & ~3;
    INFO("ptr : %d", (uint32_t)vmem_ptr);
    if (vmem_ptr + size > VMEM_SIZE) {
        WARN("Out of memory! %d/%d alloc : %d", (uint32_t)vmem_ptr, VMEM_SIZE, (uint32_t)size);
        return NULL;
    }
    void *ptr = vmem + vmem_ptr;
    vmem_ptr += size;
    return ptr;
}

static void stub_mem_free(void *ptr) { STUB_FUNC_PRINT(); }

// --- 时间 & 输入桩函数 ---
static unsigned int stub_get_ticks_ms(void) {
    STUB_FUNC_PRINT();
    return 0;
}

static unsigned int stub_get_button_state(void) {
    STUB_FUNC_PRINT();
    return 0; // 0 表示没有按钮被按下
}

// --- 图形资源管理桩函数 ---
static platform_image_t stub_gfx_create_image(int width, int height, const void *data) {
    STUB_FUNC_PRINT();
    return NULL; // 返回NULL表示创建失败
}

static void stub_gfx_update_image(platform_image_t image, Rect update_rect, const void *data) { STUB_FUNC_PRINT(); }

static platform_image_t stub_gfx_create_render_target(int width, int height) {
    STUB_FUNC_PRINT();
    return NULL; // 返回NULL表示创建失败
}

static void stub_gfx_destroy_image(platform_image_t image) { STUB_FUNC_PRINT(); }

// --- 渲染状态控制桩函数 ---
static void stub_gfx_set_render_target(platform_image_t image) { STUB_FUNC_PRINT(); }

static void stub_gfx_set_clip_rect(Rect rect) { STUB_FUNC_PRINT(); }

// --- 绘图操作桩函数 ---
static void stub_gfx_clear(Color color) { STUB_FUNC_PRINT(); }

static void stub_gfx_present(void) { STUB_FUNC_PRINT(); }

static void stub_gfx_draw_image(platform_image_t image, const Rect *src_rect, int x, int y, ImageFlip flip) {
    STUB_FUNC_PRINT();
}

static void stub_gfx_draw_line(Point p1, Point p2, int width, Color color) { STUB_FUNC_PRINT(); }

static void stub_gfx_draw_rect(Rect rect, Color color) { STUB_FUNC_PRINT(); }

static int stub_display_get_width(void) { return DISPLAY_WIDTH; }

static int stub_display_get_height(void) { return DISPLAY_HEIGHT; }

void game_platform_init() {
    platform_api_t wrapper_api = {// Memory
                                  .mem_alloc = stub_mem_alloc,
                                  .mem_free  = stub_mem_free,
                                  // Time & Input
                                  .get_ticks_ms     = stub_get_ticks_ms,
                                  .get_button_state = stub_get_button_state,
                                  // Graphics Resource Management
                                  .gfx_create_image         = stub_gfx_create_image,
                                  .gfx_update_image         = stub_gfx_update_image,
                                  .gfx_create_render_target = stub_gfx_create_render_target,
                                  .gfx_destroy_image        = stub_gfx_destroy_image,
                                  // Rendering State
                                  .gfx_set_render_target = stub_gfx_set_render_target,
                                  .gfx_set_clip_rect     = stub_gfx_set_clip_rect,
                                  // Drawing
                                  .gfx_clear      = stub_gfx_clear,
                                  .gfx_present    = stub_gfx_present,
                                  .gfx_draw_image = stub_gfx_draw_image,
                                  .gfx_draw_line  = stub_gfx_draw_line,
                                  .gfx_draw_rect  = stub_gfx_draw_rect,
                                  // Display Info
                                  .display_get_width  = stub_display_get_width,
                                  .display_get_height = stub_display_get_height};

    platform_register_api(wrapper_api);
}
