#ifndef PLATFORM_CACA_H
#define PLATFORM_CACA_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <caca.h>

// Global libcaca state
static caca_canvas_t* canvas = NULL;
static caca_display_t* display = NULL;
static int current_width = 0;
static int current_height = 0;

// Callback for window resize (called by game logic)
void(*platform_window_resized)(int width, int height) = NULL;

// Resize the canvas when the terminal changes size
void platform_resize(int width, int height) {
    if (canvas && (width != current_width || height != current_height)) {
        caca_set_canvas_size(canvas, width, height);
        current_width = width;
        current_height = height;
        if (platform_window_resized) {
            platform_window_resized(width, height);
        }
    }
}

// Initialize the platform with a "window" (terminal canvas)
void platform_init(const char* window_name, int width, int height) {
    canvas = caca_create_canvas(width, height);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas\n");
        return;
    }
    
    display = caca_create_display(canvas);
    if (!display) {
        caca_free_canvas(canvas);
        fprintf(stderr, "Failed to create display\n");
        return;
    }
    
    current_width = width;
    current_height = height;
    caca_set_display_title(display, window_name);
}

// Set the window size (resize the canvas)
void platform_set_window_size(int width, int height) {
    platform_resize(width, height);
    caca_refresh_display(display);
}

// Update the platform, handle events, return false if quitting
bool platform_update() {
    caca_event_t ev;
    if (caca_get_event(display, CACA_EVENT_KEY_PRESS | CACA_EVENT_QUIT, &ev, 0)) {
        if (ev.type == CACA_EVENT_QUIT || 
            (ev.type == CACA_EVENT_KEY_PRESS && caca_get_event_key_ch(&ev) == 'q')) {
            return false; // Quit on 'q' or window close
        }
    }
    
    // Check for terminal resize
    int new_width = caca_get_canvas_width(canvas);
    int new_height = caca_get_canvas_height(canvas);
    if (new_width != current_width || new_height != current_height) {
        platform_resize(new_width, new_height);
    }
    
    return true;
}

// Render the pixel buffer to the canvas
void platform_set_pixels(uint32_t* pixels, int width, int height) {
    if (!canvas || width > current_width || height > current_height) {
        return; // Safety check
    }
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t p = pixels[y * width + x];
            // Extract RGB (assuming ARGB format: 0xAARRGGBB)
            uint8_t r = (p >> 16) & 0xFF;
            uint8_t g = (p >> 8) & 0xFF;
            uint8_t b = p & 0xFF;
            caca_set_color_argb(canvas, 0xFF, r, g, b); // Opaque
            caca_put_char(canvas, x, y, ' '); // Space for solid color
        }
    }
    caca_refresh_display(display);
}

uint32_t platform_get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void platform_quit() {
    if (display) {
        caca_free_display(display);
        display = NULL;
    }
    if (canvas) {
        caca_free_canvas(canvas);
        canvas = NULL;
    }
}

#endif // PLATFORM_CACA_H