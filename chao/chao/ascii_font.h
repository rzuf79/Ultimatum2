#ifndef CHAO_ASCII_FONT_H
#define CHAO_ASCII_FONT_H

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "types.h"
#include "renderer.h"

typedef enum {
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER
} TextAlign;

typedef struct {
    Bitmap* bitmap;
    Vector2Int glyph_size;
    int char_spacing;
    int line_spacing;
    int space_size;
    RectInt char_rects[256];
} AsciiFont;

uint32_t ascii_font_colors[] = {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHTGRAY,
    COLOR_DARKGRAY,
    COLOR_LIGHTBLUE,
    COLOR_LIGHTGREEN,
    COLOR_LIGHTCYAN,
    COLOR_LIGHTRED,
    COLOR_LIGHTMAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE
};

struct {
    char* buffer;
    int cursor_pos;
} ascii_font_input;

Vector2Int ascii_font_get_size(AsciiFont* font, const char* text, int max_chars);

AsciiFont* ascii_font_create(Bitmap* font_bitmap, bool auto_kerning) {
	AsciiFont* new_font = (AsciiFont*)malloc(sizeof(AsciiFont));
	new_font->bitmap = font_bitmap;

	uint32_t bg_color = get_pixel(font_bitmap, 0, 0);

	new_font->glyph_size = (Vector2Int){font_bitmap->size.x / 16, font_bitmap->size.y / 16};
	new_font->char_spacing = 1;
	new_font->line_spacing = new_font->glyph_size.y * 0.25f;
	new_font->space_size = new_font->glyph_size.x / 2;


    int i = 0;
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            RectInt* rect = &(new_font->char_rects[i]);
            int glyph_x = new_font->glyph_size.x * x;

            rect->x = glyph_x;
            rect->y = new_font->glyph_size.y * y;
            rect->w = new_font->glyph_size.x;
            rect->h = new_font->glyph_size.y;

            if (auto_kerning) {
                for (int p_col = 0; p_col < new_font->glyph_size.x; ++p_col) {
                    for (int p_row = 0; p_row < new_font->glyph_size.y; ++p_row) {
                        int px = glyph_x + p_col;
                        int py = rect->y + p_row;

                        if (get_pixel(font_bitmap, px, py) != bg_color){
                            new_font->char_rects[i].x = px;
                            goto left_loop_out;
                        }
                    }
                }
                left_loop_out:

                for (int p_col = new_font->glyph_size.x - 1; p_col >= 0 ; --p_col) {
                    for (int p_row = 0; p_row < new_font->glyph_size.y; ++p_row) {
                        int px = glyph_x + p_col;
                        int py = rect->y + p_row;

                        if (get_pixel(font_bitmap, px, py) != bg_color){
                            int new_w = (px - new_font->char_rects[i].x) + 1;
                            new_font->char_rects[i].w = new_w;
                            goto right_loop_out;
                        }
                    }
                }
                right_loop_out:;
            }

            i++;
        }
    }


	return new_font;
}

void ascii_font_free(AsciiFont* font) {
    if (font) {
        free(font);
    }
}

char* ascii_font_get_raw_text(const char* text) {
	size_t n = strlen(text);
	char* out = (char*)malloc(sizeof(char) * n);
	const char* c = text;
	int i = 0;
	bool valid_char = true;
	while (*c) {
		if (valid_char) {
			if (*c == '`') {
				valid_char = false;
			} else {
				out[i] = *c;
				i++;
			}
		}
		else {
			if (*c == '`' || !isdigit(*(c+1))) {
				valid_char = true;
			}
		}
		c++;
	}
	out[i] = '\0';
	return out;
}

void ascii_font_draw(Bitmap* dst, AsciiFont* font, int x, int y, uint32_t color, const char* text, ...) {
    char buffer[1024] = {0};
    FORMAT_TEXT(buffer, text);

    int n = strlen(buffer);
    int start_x = x;
    uint32_t color_stack[32];
    int stack_pos = 0;
    color_stack[0] = color;
    uint32_t current_color = color;

    for (int i = 0; i < n; ++i) {
        if (buffer[i] == '`' && i + 1 < n) {
            if (buffer[i + 1] == '`') {
                if (stack_pos > 0) {
                    stack_pos--;
                    current_color = color_stack[stack_pos];
                }
                i++;
                continue;
            }
            else if (isdigit(buffer[i+1])) {
                int color_idx = buffer[i + 1] - '0';
                if (i + 2 < n && buffer[i + 2] >= '0' && buffer[i + 2] <= '9') {
                    color_idx = color_idx * 10 + (buffer[i + 2] - '0');
                    i++;
                }
                if (color_idx < sizeof(ascii_font_colors) / sizeof(uint32_t)) {
                    if (stack_pos < 31) {
                        stack_pos++;
                        color_stack[stack_pos] = ascii_font_colors[color_idx];
                        current_color = ascii_font_colors[color_idx];
                    }
                }
                i++;
                continue;
            }
        }

        if (buffer[i] == ' ') {
            x += font->space_size;
        } else if (buffer[i] == '\n') {
            x = start_x;
            y += font->glyph_size.y + font->line_spacing;
        } else {
            int idx = (int)buffer[i];
            RectInt rect = font->char_rects[idx];

            blit_rect(dst, font->bitmap, (RectInt){rect.x, rect.y, rect.w, rect.h}, (Vector2Int){x, y}, current_color);
            x += rect.w + font->char_spacing;
        }
    }
}

char* ascii_font_break_lines(AsciiFont* font, int max_width, const char* text, ...) {
	char buffer[1024];
    FORMAT_TEXT(buffer, text);

    int len = strlen(buffer);
    char* out = (char*)malloc(len + 1);

    int out_i = 0;
    int line_start = 0;
    int last_space_out = -1;

    for (int i = 0; i < len; ++i) {
        char c = buffer[i];
        out[out_i++] = c;

        if (c == ' ') {
            last_space_out = out_i - 1;
        }
        if (c == '\n') {
            line_start = out_i;
            last_space_out = -1;
            continue;
        }

        out[out_i] = '\0';
        Vector2Int size = ascii_font_get_size(font, out + line_start, -1);

        if (max_width > 0 && size.x > max_width && last_space_out >= 0) {
            out[last_space_out] = '\n';
            line_start = last_space_out + 1;
            last_space_out = -1;
        }
    }

    out[out_i] = '\0';
    return out;
}

void ascii_font_draw_ex(Bitmap* dst, AsciiFont* font, int x, int y, uint32_t color, TextAlign align, int cursor_position, const char* text, ...) {
    char buffer[1024];
    FORMAT_TEXT(buffer, text);

    int len = strlen(buffer);
    int line_start = 0;
    int draw_x = x;
    int draw_y = y;

    uint32_t color_stack[32];
    int stack_pos = 0;
    color_stack[0] = color;
    uint32_t current_color = color;
    int char_index = 0;

    while (line_start < len) {
        int line_end = line_start;
        while (line_end < len && buffer[line_end] != '\n') {
            line_end++;
        }

        char saved = buffer[line_end];
        buffer[line_end] = '\0';
        Vector2Int size = ascii_font_get_size(font, buffer + line_start, -1);
        buffer[line_end] = saved;

        draw_x = x;
        if (align == TEXT_ALIGN_CENTER) {
            draw_x = x - size.x / 2;
        } else if (align == TEXT_ALIGN_RIGHT) {
            draw_x = x - size.x;
        }
        int cx = draw_x;

        for (int i = line_start; i < line_end; ++i) {
            char c = buffer[i];

            if (c == '`' && i + 1 < len) {
                if (buffer[i + 1] == '`') {
                    if (stack_pos > 0) {
                        stack_pos--;
                        current_color = color_stack[stack_pos];
                    }
                    i++;
                    continue;
                } else if (isdigit(buffer[i + 1])) {
                    int color_idx = buffer[i + 1] - '0';
                    if (i + 2 < len && isdigit(buffer[i + 2])) {
                        color_idx = color_idx * 10 + (buffer[i + 2] - '0');
                        i++;
                    }
                    if (color_idx < (int)(sizeof(ascii_font_colors) / sizeof(uint32_t))) {
                        if (stack_pos < 31) {
                            stack_pos++;
                            color_stack[stack_pos] = ascii_font_colors[color_idx];
                            current_color = ascii_font_colors[color_idx];
                        }
                    }
                    i++;
                    continue;
                }
            }
            
            RectInt r = font->char_rects[(int)c];
            if (char_index == cursor_position) {
                draw_line(dst, cx, draw_y, cx, draw_y+font->glyph_size.y, COLOR_BLACK);
            }
            
            if (c == ' ') {
                cx += font->space_size;
            } else {
                blit_rect(dst, font->bitmap, (RectInt){ r.x, r.y, r.w, r.h }, (Vector2Int){ cx, draw_y }, current_color);
                cx += r.w + font->char_spacing;
            }
            char_index ++;
            if (char_index == len && char_index == cursor_position) {
                draw_line(dst, cx, draw_y, cx, draw_y+font->glyph_size.y, COLOR_BLACK);
            }
            
        }

        draw_y += font->glyph_size.y + font->line_spacing;
        char_index ++;

        if (line_end < len && buffer[line_end] == '\n') {
            line_end++;
        }

        line_start = line_end;
    }
}

Vector2Int ascii_font_get_size(AsciiFont* font, const char* text, int max_chars) {
    int longest_line = 0;
    int n = strlen(text);
    
    if (max_chars >= 0) {
        n = max(n, max_chars);
    }
    
    Vector2Int size = { 0, font->glyph_size.y };
    for (int i = 0; i < n; ++i) {
        if (text[i] == '`' && i + 1 < n) {
            if (text[i + 1] == '`') {
                i++;
                continue;
            }
            else if (isdigit(text[i+1])) {
                if (i + 2 < n && text[i + 2] >= '0' && text[i + 2] <= '9') {
                    i += 2;
                } else {
                    i++;
                }
                continue;
            }
        }

        if (text[i] == ' ') {
            size.x += font->space_size;
        } else if (text[i] == '\n') {
            longest_line = max(size.x, longest_line);
            size.x = 0;
            if (i < n-1) {
                size.y += font->glyph_size.y + font->line_spacing;
            }
        } else {
            int idx = (int)text[i];
            if (size.x > 0) {
                size.x += font->char_spacing;
            }
            size.x += font->char_rects[idx].w;
        }
    }
    size.x = max(size.x, longest_line);
    return size;
}

Bitmap* ascii_font_bake(AsciiFont* font, uint32_t color, const char* text, ...) {
    char buffer[1024];
    FORMAT_TEXT(buffer, text);

	Vector2Int size = ascii_font_get_size(font, buffer, -1);
	Bitmap* bmp = create_bitmap_filled(size.x, size.y, COLOR_TRANSPARENT);
	ascii_font_draw(bmp, font, 0, 0, color, buffer);
	return bmp;
}

void ascii_font_input_set_buffer(char* buffer, bool clear) {
    ascii_font_input.buffer = buffer;
    if (ascii_font_input.buffer != NULL) {
        if (clear) {
            buffer[0] = '\0';
            ascii_font_input.cursor_pos = -1;
        } else {
            ascii_font_input.cursor_pos = strlen(buffer);
        }
    }
}

void ascii_font_input_update() {
    if (ascii_font_input.buffer == NULL) {
        return;
    }
    size_t n = strlen(ascii_font_input.buffer);
    if (input.just_pressed[KEY_LEFT]) ascii_font_input.cursor_pos --;
    if (input.just_pressed[KEY_RIGHT]) ascii_font_input.cursor_pos ++;
    ascii_font_input.cursor_pos = clamp(ascii_font_input.cursor_pos, 0, n);

    int new_char = -1;
    for (int key = KEY_A; key <= KEY_Z; ++key) {
        if (input.just_pressed[key]) {
            new_char = (input.pressed[KEY_LSHIFT] || input.pressed[KEY_RSHIFT]) ? key : (key+32);
        }
    }
    if (input.just_pressed[KEY_SPACE]) {
        new_char = 32;
    }
    
    if (input.just_pressed[KEY_BACKSPACE] && ascii_font_input.cursor_pos > 0) {
        for (int i = ascii_font_input.cursor_pos; i <= n; ++i) {
            ascii_font_input.buffer[i-1] = ascii_font_input.buffer[i];
        }
        ascii_font_input.cursor_pos --;
    }
    
    // if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_ESC]) {
    //     ascii_font_input.buffer = NULL;
    //     return;
    // }    
    
    if (new_char != -1) {
        for (int i = n; i >= ascii_font_input.cursor_pos; --i) {
            ascii_font_input.buffer[i+1] = ascii_font_input.buffer[i];
        }
        ascii_font_input.buffer[ascii_font_input.cursor_pos] = new_char;
        ascii_font_input.cursor_pos ++;
    }
}


#endif // CHAO_ASCII_FONT_H
