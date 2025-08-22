#include "ascii_camera_preview.h"
#include <stdio.h>
#include <stdlib.h>

static size_t g_width_chars = 0;
static size_t g_height_chars = 0;

static const char *ASCII_GRADIENT = " .:-=+*#%@";
static const size_t GRADIENT_LEN = 10;

int ascii_camera_init(size_t width_chars, size_t height_chars) {
    if (width_chars == 0 || height_chars == 0) return -1;
    g_width_chars = width_chars;
    g_height_chars = height_chars;
    return 0;
}

void ascii_camera_clear(void) {
    printf("\033[2J\033[H"); // ANSI escape: clear screen, move cursor home
}

void ascii_camera_render(const uint8_t *frame,
                         size_t width_px,
                         size_t height_px,
                         size_t stride_bytes) {
    if (!frame || g_width_chars == 0 || g_height_chars == 0) return;

    size_t step_x = width_px / g_width_chars;
    size_t step_y = height_px / g_height_chars;
    if (step_x == 0) step_x = 1;
    if (step_y == 0) step_y = 1;

    for (size_t y = 0; y < height_px; y += step_y) {
        for (size_t x = 0; x < width_px; x += step_x) {
            uint8_t pixel = frame[y * stride_bytes + x];
            size_t idx = (pixel * (GRADIENT_LEN - 1)) / 255;
            putchar(ASCII_GRADIENT[idx]);
        }
        putchar('\n');
    }
}

void ascii_camera_shutdown(void) {
    g_width_chars = 0;
    g_height_chars = 0;
}
