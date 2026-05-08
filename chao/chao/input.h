#ifndef CHAO_INPUT_H
#define CHAO_INPUT_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define CHAO_MAX_KEYS 256

typedef enum { 
	KEY_A = 65, KEY_B = 66, KEY_C = 67, KEY_D = 68, KEY_E = 69, KEY_F = 70, KEY_G = 71, KEY_H = 72, KEY_I = 73, KEY_J = 74, KEY_K = 75, KEY_L = 76, KEY_M = 77, KEY_N = 78, KEY_O = 79, KEY_P = 80, KEY_Q = 81, KEY_R = 82, KEY_S = 83, KEY_T = 84, KEY_U = 85, KEY_V = 86, KEY_W = 87, KEY_X = 88, KEY_Y = 89, KEY_Z = 90, KEY_0 = 48, KEY_1 = 49, KEY_2 = 50, KEY_3 = 51, KEY_4 = 52, KEY_5 = 53, KEY_6 = 54, KEY_7 = 55, KEY_8 = 56, KEY_9 = 57, KEY_0_KEY_PAD = 96, KEY_1_KEY_PAD = 97, KEY_2_KEY_PAD = 98, KEY_3_KEY_PAD = 99, KEY_4_KEY_PAD = 100, KEY_5_KEY_PAD = 101, KEY_6_KEY_PAD = 102, KEY_7_KEY_PAD = 103, KEY_8_KEY_PAD = 104, KEY_9_KEY_PAD = 105, KEY_F1 = 112, KEY_F2 = 113, KEY_F3 = 114, KEY_F4 = 115, KEY_F5 = 116, KEY_F6 = 117, KEY_F7 = 118, KEY_F8 = 119, KEY_F9 = 120, KEY_F10 = 121, KEY_F11 = 122, KEY_F12 = 123, KEY_ESC = 27, KEY_TILDE = 192, KEY_MINUS = 189, KEY_EQUALS = 187, KEY_BACKSPACE = 8, KEY_TAB = 9, KEY_OPENBRACE = 219, KEY_CLOSEBRACE = 221, KEY_ENTER = 13, KEY_COLON = 186, KEY_QUOTE = 222, KEY_BACKSLASH = 220, KEY_COMMA = 188, KEY_STOP = 190, KEY_SLASH = 191, KEY_SPACE = 32, KEY_INSERT = 45, KEY_DEL = 46, KEY_HOME = 36, KEY_END = 35, KEY_PGUP = 33, KEY_PGDN = 34, KEY_LEFT = 37, KEY_RIGHT = 39, KEY_UP = 38, KEY_DOWN = 40, SLASH_KEY_PAD = 111, KEY_ASTERISK = 106, MINUS_KEY_PAD = 109, PLUS_KEY_PAD = 107, ENTER_KEY_PAD = 13, KEY_PRTSCR = 44, KEY_PAUSE = 19, EQUALS_KEY_PAD = 12, KEY_LSHIFT = 16, KEY_RSHIFT = 16, KEY_LCONTROL = 17, KEY_RCONTROL = 17, KEY_ALT = 18, KEY_ALTGR = 18, KEY_LWIN = 91, KEY_RWIN = 92, KEY_MENU = 93, KEY_SCRLOCK = 157, KEY_NUMLOCK = 144, KEY_CAPSLOCK = 20
} Key;

typedef struct {
    int x;
    int y;
    int wheel_delta;
    bool pressed;
    bool just_pressed;
    bool just_released;
    bool pressed_right;
    bool just_pressed_right;
    bool just_released_right;
    bool pressed_middle;
    bool just_pressed_middle;
    bool just_repeased_middle;
    bool moved_last_frame;
} ChaoMouse;

struct {
    bool pressed[CHAO_MAX_KEYS];
    bool just_pressed[CHAO_MAX_KEYS];
    bool just_released[CHAO_MAX_KEYS];
    ChaoMouse mouse;
} input;

bool mouse_above(int x, int y, int w, int h) {
    return input.mouse.x >= x && input.mouse.x <= x+w && input.mouse.y >= y && input.mouse.y <= y+h;
}

void chao_input_reset_keys(bool* keys_ptr) {
    for (int i = 0; i < CHAO_MAX_KEYS; ++i) {
        keys_ptr[i] = false;
    }
}

void chao_input_init() {
    input.mouse.x = -1;
    input.mouse.y = -1;
    input.mouse.wheel_delta = false;
    input.mouse.pressed = false;
    input.mouse.just_pressed = false;
    input.mouse.just_released = false;
    input.mouse.pressed_right = false;
    input.mouse.just_pressed_right = false;
    input.mouse.just_released_right = false;
    input.mouse.pressed_middle = false;
    input.mouse.just_pressed_middle = false;
    input.mouse.just_repeased_middle = false;
    
    chao_input_reset_keys(input.pressed);
    chao_input_reset_keys(input.just_pressed);
    chao_input_reset_keys(input.just_released);
}





#endif // CHAO_INPUT_H
