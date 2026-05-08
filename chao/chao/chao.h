#ifndef CHAO_H
#define CHAO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include "array.h"
#include "types.h"
#include "json.h"
#include "utils.h"
#include "dict.h"
#include "tweener.h"
#include "noise.h"
#include "renderer.h"
#include "assets.h"
#include "particles.h"
#include "nine_slice.h"
#include "input.h"
#include "ascii_font.h"
#include "button.h"
#include "popups.h"
#include "sprite.h"
#include "signal.h"

#define COMPANY_NAME "GamesByPiotr"
#define GAME_NAME "Mud Quest"

#ifdef PLATFORM_SDL
#include "platforms/platform_sdl.h"
#else
#include "platforms/platform_dummy.h"
#endif

#define MAX_DELTA_TIME 0.05f

typedef enum {
	CSM_EXPAND = 0,
	CSM_FIT,
} ChaoStretchMode;

int chao_fps = 0;

ChaoStretchMode chao_stretch_mode = CSM_FIT;
float chao_width_height_ratio = 0.5f;
Vector2Int chao_design_size;
Signal chao_resized;

void chao_on_window_resized(int width, int height) {
    int canvas_w = width;
    int canvas_h = height;
	Vector2 ratio = { (float)width / (float)chao_design_size.x, (float)height / (float)chao_design_size.y };

	switch (chao_stretch_mode) {
		case CSM_EXPAND: {
			Vector2 weight = { 1.f - chao_width_height_ratio, chao_width_height_ratio };
			float scale = ratio.x * weight.x + ratio.y * weight.y;
			canvas_w = (float)width / scale;
			canvas_h = (float)height / scale;
			break;
		}
		case CSM_FIT: {
			float scale = min(ratio.x, ratio.y);
			canvas_w = ((float)width / scale);
			canvas_h = ((float)height / scale);
			break;
		}
	}
    resize_bitmap(chao_canvas, canvas_w, canvas_h);
    platform_resize(canvas_w, canvas_h);
	popups_on_window_resize();
    signal_invoke(&chao_resized);
}

void chao_run(const char* window_name, int width, int height, void(*init_func_pointer)(), void(*update_func_pointer)(float dt)) {
    randomize();
	chao_resized = signal_create(16);
    chao_design_size = (Vector2Int){width, height};
    chao_canvas = create_bitmap(width, height);
    create_bitmap_z_buffer(chao_canvas);
    
    platform_init(window_name, width, height);
    platform_window_resized = &chao_on_window_resized;

    tweener_remove_all();
	popups_remove_all();

	chao_3d_fog.enabled = false;
	chao_3d_fog.color = 0x7f99b4ff;
	chao_3d_fog.start = 2.0f;
	chao_3d_fog.end = 5.0f;

	chao_3d_directional_light.enabled = false;
	chao_3d_directional_light.ambient = 0.2f;
	chao_3d_directional_light.direction = (Vector3){0,-1,1};

    if (init_func_pointer){
        (*init_func_pointer)();
    }
    uint32_t last_time = platform_get_time();
    Vector2Int last_mouse_position = {-1, -1};

	float fps_timer = 0.0f;
	int frames_counter = 0;

    do {
        uint32_t current_time = platform_get_time();
        double dt = (current_time - last_time) / 1000.f;
        dt = min(dt, MAX_DELTA_TIME);
        
        // calculate fps
        {
           frames_counter ++;
           fps_timer += dt;
           if (fps_timer >= 1.0f) {
           	   chao_fps = frames_counter;
           	   frames_counter = 0;
           	   fps_timer = 0;
		   }
		}
        
        last_time = current_time;

        input.mouse.moved_last_frame = last_mouse_position.x != input.mouse.x || last_mouse_position.y != input.mouse.y;
        last_mouse_position = (Vector2Int){input.mouse.x, input.mouse.y};
        
        tweener_update(dt);
        if (update_func_pointer != NULL){
            (*update_func_pointer)(dt * 1.0f);
        }
        
        platform_set_pixels(chao_canvas->pixels, screen_size.x, screen_size.y);
    } while(platform_update());
    
    platform_quit();

	assets_free_bitmaps();
	assets_free_model3ds();
    if (chao_canvas) {
        free_bitmap(chao_canvas);
        chao_canvas = NULL;
    }
    signal_free(&chao_resized);
}

#endif // CHAO_H
