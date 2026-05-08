#ifndef PLATFORM_SDL_H
#define PLATFORM_SDL_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
//#include <SDL2_mixer/SDL_mixer.h>
#include "../input.h"

SDL_Window* sdl_window;
SDL_Renderer* sdl_renderer;
SDL_Texture *sdl_texture;

int sdl_to_custom_key(SDL_Keycode sdl_keycode);

void(*platform_window_resized)(int width, int height) = NULL;

void platform_resize(int width, int height) {
    if (sdl_texture) {
        SDL_DestroyTexture(sdl_texture);
    }
    sdl_texture = SDL_CreateTexture(
        sdl_renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        width, 
        height
    );
    SDL_RenderSetLogicalSize(sdl_renderer, width, height);
}

void platform_init(const char* window_name, int width, int height) {
    SDL_Init(SDL_INIT_EVERYTHING);
    sdl_window = SDL_CreateWindow(window_name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_RESIZABLE);
    sdl_renderer = SDL_CreateRenderer( sdl_window, -1, SDL_RENDERER_ACCELERATED);
    // sdl_renderer = SDL_CreateRenderer( sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetViewport( sdl_renderer, NULL);
    
    platform_resize(width, height);
}

void platform_set_window_size(int width, int height) {
    SDL_SetWindowSize(sdl_window, width, height);
    
    int displayIndex = SDL_GetWindowDisplayIndex(sdl_window);
    if (displayIndex >= 0) {
        SDL_Rect displayBounds;
        if (SDL_GetDisplayBounds(displayIndex, &displayBounds) == 0) {
            int newX = displayBounds.x + (displayBounds.w - width) / 2;
            int newY = displayBounds.y + (displayBounds.h - height) / 2;
            SDL_SetWindowPosition(sdl_window, newX, newY);
        }
    }
}

bool platform_update() {
    
    SDL_Event event;
    
    input.mouse.wheel_delta = 0;
    input.mouse.just_pressed = false;
    input.mouse.just_released = false;
    input.mouse.just_pressed_right = false;
    input.mouse.just_released_right = false;
    input.mouse.just_pressed_middle = false;
    input.mouse.just_repeased_middle = false;
    chao_input_reset_keys(input.just_pressed);
    chao_input_reset_keys(input.just_released);
    
    bool should_quit = false;
    
    while( SDL_PollEvent( &event ) != 0 ) {
        switch (event.type) {
            case SDL_QUIT: {
                should_quit = true;
                break;
            }
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        int width = event.window.data1;
                        int height = event.window.data2;
                        
                        if (platform_window_resized) {
                            (*platform_window_resized)(width, height);
                        }
                        
                        SDL_RenderSetViewport( sdl_renderer, NULL);
                        SDL_RenderPresent( sdl_renderer );
                        break;
                    }
                    break;
                }
            }
            case SDL_KEYDOWN: {
                SDL_Keycode code = sdl_to_custom_key(event.key.keysym.sym);
                if (event.key.repeat == 0 && code >= 0 && code < CHAO_MAX_KEYS) {
                    input.pressed[code] = true;
                    input.just_pressed[code] = true;
                }
                break;
            }
            case SDL_KEYUP: {
                SDL_Keycode code = sdl_to_custom_key(event.key.keysym.sym);
                if (code >= 0 && code < CHAO_MAX_KEYS) {
                    input.pressed[code] = false;
                    input.just_released[code] = true;
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                input.mouse.x = event.motion.x;
                input.mouse.y = event.motion.y;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    input.mouse.pressed = true;
                    input.mouse.just_pressed = true;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    input.mouse.pressed_right = true;
                    input.mouse.just_pressed_right = true;
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    input.mouse.pressed_middle = true;
                    input.mouse.just_pressed_middle = true;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    input.mouse.pressed = false;
                    input.mouse.just_released = true;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    input.mouse.pressed_right = false;
                    input.mouse.just_released_right = true;
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    input.mouse.pressed_middle = false;
                    input.mouse.just_repeased_middle = true;
                }
                break;
            }
            case SDL_MOUSEWHEEL: {
                input.mouse.wheel_delta = event.wheel.y;
                break;
            }
        }
    }
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
    
    return !should_quit;
}

void platform_set_pixels(uint32_t* pixels, int width, int height) {
    SDL_UpdateTexture(sdl_texture, NULL, pixels, width * sizeof(uint32_t));
}

Uint32 platform_get_time() {
    return SDL_GetTicks();
}

char* platform_load_file(const char* path) {
    SDL_RWops* file = SDL_RWFromFile(path, "rb");
    if (!file) return NULL;
    
    Sint64 file_size = SDL_RWsize(file);
    if (file_size < 0) {
        SDL_RWclose(file);
        return NULL;
    }
    
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        SDL_RWclose(file);
        return NULL;
    }
    
    Sint64 bytes_read = SDL_RWread(file, buffer, 1, file_size);
    if (bytes_read != file_size) {
        free(buffer);
        SDL_RWclose(file);
        return NULL;
    }
    buffer[file_size] = '\0';
    
    SDL_RWclose(file);
    return buffer;
}

bool platform_save(const char* file_name, char* data) {
    char* pref_path = SDL_GetPrefPath(COMPANY_NAME, GAME_NAME);
    if (!pref_path) {
        fprintf(stderr, "Failed to get pref path: %s\n", SDL_GetError());
        return false;
    }

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", pref_path, file_name);
    SDL_free(pref_path);

    SDL_RWops* file = SDL_RWFromFile(full_path, "w");
    if (!file) {
        //fprintf(stderr, "Failed to open file for writing: %s (%s)\n", full_path, SDL_GetError());
        return false;
    }

    size_t data_len = strlen(data);
    Sint64 bytes_written = SDL_RWwrite(file, data, 1, data_len);
    SDL_RWclose(file);

    if (bytes_written != data_len) {
        fprintf(stderr, "Failed to write all data to file: %s (%s)\n", full_path, SDL_GetError());
        return false;
    }

    return true;
}

char* platform_load(const char* file_name) {
    char* pref_path = SDL_GetPrefPath(COMPANY_NAME, GAME_NAME);
    if (!pref_path) {
        fprintf(stderr, "Failed to get pref path: %s\n", SDL_GetError());
        return NULL;
    }

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", pref_path, file_name);
    SDL_free(pref_path);

    SDL_RWops* file = SDL_RWFromFile(full_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file for reading: %s (%s)\n", full_path, SDL_GetError());
        return NULL;
    }

    Sint64 file_size = SDL_RWsize(file);
    if (file_size < 0) {
        fprintf(stderr, "Failed to get file size: %s (%s)\n", full_path, SDL_GetError());
        SDL_RWclose(file);
        return NULL;
    }

    char* data = (char*)malloc(file_size + 1);
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for file contents\n");
        SDL_RWclose(file);
        return NULL;
    }

    Sint64 bytes_read = SDL_RWread(file, data, 1, file_size);
    SDL_RWclose(file);

    if (bytes_read != file_size) {
        fprintf(stderr, "Failed to read entire file: %s (%s)\n", full_path, SDL_GetError());
        free(data);
        return NULL;
    }

    data[bytes_read] = '\0'; // Null-terminate the string
    return data;
}

void platform_quit() {
    if (sdl_texture) {
        SDL_DestroyTexture(sdl_texture);
    }
    SDL_DestroyRenderer( sdl_renderer );
    SDL_DestroyWindow( sdl_window );
    sdl_renderer = NULL;
    sdl_window = NULL;
    SDL_Quit();
}


int sdl_to_custom_key(SDL_Keycode sdl_keycode) {
    switch (sdl_keycode) {
        case SDLK_a: return KEY_A;
        case SDLK_b: return KEY_B;
        case SDLK_c: return KEY_C;
        case SDLK_d: return KEY_D;
        case SDLK_e: return KEY_E;
        case SDLK_f: return KEY_F;
        case SDLK_g: return KEY_G;
        case SDLK_h: return KEY_H;
        case SDLK_i: return KEY_I;
        case SDLK_j: return KEY_J;
        case SDLK_k: return KEY_K;
        case SDLK_l: return KEY_L;
        case SDLK_m: return KEY_M;
        case SDLK_n: return KEY_N;
        case SDLK_o: return KEY_O;
        case SDLK_p: return KEY_P;
        case SDLK_q: return KEY_Q;
        case SDLK_r: return KEY_R;
        case SDLK_s: return KEY_S;
        case SDLK_t: return KEY_T;
        case SDLK_u: return KEY_U;
        case SDLK_v: return KEY_V;
        case SDLK_w: return KEY_W;
        case SDLK_x: return KEY_X;
        case SDLK_y: return KEY_Y;
        case SDLK_z: return KEY_Z;

        case SDLK_0: return KEY_0;
        case SDLK_1: return KEY_1;
        case SDLK_2: return KEY_2;
        case SDLK_3: return KEY_3;
        case SDLK_4: return KEY_4;
        case SDLK_5: return KEY_5;
        case SDLK_6: return KEY_6;
        case SDLK_7: return KEY_7;
        case SDLK_8: return KEY_8;
        case SDLK_9: return KEY_9;

        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_RIGHT: return KEY_RIGHT;

        case SDLK_LSHIFT: return KEY_LSHIFT;
        case SDLK_RSHIFT: return KEY_RSHIFT;
        
        case SDLK_LCTRL: return KEY_LCONTROL;
        case SDLK_RCTRL: return KEY_RCONTROL;

        case SDLK_F1: return KEY_F1;
        case SDLK_F2: return KEY_F2;
        case SDLK_F3: return KEY_F3;
        case SDLK_F4: return KEY_F4;
        case SDLK_F5: return KEY_F5;
        case SDLK_F6: return KEY_F6;
        case SDLK_F7: return KEY_F7;
        case SDLK_F8: return KEY_F8;
        case SDLK_F9: return KEY_F9;
        case SDLK_F10: return KEY_F10;
        case SDLK_F11: return KEY_F11;
        case SDLK_F12: return KEY_F12;

        case SDLK_ESCAPE: return KEY_ESC;
        case SDLK_TAB: return KEY_TAB;
        case SDLK_BACKSPACE: return KEY_BACKSPACE;
        case SDLK_RETURN: return KEY_ENTER;

        case SDLK_SPACE: return KEY_SPACE;
        case SDLK_MINUS: return KEY_MINUS;
        case SDLK_EQUALS: return KEY_EQUALS;
        case SDLK_COMMA: return KEY_COMMA;
        case SDLK_PERIOD: return KEY_STOP;
        case SDLK_SLASH: return KEY_SLASH;
        case SDLK_SEMICOLON: return KEY_COLON;
        case SDLK_QUOTE: return KEY_QUOTE;
        case SDLK_LEFTBRACKET: return KEY_OPENBRACE;
        case SDLK_RIGHTBRACKET: return KEY_CLOSEBRACE;
        case SDLK_BACKSLASH: return KEY_BACKSLASH;

        case SDLK_KP_0: return KEY_0_KEY_PAD;
        case SDLK_KP_1: return KEY_1_KEY_PAD;
        case SDLK_KP_2: return KEY_2_KEY_PAD;
        case SDLK_KP_3: return KEY_3_KEY_PAD;
        case SDLK_KP_4: return KEY_4_KEY_PAD;
        case SDLK_KP_5: return KEY_5_KEY_PAD;
        case SDLK_KP_6: return KEY_6_KEY_PAD;
        case SDLK_KP_7: return KEY_7_KEY_PAD;
        case SDLK_KP_8: return KEY_8_KEY_PAD;
        case SDLK_KP_9: return KEY_9_KEY_PAD;

        default: return -1;
    }
}

#endif //PLATFORM_SDL_H
