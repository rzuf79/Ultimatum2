#ifndef PLATFORM_DUMMY_H
#define PLATFORM_DUMMY_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

void(*platform_window_resized)(int width, int height) = NULL;
void platform_resize(int width, int height){}
void platform_init(const char* window_name, int width, int height) {}
void platform_set_window_size(int width, int height) {}
bool platform_update() { return true; }
void platform_set_pixels(uint32_t* pixels, int width, int height) {}
uint32_t platform_get_time() { clock() / CLOCKS_PER_SEC;}
void platform_quit() {}

#endif //PLATFORM_DUMMY_H
