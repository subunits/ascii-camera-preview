#ifndef ASCII_CAMERA_PREVIEW_H
#define ASCII_CAMERA_PREVIEW_H

#include <stddef.h>
#include <stdint.h>

/**
 * Initialize ASCII preview with a target size in characters.
 * @param width_chars  Width of ASCII preview (characters).
 * @param height_chars Height of ASCII preview (characters).
 * @return 0 on success, -1 on failure.
 */
int ascii_camera_init(size_t width_chars, size_t height_chars);

/**
 * Render a grayscale frame to ASCII characters on stdout.
 * @param frame        Pointer to grayscale image (uint8_t, 0–255).
 * @param width_px     Image width in pixels.
 * @param height_px    Image height in pixels.
 * @param stride_bytes Number of bytes per row in the image.
 */
void ascii_camera_render(const uint8_t *frame,
                         size_t width_px,
                         size_t height_px,
                         size_t stride_bytes);

/**
 * Clear the terminal screen (ANSI escape).
 */
void ascii_camera_clear(void);

/**
 * Shutdown and reset ASCII preview state.
 */
void ascii_camera_shutdown(void);

#endif /* ASCII_CAMERA_PREVIEW_H */
