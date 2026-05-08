#ifndef PLATFORM_WINAPI_H
#define PLATFORM_WINAPI_H

#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <shlobj.h> // for SHGetFolderPathA
#include "../input.h" // your input definitions

static HWND            win_window       = NULL;
static HDC             win_dc           = NULL;
static BITMAPINFO      win_bmi          = {0};
static int             win_buffer_w     = 0;
static int             win_buffer_h     = 0;
static uint32_t*       win_pixel_buffer = NULL;

void (*platform_window_resized)(int width, int height) = NULL;

int win_to_custom_key(WPARAM vk);
static LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

static void platform_resize(int width, int height) {
    win_buffer_w = width;
    win_buffer_h = height;
    ZeroMemory(&win_bmi, sizeof(win_bmi));
    win_bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    win_bmi.bmiHeader.biWidth       = width;
    win_bmi.bmiHeader.biHeight      = -height;  // top-down
    win_bmi.bmiHeader.biPlanes      = 1;
    win_bmi.bmiHeader.biBitCount    = 32;
    win_bmi.bmiHeader.biCompression = BI_RGB;
    if (platform_window_resized) {
        platform_window_resized(width, height);
    }
}

static void platform_init(const char* window_name, int width, int height) {
    HINSTANCE hInst = GetModuleHandleA(NULL);

    WNDCLASSA wc = {0};
    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WinProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "ChaoWinClass";
    RegisterClassA(&wc);

    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    win_window = CreateWindowExA(
        0,
        wc.lpszClassName,
        window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL, NULL, hInst, NULL
    );

    win_dc = GetDC(win_window);

    platform_resize(width, height);
}

static void platform_set_window_size(int width, int height) {
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, GetWindowLong(win_window, GWL_STYLE), FALSE);
    int win_w = rc.right - rc.left;
    int win_h = rc.bottom - rc.top;

    // center on primary monitor
    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    int x = (screen_w  - win_w) / 2;
    int y = (screen_h - win_h) / 2;

    SetWindowPos(win_window, NULL, x, y, win_w, win_h, SWP_NOZORDER);
}

static bool platform_update(void) {
    input.mouse.wheel_delta      = 0;
    input.mouse.just_pressed     = false;
    input.mouse.just_released    = false;
    input.mouse.just_pressed_right  = false;
    input.mouse.just_released_right = false;
    input.mouse.just_pressed_middle = false;
    input.mouse.just_repeased_middle = false;
    chao_input_reset_keys(input.just_pressed);
    chao_input_reset_keys(input.just_released);

    bool should_quit = false;
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            should_quit = true;
        } else {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    if (win_pixel_buffer) {
        RECT client;
        GetClientRect(win_window, &client);
        StretchDIBits(
            win_dc,
            0, 0, client.right, client.bottom,
            0, 0, win_buffer_w, win_buffer_h,
            win_pixel_buffer,
            &win_bmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }

    return !should_quit;
}

static void platform_set_pixels(uint32_t* pixels, int width, int height) {
    win_pixel_buffer = pixels;
    if (width != win_buffer_w || height != win_buffer_h) {
        platform_resize(width, height);
    }
}

static uint32_t platform_get_time(void) {
    return (uint32_t)GetTickCount();
}

static char* platform_load_file(const char* path) {
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return NULL;
    DWORD size = GetFileSize(h, NULL);
    char* buf = (char*)malloc(size + 1);
    if (!buf) { CloseHandle(h); return NULL; }
    DWORD read;
    if (!ReadFile(h, buf, size, &read, NULL) || read != size) {
        free(buf);
        CloseHandle(h);
        return NULL;
    }
    buf[size] = '\0';
    CloseHandle(h);
    return buf;
}

// Save to user prefs folder (AppData\Roaming\<COMPANY>\<GAME>\)
static bool platform_save(const char* file_name, const char* data) {
    char prefs[MAX_PATH];
    if (FAILED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, prefs)))
        return false;

    char dir[MAX_PATH];
    snprintf(dir, MAX_PATH, "%s\\%s\\%s",
             prefs, COMPANY_NAME, GAME_NAME);
    CreateDirectoryA(dir, NULL);

    char full[MAX_PATH];
    snprintf(full, MAX_PATH, "%s\\%s", dir, file_name);

    HANDLE h = CreateFileA(full, GENERIC_WRITE, 0, NULL,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;

    DWORD written;
    size_t len = strlen(data);
    bool ok = WriteFile(h, data, (DWORD)len, &written, NULL)
              && written == (DWORD)len;
    CloseHandle(h);
    return ok;
}

static char* platform_load(const char* file_name) {
    char prefs[MAX_PATH];
    if (FAILED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, prefs)))
        return NULL;

    char full[MAX_PATH];
    snprintf(full, MAX_PATH, "%s\\%s\\%s\\%s",
             prefs, COMPANY_NAME, GAME_NAME, file_name);

    return platform_load_file(full);
}

static void platform_quit(void) {
    if (win_dc) {
        ReleaseDC(win_window, win_dc);
        win_dc = NULL;
    }
    if (win_window) {
        DestroyWindow(win_window);
        win_window = NULL;
    }
    UnregisterClassA("ChaoWinClass", GetModuleHandleA(NULL));
}

int win_to_custom_key(WPARAM vk) {
    switch (vk) {
        case 'A': return KEY_A;
        case 'B': return KEY_B;
        case 'C': return KEY_C;
        case 'D': return KEY_D;
        case 'E': return KEY_E;
        case 'F': return KEY_F;
        case 'G': return KEY_G;
        case 'H': return KEY_H;
        case 'I': return KEY_I;
        case 'J': return KEY_J;
        case 'K': return KEY_K;
        case 'L': return KEY_L;
        case 'M': return KEY_M;
        case 'N': return KEY_N;
        case 'O': return KEY_O;
        case 'P': return KEY_P;
        case 'Q': return KEY_Q;
        case 'R': return KEY_R;
        case 'S': return KEY_S;
        case 'T': return KEY_T;
        case 'U': return KEY_U;
        case 'V': return KEY_V;
        case 'W': return KEY_W;
        case 'X': return KEY_X;
        case 'Y': return KEY_Y;
        case 'Z': return KEY_Z;

        case '0': return KEY_0;
        case '1': return KEY_1;
        case '2': return KEY_2;
        case '3': return KEY_3;
        case '4': return KEY_4;
        case '5': return KEY_5;
        case '6': return KEY_6;
        case '7': return KEY_7;
        case '8': return KEY_8;
        case '9': return KEY_9;

        case VK_UP:    return KEY_UP;
        case VK_DOWN:  return KEY_DOWN;
        case VK_LEFT:  return KEY_LEFT;
        case VK_RIGHT: return KEY_RIGHT;

        case VK_F1:  return KEY_F1;
        case VK_F2:  return KEY_F2;
        case VK_F3:  return KEY_F3;
        case VK_F4:  return KEY_F4;
        case VK_F5:  return KEY_F5;
        case VK_F6:  return KEY_F6;
        case VK_F7:  return KEY_F7;
        case VK_F8:  return KEY_F8;
        case VK_F9:  return KEY_F9;
        case VK_F10: return KEY_F10;
        case VK_F11: return KEY_F11;
        case VK_F12: return KEY_F12;

        case VK_ESCAPE:   return KEY_ESC;
        case VK_TAB:      return KEY_TAB;
        case VK_BACK:     return KEY_BACKSPACE;
        case VK_RETURN:   return KEY_ENTER;

        case VK_SPACE:    return KEY_SPACE;
        case VK_OEM_MINUS:   return KEY_MINUS;
        case VK_OEM_PLUS:    return KEY_EQUALS;
        case VK_OEM_COMMA:   return KEY_COMMA;
        case VK_OEM_PERIOD:  return KEY_STOP;
        case VK_OEM_2:       return KEY_SLASH;
        case VK_OEM_1:       return KEY_COLON;      // ;
        case VK_OEM_7:       return KEY_QUOTE;      // '
        case VK_OEM_4:       return KEY_OPENBRACE;  // [
        case VK_OEM_6:       return KEY_CLOSEBRACE; // ]
        case VK_OEM_5:       return KEY_BACKSLASH;  // \

        case VK_NUMPAD0: return KEY_0_KEY_PAD;
        case VK_NUMPAD1: return KEY_1_KEY_PAD;
        case VK_NUMPAD2: return KEY_2_KEY_PAD;
        case VK_NUMPAD3: return KEY_3_KEY_PAD;
        case VK_NUMPAD4: return KEY_4_KEY_PAD;
        case VK_NUMPAD5: return KEY_5_KEY_PAD;
        case VK_NUMPAD6: return KEY_6_KEY_PAD;
        case VK_NUMPAD7: return KEY_7_KEY_PAD;
        case VK_NUMPAD8: return KEY_8_KEY_PAD;
        case VK_NUMPAD9: return KEY_9_KEY_PAD;
    }
    return -1;
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SIZE: {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            platform_resize(w, h);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_KEYDOWN: {
            int code = win_to_custom_key(wParam);
            // lParam bit 30 == previous state; only fire on fresh press
            if (!(lParam & (1 << 30)) && code >= 0 && code < CHAO_MAX_KEYS) {
                input.pressed[code]      = true;
                input.just_pressed[code] = true;
            }
            return 0;
        }
        case WM_KEYUP: {
            int code = win_to_custom_key(wParam);
            if (code >= 0 && code < CHAO_MAX_KEYS) {
                input.pressed[code]       = false;
                input.just_released[code] = true;
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            input.mouse.x = GET_X_LPARAM(lParam);
            input.mouse.y = GET_Y_LPARAM(lParam);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            input.mouse.pressed       = true;
            input.mouse.just_pressed  = true;
            return 0;
        }
        case WM_LBUTTONUP: {
            input.mouse.pressed        = false;
            input.mouse.just_released  = true;
            return 0;
        }
        case WM_RBUTTONDOWN: {
            input.mouse.pressed_right      = true;
            input.mouse.just_pressed_right = true;
            return 0;
        }
        case WM_RBUTTONUP: {
            input.mouse.pressed_right       = false;
            input.mouse.just_released_right = true;
            return 0;
        }
        case WM_MBUTTONDOWN: {
            input.mouse.pressed_middle      = true;
            input.mouse.just_pressed_middle = true;
            return 0;
        }
        case WM_MBUTTONUP: {
            input.mouse.pressed_middle        = false;
            input.mouse.just_repeased_middle  = true;
            return 0;
        }
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            input.mouse.wheel_delta = delta;
            return 0;
        }
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

#endif // PLATFORM_WINAPI_H
