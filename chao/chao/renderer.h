#ifndef CHAO_RENDERER_H
#define CHAO_RENDERER_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "3rd_party/stb_image.h"

// classic BGI palette ftw
#define COLOR_BLACK 0x000000FF
#define COLOR_BLUE 0x0000FFFF
#define COLOR_GREEN 0x00FF00FF
#define COLOR_CYAN 0x00FFFFFF
#define COLOR_RED 0xFF0000FF
#define COLOR_MAGENTA 0xFF00FFFF
#define COLOR_BROWN 0xA0522DFF
#define COLOR_LIGHTGRAY 0xC0C0C0FF
#define COLOR_DARKGRAY 0x808080FF
#define COLOR_LIGHTBLUE 0xADD8E6FF
#define COLOR_LIGHTGREEN 0x90EE90FF
#define COLOR_LIGHTCYAN 0xE0FFFFff
#define COLOR_LIGHTRED 0xFFB6C1FF
#define COLOR_LIGHTMAGENTA 0xFF00FFFF
#define COLOR_YELLOW 0xFFFF00FF
#define COLOR_WHITE 0xFFFFFFFF

#define COLOR_TRANSPARENT 0xFFFFFF00

#define RGBA(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)
#define FRGBA(r, g, b, a) (((r*256) << 24) | ((g*255) << 16) | ((b*255) << 8) | (a*255))
#define SET_ALPHA(color, alpha) ((color & 0xFFFFFF00) | ((int)((alpha)) & 0xFF))
#define SET_ALPHAF(color, alpha) (SET_ALPHA(color, alpha * 255))
#define screen_size chao_canvas->size
#define BMP_RECT(bitmap) (RectInt){0,0,bitmap->size.x,bitmap->size.y}

const uint32_t PIXEL_HARD_MASK = RGBA(00, 00, 00, 128);

Bitmap* chao_canvas;
BlendMode chao_blend_mode;
bool chao_hard_tint = false;

struct {
	bool enabled;
	uint32_t color;
	float start;
	float end;
} chao_3d_fog;

struct {
	bool enabled;
	Vector3 direction;
	float ambient;
} chao_3d_directional_light;

Bitmap* create_bitmap(int w, int h){
    Bitmap* bitmap = (Bitmap*)malloc(sizeof(Bitmap));
    bitmap->size.x = w;
    bitmap->size.y = h;
    bitmap->pixels = (uint32_t*)malloc(w * h * sizeof(uint32_t));
    bitmap->z_buffer = NULL;
    return bitmap;
}

Bitmap* create_bitmap_filled(int w, int h, uint32_t fill_color) {
    Bitmap* bitmap = create_bitmap(w, h);
	for (int i = 0; i < bitmap->size.x * bitmap->size.y; ++i) {
		bitmap->pixels[i] = fill_color;
	}
    return bitmap;
}

void create_bitmap_z_buffer(Bitmap* bitmap) {
	if (bitmap->z_buffer != NULL) {
		free(bitmap->z_buffer);
	}
	bitmap->z_buffer = (float*)malloc(sizeof(float) * bitmap->size.x * bitmap->size.y);
	for (int i = 0; i < bitmap->size.x * bitmap->size.y; ++i) {
		bitmap->z_buffer[i] = -INFINITY;
	}
}

void resize_bitmap(Bitmap* bitmap, int w, int h) {
    if (bitmap->size.x == w && bitmap->size.y == h) {
        return;
    }
    if (bitmap->pixels) {
        free(bitmap->pixels);
    }
    bitmap->size.x = w;
    bitmap->size.y = h;
    bitmap->pixels = (uint32_t*)malloc(w * h * sizeof(uint32_t));
    if (bitmap->z_buffer) {
		create_bitmap_z_buffer(bitmap);
	}
}

void free_bitmap(Bitmap* bitmap) {
    if (bitmap) {
        if (bitmap->pixels) {
            free(bitmap->pixels);
            bitmap->pixels = NULL;
        }
        if (bitmap->z_buffer) {
        	free(bitmap->z_buffer);
        	bitmap->z_buffer = NULL;
		}
        free(bitmap);
    }
}


///////////////////////////////
//// PIXELS
///////////////////////////////
static inline uint32_t blend_pixel_alpha(uint32_t src, uint32_t dst) {
    uint32_t a1 = src & 0xFF;
    uint32_t inv = 255 - a1;
    uint32_t r = ((((src >> 24) & 0xFF) * a1) + (((dst >> 24) & 0xFF) * inv)) / 255;
    uint32_t g = ((((src >> 16) & 0xFF) * a1) + (((dst >> 16) & 0xFF) * inv)) / 255;
    uint32_t b = ((((src >>  8) & 0xFF) * a1) + (((dst >>  8) & 0xFF) * inv)) / 255;
    uint32_t a =  a1 + (((( dst & 0xFF) * inv)) / 255);
    return RGBA(r, g, b, a);
}

static inline uint32_t blend_pixel_additive(uint32_t src, uint32_t dst) {
    uint32_t r1 = (src >> 24) & 0xFF;
    uint32_t g1 = (src >> 16) & 0xFF;
    uint32_t b1 = (src >>  8) & 0xFF;
    uint32_t a1 =  src & 0xFF;
    uint32_t r2 = (dst >> 24) & 0xFF;
    uint32_t g2 = (dst >> 16) & 0xFF;
    uint32_t b2 = (dst >>  8) & 0xFF;
    uint32_t a2 =  dst & 0xFF;
    uint32_t r = r1 + r2; if (r > 255) r = 255;
    uint32_t g = g1 + g2; if (g > 255) g = 255;
    uint32_t b = b1 + b2; if (b > 255) b = 255;
    uint32_t a = a1 + a2; if (a > 255) a = 255;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

static inline uint32_t blend_pixel_multiply(uint32_t src, uint32_t dst) {
    uint32_t rs = (src >> 24) & 0xFF;
    uint32_t gs = (src >> 16) & 0xFF;
    uint32_t bs = (src >>  8) & 0xFF;
    uint32_t as = src & 0xFF;
    uint32_t rd = (dst >> 24) & 0xFF;
    uint32_t gd = (dst >> 16) & 0xFF;
    uint32_t bd = (dst >>  8) & 0xFF;
    uint32_t ad = dst & 0xFF;
    uint32_t rm = (rd * rs) / 255;
    uint32_t gm = (gd * gs) / 255;
    uint32_t bm = (bd * bs) / 255;

    uint32_t r = rd + ((int)(rm - rd) * as) / 255;
    uint32_t g = gd + ((int)(gm - gd) * as) / 255;
    uint32_t b = bd + ((int)(bm - bd) * as) / 255;

    uint32_t a = as + ((ad * (255 - as)) / 255);

    return (r << 24) | (g << 16) | (b << 8) | a;
}

static inline uint32_t blend_pixel(uint32_t src, uint32_t dst, BlendMode blend_mode) {
    switch (blend_mode) {
        default:
        case BLEND_NONE: return src;
        case BLEND_ALPHA: return blend_pixel_alpha(src, dst);
        case BLEND_ADDITIVE: return blend_pixel_additive(src, dst);
        case BLEND_MULTIPLY: return blend_pixel_multiply(src, dst);
    }
}

// hard tint, tints everything including dark pixels
uint32_t tint_pixel(uint32_t base, uint32_t tint) {
    uint32_t alpha = tint & 0xFF;
    uint32_t inv_alpha = 255 - alpha;

    uint32_t rb = ((base & 0xFF00FF00) >> 8);
    uint32_t trb = ((tint & 0xFF00FF00) >> 8);

    uint32_t g = ((base & 0x00FF0000) >> 16);
    uint32_t tg = ((tint & 0x00FF0000) >> 16);

    uint32_t r_out = (((((base >> 24) & 0xFF) * inv_alpha) + (((tint >> 24) & 0xFF) * alpha)) >> 8);
    uint32_t g_out = (((g * inv_alpha) + (tg * alpha)) >> 8);
    uint32_t b_out = (((((base >> 8) & 0xFF) * inv_alpha) + (((tint >> 8) & 0xFF) * alpha)) >> 8);

    uint32_t a_out = base & 0xFF;

    return (r_out << 24) | (g_out << 16) | (b_out << 8) | a_out;
}

// this is a kinda soft tint, so black stays black and brighter a pixel is the more tint it gets
uint32_t modulate_pixel(uint32_t base, uint32_t modulate) {
    uint32_t r_base = (base >> 24) & 0xFF;
    uint32_t g_base = (base >> 16) & 0xFF;
    uint32_t b_base = (base >> 8)  & 0xFF;
    uint32_t a_base = base & 0xFF;
    
    uint32_t r_mod = (modulate >> 24) & 0xFF;
    uint32_t g_mod = (modulate >> 16) & 0xFF;
    uint32_t b_mod = (modulate >> 8)  & 0xFF;
    uint32_t a_mod = modulate & 0xFF;

    uint32_t r_out = (r_base * r_mod) / 255;
    uint32_t g_out = (g_base * g_mod) / 255;
    uint32_t b_out = (b_base * b_mod) / 255;
    
    uint32_t a_out = a_base;

    return (r_out << 24) | (g_out << 16) | (b_out << 8) | a_out;
}

uint32_t brighten_pixel(uint32_t base, float factor) {
    uint32_t r = (base >> 24) & 0xFF;
    uint32_t g = (base >> 16) & 0xFF;
    uint32_t b = (base >> 8)  & 0xFF;
    uint32_t a = base & 0xFF;
    
    r = r + (uint32_t)((255 - r) * factor);
    g = g + (uint32_t)((255 - g) * factor);
    b = b + (uint32_t)((255 - b) * factor);
    
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void put_pixel(Bitmap* bitmap, int x, int y, uint32_t color) {
    if (x >= 0 && x < bitmap->size.x && y >= 0 && y < bitmap->size.y) {
        uint32_t* px = &bitmap->pixels[y * bitmap->size.x + x];
        *px = blend_pixel(color, *px, chao_blend_mode);
    }
}

uint32_t get_pixel(Bitmap* bitmap, int x, int y) {
    if (bitmap == NULL || x < 0 || x > bitmap->size.x || y < 0 || y > bitmap->size.y) {
        return 0xFF00FF00;
    }
    return bitmap->pixels[y * bitmap->size.x + x];
}

uint32_t get_pixel_clamped(Bitmap* bitmap, int x, int y) {
	x = clamp(x, 0, bitmap->size.x-1);
	y = clamp(y, 0, bitmap->size.y-1);
	return bitmap->pixels[y * bitmap->size.x + x];
}

uint32_t get_pixel_wrapped(Bitmap* bitmap, int x, int y) {
	x = wrap_int(x, 0, bitmap->size.x);
	y = wrap_int(y, 0, bitmap->size.y);
	return get_pixel(bitmap, x, y);
}

uint32_t get_pixel_nearest(Bitmap* bitmap, float x, float y) {
	x = round(x * (bitmap->size.x - 1));
	y = round(y * (bitmap->size.y - 1));
	return get_pixel(bitmap, x, y);
}

uint32_t get_pixel_bilinear(Bitmap* bitmap, float x, float y) {
	return 0x00000000;
}

///////////////////////////////
//// PRIMITIVES
///////////////////////////////

void fill(Bitmap* dst, uint32_t color) {
	if (dst->z_buffer != NULL){
		for (int i = 0; i < dst->size.x * dst->size.y; ++i) {
			dst->pixels[i] = color;
			dst->z_buffer[i] = INFINITY;
		}
	} else {
		for (int i = 0; i < dst->size.x * dst->size.y; ++i) {
			dst->pixels[i] = color;
		}
	}
}

void fill_rect(Bitmap* dst, int x, int y, int w, int h, uint32_t color) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            put_pixel(dst, i, j, color);
        }
    }
}

void draw_line(Bitmap* dst, int x1, int y1, int x2, int y2, uint32_t color) {
	bool steep = abs(x1-x2) < abs(y1-y2);
	if (steep) {
		swap(int, x1, y1);
		swap(int, x2, y2);
	}
	if (x1 > x2) {
		swap(int, x1, x2);
		swap(int, y1, y2);
	}
	int y = y1;
	int ierror = 0;
	for (int x = x1; x <= x2; ++x) {
		if (steep) {
			put_pixel(dst, y, x, color);
		} else {
			put_pixel(dst, x, y, color);
		}
		ierror += 2 * abs(y2 - y1);
		y += (y2 > y1 ? 1 : -1) * (ierror > x2 - x1);
        ierror -= 2 * (x2-x1) * (ierror > x2 - x1);
	}
}

void draw_line_thick(Bitmap* dst, int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len == 0) return;

    float nx = -dy / len;
    float ny = dx / len;
    float half = thickness / 2.0f;

    float ox = nx * half;
    float oy = ny * half;

    float vx[4] = {
        x1 + ox, x1 - ox,
        x2 - ox, x2 + ox
    };
    float vy[4] = {
        y1 + oy, y1 - oy,
        y2 - oy, y2 + oy
    };

    int minx = (int)fminf(fminf(vx[0], vx[1]), fminf(vx[2], vx[3]));
    int maxx = (int)fmaxf(fmaxf(vx[0], vx[1]), fmaxf(vx[2], vx[3]));
    int miny = (int)fminf(fminf(vy[0], vy[1]), fminf(vy[2], vy[3]));
    int maxy = (int)fmaxf(fmaxf(vy[0], vy[1]), fmaxf(vy[2], vy[3]));

    for (int y = miny; y <= maxy; ++y) {
        if (y < 0 || y >= dst->size.y) continue;
        for (int x = minx; x <= maxx; ++x) {
            if (x < 0 || x >= dst->size.x) continue;

            float w0 = (vx[1] - vx[0]) * (y - vy[0]) - (vy[1] - vy[0]) * (x - vx[0]);
            float w1 = (vx[2] - vx[1]) * (y - vy[1]) - (vy[2] - vy[1]) * (x - vx[1]);
            float w2 = (vx[3] - vx[2]) * (y - vy[2]) - (vy[3] - vy[2]) * (x - vx[2]);
            float w3 = (vx[0] - vx[3]) * (y - vy[3]) - (vy[0] - vy[3]) * (x - vx[3]);

            if ((w0 <= 0 && w1 <= 0 && w2 <= 0 && w3 <= 0) ||
                (w0 >= 0 && w1 >= 0 && w2 >= 0 && w3 >= 0)) {
                put_pixel(dst, x, y, color);
            }
        }
    }
}

void draw_rect(Bitmap* dst, int x, int y, int w, int h, int thickness, uint32_t color) {
    // top
    fill_rect(dst, x, y, w, thickness, color);
    // bottom
    fill_rect(dst, x, y+(h-thickness), w, thickness, color);
    // left
    fill_rect(dst, x, y+thickness, thickness, h-thickness*2, color);
    // right
    fill_rect(dst, x+(w-thickness), y+thickness, thickness, h-thickness*2, color);
}

void fill_circle(Bitmap* dst, int x, int y, int r, uint32_t color) {
	int r2 = r * r;
	for (int dy = -r; dy <= r; ++dy) {
		int y2 = dy * dy;
		if (y + dy >= 0 && y + dy < dst->size.y) {
			int x_offset = sqrt(r2 - y2);
			for (int dx = -x_offset; dx <= x_offset; ++dx) {
				put_pixel(dst, x+dx, y+dy, color);
			}
		}
	}
}

void fill_arc(Bitmap* dst, int cx, int cy, int r, int start_angle, int end_angle, uint32_t color) {
    while (start_angle < 0) start_angle += 360;
    while (end_angle < 0) end_angle += 360;
	start_angle %= 360;
    end_angle %= 360;

    int r2 = r * r;

    for (int dy = -r; dy <= r; ++dy) {
        int y = cy + dy;
        if (y < 0 || y >= dst->size.y) continue;

        for (int dx = -r; dx <= r; ++dx) {
            int x = cx + dx;
            if (x < 0 || x >= dst->size.x) continue;

            int d2 = dx * dx + dy * dy;
            if (d2 > r2) continue;

            float angle = atan2f((float)dy, (float)dx) * 180.0f / M_PI;
            if (angle < 0) angle += 360.0f;

            if (start_angle < end_angle) {
                if (angle >= start_angle && angle <= end_angle) {
                    put_pixel(dst, x, y, color);
                }
            } else {
                if (angle >= start_angle || angle <= end_angle) {
                    put_pixel(dst, x, y, color);
                }
            }
        }
    }
}


///////////////////////////////
//// TRIANGLES
///////////////////////////////

float signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {
	return ((y2-y1)*(x2+x1) + (y3-y2)*(x3+x2) + (y1-y3)*(x1+x3)) * 0.5f;
}

RectInt get_tri_bounds(int x1, int y1, int x2, int y2, int x3, int y3) {
	return (RectInt) {
		min(min(x1, x2), x3),
		min(min(y1, y2), y3),
		max(max(x1, x2), x3),
		max(max(y1, y2), y3)
	};
}

RectInt get_tri_bounds_clamped(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3) {
	return (RectInt) {
		max(0, min(min(x1, x2), x3)),
		max(0, min(min(y1, y2), y3)),
		min(dst->size.x-1, max(max(x1, x2), x3)),
		min(dst->size.y-1, max(max(y1, y2), y3))
	};
}

bool tri_barycentric(int x1, int y1, int x2, int y2, int x3, int y3, int xp, int yp, int *u1, int *u2, int *det) {
	*det = ((x1 - x3)*(y2 - y3) - (x2 - x3)*(y1 - y3));
	*u1  = ((y2 - y3)*(xp - x3) + (x3 - x2)*(yp - y3));
	*u2  = ((y3 - y1)*(xp - x3) + (x1 - x3)*(yp - y3));
	int u3 = *det - *u1 - *u2;
	return (
			(sign(int, *u1) == sign(int, *det) || *u1 == 0) &&
			(sign(int, *u2) == sign(int, *det) || *u2 == 0) &&
			(sign(int, u3) == sign(int, *det) || u3 == 0)
		   );
}

void fill_triangle(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
	RectInt bb = get_tri_bounds_clamped(dst, x1, y1, x2, y2, x3, y3);
	for (int x = bb.x; x <= bb.w; ++x) {
		for (int y = bb.y; y <= bb.h; ++y) {
			int u1, u2, det;
			if (tri_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
				put_pixel(dst, x, y, color);
			}
		}
	}
}

bool validate_z_buffer(Bitmap* dst, int x, int y, int u1, int u2, int det, float z1, float z2, float z3, float* out_z) {
    if (dst->z_buffer == NULL) {
        return true;
    }
	int u3 = det - u1 - u2;
	float detf = (float)det;
	if (detf == 0.0f) {
	   return false;
    }
	float w1 = (float)u1 / detf;
	float w2 = (float)u2 / detf;
	float w3 = 1.0f - w1 - w2;
	float z = z1 * w1 + z2 * w2 + z3 * w3;
	int z_buffer_index = y * dst->size.x + x;
	if (z >= dst->z_buffer[z_buffer_index]) {
		return false;
	}
	dst->z_buffer[z_buffer_index] = z;
	*out_z = z;
	return true;
}

void fill_triangle_3c(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t c1, uint32_t c2, uint32_t c3) {
	RectInt bb = get_tri_bounds_clamped(dst, x1, y1, x2, y2, x3, y3);
	for (int x = bb.x; x <= bb.w; ++x) {
		for (int y = bb.y; y <= bb.h; ++y) {
			int u1, u2, det;
			if (tri_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
				put_pixel(dst, x, y, mix_colors3(c1, c2, c3, u1, u2, det));
			}
		}
	}
}

void fill_triangle_3uv(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, float z1, float z2, float z3, Vector4 v1, Vector4 v2, Vector4 v3, Bitmap* texture) {
    RectInt bb = get_tri_bounds_clamped(dst, x1, y1, x2, y2, x3, y3);
    for (int y = bb.y; y <= bb.h; ++y) {
        for (int x = bb.x; x <= bb.w; ++x) {
            int u1, u2, det;
            if (z1 == 0.0f || z2 == 0.0f || z3 == 0.0f) {
            	continue;
			}
            if (!tri_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
            	continue;
			}
			float out_z;
			if (!validate_z_buffer(dst, x, y, u1, u2, det, z1, z2, z3, &out_z)) {
				continue;
			}
			uint32_t sample = 0xFF00FFFF;
			if (texture != NULL) {
				int u3 = det - u1 - u2;

				float invz1 = 1.0f / z1;
				float invz2 = 1.0f / z2;
				float invz3 = 1.0f / z3;

				float itx = (tx1 * invz1 * (float)u1 + tx2 * invz2 * (float)u2 + tx3 * invz3 * (float)u3) / (float)det;
				float ity = (ty1 * invz1 * (float)u1 + ty2 * invz2 * (float)u2 + ty3 * invz3 * (float)u3) / (float)det;
				float iw  = (invz1 * (float)u1 + invz2 * (float)u2 + invz3 * (float)u3) / (float)det;

				if (iw == 0.0f) continue;

				float tu = itx / iw;
				float tv = ity / iw;
				tu = clamp(tu, 0, 1);
				tv = clamp(tv, 0, 1);

				int tex_x = (int)(tu * (texture->size.x - 1));
				int tex_y = (int)(tv * (texture->size.y - 1));
				sample = get_pixel(texture, tex_x, tex_y);
			}

            put_pixel(dst, x, y, sample);
        }
    }
}

void fill_triangle_3uv_lit(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, float z1, float z2, float z3, Vector4 v1, Vector4 v2, Vector4 v3, Vector3 n1, Vector3 n2, Vector3 n3, Vector3 light_dir, Bitmap* texture) {
    if ((x1 - x3)*(y2 - y3) - (x2 - x3)*(y1 - y3) == 0) {
    	return;
	}
    RectInt bb = get_tri_bounds_clamped(dst, x1, y1, x2, y2, x3, y3);
    Vector3 L = vector3_get_normalized(light_dir);
    if (z1 == 0.0f || z2 == 0.0f || z3 == 0.0f) {
        return;
    }

    float invz1 = 1.0f / z1;
    float invz2 = 1.0f / z2;
    float invz3 = 1.0f / z3;

	float fog_r = 1;
	float fog_g = 1;
	float fog_b = 1;

	if (chao_3d_fog.enabled) {
		fog_r = (float)((chao_3d_fog.color >> 24) & 0xFF) / 255.0f;
		fog_g = (float)((chao_3d_fog.color >> 16) & 0xFF) / 255.0f;
		fog_b = (float)((chao_3d_fog.color >> 8) & 0xFF) / 255.0f;
	}

    for (int y = bb.y; y <= bb.h; ++y) {
        for (int x = bb.x; x <= bb.w; ++x) {
            int u1, u2, det;
            if (!tri_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
                continue;
            }
			float out_z = 1.0f;
            if (!validate_z_buffer(dst, x, y, u1, u2, det, z1, z2, z3, &out_z)) {
                continue;
            }

            int u3 = det - u1 - u2;
            float detf = (float)det;
            float fu1 = (float)u1;
            float fu2 = (float)u2;
            float fu3 = (float)u3;

            uint32_t sample = 0xFF00FFFF;

            if (texture != NULL) {
                float itx = (tx1 * invz1 * fu1 + tx2 * invz2 * fu2 + tx3 * invz3 * fu3) / detf;
                float ity = (ty1 * invz1 * fu1 + ty2 * invz2 * fu2 + ty3 * invz3 * fu3) / detf;
                float iw  = (invz1 * fu1 + invz2 * fu2 + invz3 * fu3) / detf;

                if (iw == 0.0f) continue;

                float tu = itx / iw;
                float tv = ity / iw;

                tu = clamp(tu, 0.0f, 1.0f);
                tv = clamp(tv, 0.0f, 1.0f);

                int tex_x = (int)(tu * (texture->size.x - 1));
                int tex_y = (int)(tv * (texture->size.y - 1));

                sample = get_pixel(texture, tex_x, tex_y);
            }

            float inx = (n1.x * invz1 * fu1 + n2.x * invz2 * fu2 + n3.x * invz3 * fu3) / detf;
            float iny = (n1.y * invz1 * fu1 + n2.y * invz2 * fu2 + n3.y * invz3 * fu3) / detf;
            float inz = (n1.z * invz1 * fu1 + n2.z * invz2 * fu2 + n3.z * invz3 * fu3) / detf;
            float iwN = (invz1 * fu1 + invz2 * fu2 + invz3 * fu3) / detf;

            if (iwN == 0.0f) continue;

            Vector3 N = {
                inx / iwN,
                iny / iwN,
                inz / iwN
            };

            float ndotl = max(vector3_dot(N, L), 0);
            float light = clamp01(chao_3d_directional_light.ambient + ndotl * (1.0f - chao_3d_directional_light.ambient));	

            uint8_t r = (sample >> 24) & 0xFF;
            uint8_t g = (sample >> 16) & 0xFF;
            uint8_t b = (sample >>  8) & 0xFF;
            uint8_t a =  sample        & 0xFF;

            r = (uint8_t)(r * light);
            g = (uint8_t)(g * light);
            b = (uint8_t)(b * light);

			if (chao_3d_fog.enabled) {
				float fog_factor = clamp01((out_z - chao_3d_fog.start) / (chao_3d_fog.end - chao_3d_fog.start));
				float fr = r / 255.0f;
				float fg = g / 255.0f;
				float fb = b / 255.0f;
				fr = fr * (1.0f - fog_factor) + fog_r * fog_factor;
				fg = fg * (1.0f - fog_factor) + fog_g * fog_factor;
				fb = fb * (1.0f - fog_factor) + fog_b * fog_factor;
				r = (uint8_t)(fr * 255.0f);
				g = (uint8_t)(fg * 255.0f);
				b = (uint8_t)(fb * 255.0f);
			}

            uint32_t lit_color = RGBA(r, g, b, a);
            put_pixel(dst, x, y, lit_color);
        }
    }
}

void fill_triangle_scanline(Bitmap* dst, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    if (y1 > y2) { swap(int, x1, x2); swap(int, y1, y2); }
    if (y1 > y3) { swap(int, x1, x3); swap(int, y1, y3); }
    if (y2 > y3) { swap(int, x2, x3); swap(int, y2, y3); }
    
    int total_height = y3 - y1;
    if (total_height == 0) return;
    
    y1 = ceil(y1);
    y3 = ceil(y3);

    for (int y = y1; y <= y3; y++) {
        bool second_half = y > y2 || y2 == y1;
        int segment_height = second_half ? (y3 - y2) : (y2 - y1);
        if (segment_height == 0) continue;

        float step_a = (float)(y - y1) / total_height;
        float step_b = (float)(y - (second_half ? y2 : y1)) / segment_height;

        int start_x = x1 + (x3 - x1) * step_a;
        int end_x = second_half
            ? x2 + (x3 - x2) * step_b
            : x1 + (x2 - x1) * step_b;

        if (start_x > end_x) { swap(int, start_x, end_x); }

        for (int x = ceil(start_x); x <= ceil(end_x); x++) {
            put_pixel(dst, x, y, color);
        }
    }
}

///////////////////////////////
//// 2D BLITTING
///////////////////////////////

void maskedcpy(uint32_t* dst, uint32_t* src, size_t pixels, bool flip) {
	register uint32_t *srcptr = src, *dstptr = dst;

	if (flip) {
    	while (pixels--) {
    		if (!(*srcptr&PIXEL_HARD_MASK)) {
    			dstptr ++;
    			srcptr --;
    			continue;
    		}
            *dstptr++ = *srcptr--;
    	}
	} else {
	   while (pixels--) {
    		if (!(*srcptr&PIXEL_HARD_MASK)) {
    			dstptr ++;
    			srcptr ++;
    			continue;
    		}
            *dstptr++ = *srcptr++;
    	}
    }
}

void blit_ex(Bitmap* dst, Bitmap* src, RectInt rect, Vector2Int pos, bool flip_h, bool flip_v, uint32_t modulate, float alpha) {
	if (dst == NULL || src == NULL) {
		printf("Trying to blit onto or a NULL here?\n");
		return;
	}
    rect.w = rect.w > 0 ? rect.w : src->size.x;
    rect.h = rect.h > 0 ? rect.h : src->size.y;

    if (flip_h) {
        rect.x += rect.w-1;
    }
    signed char src_v_dir = 1;
    if (flip_v) {
        src_v_dir = -1;
        rect.y += rect.h-1;
    }
    
    if (pos.x < 0) {
        if (flip_h){
            rect.x += pos.x;
        } else {
            rect.x -= pos.x;
        }
        rect.w += pos.x;
        pos.x = 0;
    }
    if (pos.y < 0) {
        if (flip_v){
            rect.y += pos.y;
        } else {
            rect.y -= pos.y;
        }
        rect.h += pos.y;
        pos.y = 0;
    }
    if (pos.x + rect.w >= dst->size.x) {
        rect.w = dst->size.x - pos.x;
    }
    if (pos.y + rect.h >= dst->size.y) {
        rect.h = dst->size.y - pos.y;
    }

    if (rect.w <= 0 || rect.h <= 0) {
        return;
    }

    uint32_t* src_ptr = src->pixels + (rect.y * src->size.x) + rect.x;
    uint32_t* ptr = dst->pixels + (pos.y * dst->size.x) + pos.x;

	alpha = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);
    uint8_t alpha_byte = (uint8_t)(alpha * 255.0f);
    
    if ((modulate == 0xffffffff  || (chao_hard_tint && (modulate&0xFF) == 0)) && chao_blend_mode == BLEND_NONE && alpha_byte == 255) {
    	// fast, no modulate and no blend path
        for (int y = 0; y < rect.h; ++y) {
            maskedcpy(ptr, src_ptr, rect.w, flip_h);
            src_ptr += src->size.x * src_v_dir;
            ptr += dst->size.x;
        }
    } else {
    	// slightly slower path with tints and blending
        signed char src_h_dir = flip_h ? -1 : 1;
        for (int y = 0; y < rect.h; ++y) {
            for (int x = 0; x < rect.w; ++x) {
            	int xi = flip_h ? -x : x;
				uint32_t src_pix = chao_hard_tint ? tint_pixel(*(src_ptr+xi), modulate) : modulate_pixel(*(src_ptr+xi), modulate);
            	if (chao_blend_mode == BLEND_NONE) {
					if (src_pix&PIXEL_HARD_MASK) {
						*(ptr+x) = src_pix;
					}
				} else {
                    src_pix = SET_ALPHA(src_pix, alpha * (src_pix & 0xFF));
                    *(ptr+x) = blend_pixel(src_pix, *(ptr+x), chao_blend_mode);
				}
            }
            src_ptr += src->size.x * src_v_dir;
            ptr += dst->size.x;
        }
    }
}


void blit_rect(Bitmap* dst, Bitmap* src, RectInt rect, Vector2Int pos, uint32_t modulate) {
	blit_ex(dst, src, rect, pos, false, false,  modulate, 1.0f);
}

void blit_tile(Bitmap* dst, Bitmap* src, int tile_size, int tile_index, int x, int y, bool flip_h, bool flip_v, uint32_t modulate) {
	int h_tiles = src->size.x / tile_size;
	int tile_x = (tile_index % h_tiles) * tile_size;
	int tile_y = (tile_index / h_tiles) * tile_size;
	blit_ex(dst, src, (RectInt){tile_x, tile_y, tile_size, tile_size}, (Vector2Int){x, y}, flip_h, flip_v, modulate, 1.0f);
}

void blit(Bitmap* dst, Bitmap* src, int x, int y, uint32_t modulate) {
	if (src == NULL) {
		printf("passed a NULL as src to blit dude!\n");
		return;
	}
	blit_ex(dst, src, (RectInt){0, 0, src->size.x, src->size.y}, (Vector2Int){x, y}, false, false, modulate, 1.0f);
}

void blit_transformed(Bitmap* dst, Bitmap* src, RectInt rect, Vector2Int position, Vector2 scale, float angle, Vector2 pivot, uint32_t modulate, float alpha) {
	rect.w = rect.w > 0 ? rect.w : src->size.x;
	rect.h = rect.h > 0 ? rect.h : src->size.y;
	pivot.x *= src->size.x;
	pivot.y *= src->size.y;

	alpha = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);

	float c = cosf(angle);
	float s = sinf(angle);
	bool flip_x = scale.x < 0;
	bool flip_y = scale.y < 0;
	float sx = fabsf(scale.x);
	float sy = fabsf(scale.y);

	Vector2 corners[4] = {
		{0, 0},
		{(float)rect.w, 0},
		{(float)rect.w, (float)rect.h},
		{0, (float)rect.h}
	};

	float min_x =  1e9f, min_y =  1e9f;
	float max_x = -1e9f, max_y = -1e9f;

	for (int i = 0; i < 4; i++) {
		float x = corners[i].x - pivot.x;
		float y = corners[i].y - pivot.y;
		x *= sx;
		y *= sy;
		float tx = x * c - y * s + position.x;
		float ty = x * s + y * c + position.y;
		if (tx < min_x) min_x = tx;
		if (ty < min_y) min_y = ty;
		if (tx > max_x) max_x = tx;
		if (ty > max_y) max_y = ty;
	}

	int start_x = (int)floorf(min_x);
	int end_x = (int)ceilf(max_x);
	int start_y = (int)floorf(min_y);
	int end_y = (int)ceilf(max_y);

	start_x = max(start_x, 0);
	start_y = max(start_y, 0);
	end_x = min(end_x, dst->size.x - 1);
	end_y = min(end_y, dst->size.y - 1);

	for (int dy = start_y; dy <= end_y; dy++) {
		for (int dx = start_x; dx <= end_x; dx++) {
			float lx = dx - position.x;
			float ly = dy - position.y;

			float rx =  lx * c + ly * s;
			float ry = -lx * s + ly * c;

			rx /= sx;
			ry /= sy;

			rx += pivot.x;
			ry += pivot.y;

			if (rx < 0 || ry < 0 || rx >= rect.w || ry >= rect.h)
				continue;

			int sx_i = (int)(flip_x ? (rect.x + rect.w - 1 - (int)rx) : (rect.x + (int)rx));
			int sy_i = (int)(flip_y ? (rect.y + rect.h - 1 - (int)ry) : (rect.y + (int)ry));

			uint32_t src_pix = get_pixel(src, sx_i, sy_i);

			if (!(src_pix & PIXEL_HARD_MASK))
				continue;

			if (chao_hard_tint)
				src_pix = tint_pixel(src_pix, modulate);
			else
				src_pix = modulate_pixel(src_pix, modulate);

			if (alpha < 1.0f) {
				uint32_t a = src_pix & 0xFF;
				a = (uint32_t)(a * alpha);
				src_pix = (src_pix & 0xFFFFFF00) | a;
			}

			uint32_t* dst_ptr = &dst->pixels[dy * dst->size.x + dx];
			*dst_ptr = blend_pixel(src_pix, *dst_ptr, chao_blend_mode);
		}
	}
}

///////////////////////////////
//// 3D CRAP
///////////////////////////////

void draw_model3d_m(Bitmap* dst, Camera3D* cam, Model3D* model, Matrix4 model_matrix) {
    Matrix4 mvp;
    MATRIX4_COPY(mvp, cam->projection);
    matrix4_mul(mvp, cam->view);
    matrix4_mul(mvp, model_matrix);

	Matrix4 normal_matrix;
	MATRIX4_COPY(normal_matrix, model_matrix);
	matrix4_fast_inverse_transpose_3x3(normal_matrix); 

    Matrix4 screen;
    matrix4_set_screen_space_transform(screen, screen_size.x * 0.5f, screen_size.y * 0.5f);

    for (int i = 0; i < model->tris_count; i++) {

        Vector3Int tri  = model->tris[i];
        Vector3Int tuv  = model->tri_uvs[i];

        Vector4 v1 = { model->vertices[tri.x].x, model->vertices[tri.x].y, model->vertices[tri.x].z, 1 };
        Vector4 v2 = { model->vertices[tri.y].x, model->vertices[tri.y].y, model->vertices[tri.y].z, 1 };
        Vector4 v3 = { model->vertices[tri.z].x, model->vertices[tri.z].y, model->vertices[tri.z].z, 1 };

        vector4_transform(&v1, mvp);
        vector4_transform(&v2, mvp);
        vector4_transform(&v3, mvp);

        if (v1.w <= 0 || v2.w <= 0 || v3.w <= 0) {
        	continue;
		}

        float z1 = v1.w;
        float z2 = v2.w;
        float z3 = v3.w;

        vector4_perspective_divide(&v1);
        vector4_perspective_divide(&v2);
        vector4_perspective_divide(&v3);
		v1.w = v2.w = v3.w = 1.0f;

		if ((v1.x < -1 && v2.x < -1 && v3.x < -1) ||
			(v1.x >  1 && v2.x >  1 && v3.x >  1) ||
			(v1.y < -1 && v2.y < -1 && v3.y < -1) ||
			(v1.y >  1 && v2.y >  1 && v3.y >  1))
		{
			continue;
		}
        
        float ax = v2.x - v1.x;
        float ay = v2.y - v1.y;
        float bx = v3.x - v1.x;
        float by = v3.y - v1.y;
        float cross = ax * by - ay * bx;
        if (cross <= 0) continue; // Cull clockwise triangles

        vector4_transform(&v1, screen);
        vector4_transform(&v2, screen);
        vector4_transform(&v3, screen);

        Vector2 uv1 = model->uvs[tuv.x];
        Vector2 uv2 = model->uvs[tuv.y];
        Vector2 uv3 = model->uvs[tuv.z];


		if (chao_3d_directional_light.enabled) {
			Vector3Int tni = model->tri_normals[i];
			Vector3 n1 = model->normals[tni.x];
			Vector3 n2 = model->normals[tni.y];
			Vector3 n3 = model->normals[tni.z];

			{
				Vector4 t;

				t = (Vector4){ n1.x, n1.y, n1.z, 0 };
				vector4_transform(&t, normal_matrix);
				n1 = (Vector3){ t.x, t.y, t.z };
				vector3_normalize(&n1);

				t = (Vector4){ n2.x, n2.y, n2.z, 0 };
				vector4_transform(&t, normal_matrix);
				n2 = (Vector3){ t.x, t.y, t.z };
				vector3_normalize(&n2);

				t = (Vector4){ n3.x, n3.y, n3.z, 0 };
				vector4_transform(&t, normal_matrix);
				n3 = (Vector3){ t.x, t.y, t.z };
				vector3_normalize(&n3);
			}

			vector3_normalize(&n1);
			vector3_normalize(&n2);
			vector3_normalize(&n3);

			fill_triangle_3uv_lit(
				dst,
				(int)v1.x, (int)v1.y,
				(int)v2.x, (int)v2.y,
				(int)v3.x, (int)v3.y,
				uv1.x, uv1.y,
				uv2.x, uv2.y,
				uv3.x, uv3.y,
				z1, z2, z3,
				v1, v2, v3,
				n1, n2, n3,
				chao_3d_directional_light.direction,
				model->texture
					);
		} else {
			fill_triangle_3uv(
				dst,
				(int)v1.x, (int)v1.y,
				(int)v2.x, (int)v2.y,
				(int)v3.x, (int)v3.y,
				uv1.x, uv1.y,
				uv2.x, uv2.y,
				uv3.x, uv3.y,
				z1, z2, z3,
				v1, v2, v3,
				model->texture
				);
		}
    }
}

void draw_model3d(Bitmap* dst, Camera3D* camera, Model3D* model, Vector3 position, Vector3 rotation) {
	Matrix4 model_matrix;
    matrix4_identity(model_matrix);

    Matrix4 trans;
    matrix4_set_translation(trans, position.x, position.y, position.z);
    matrix4_mul(model_matrix, trans);

    Matrix4 rot;
    matrix4_set_euler(rot, rotation.x, rotation.y, rotation.z);
    matrix4_mul(model_matrix, rot);

	draw_model3d_m(dst, camera, model, model_matrix);
}

#endif // CHAO_RENDERER_H
