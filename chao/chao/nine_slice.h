#ifndef CHAO_NINE_SLICE_H
#define CHAO_NINE_SLICE_H

typedef enum {
    SLICE_MODE_TILE,
    SLICE_MODE_STRETCH
} SliceStretchMode;

typedef struct {
    Bitmap* bitmap;
    int top;
    int bottom;
    int left;
    int right;
    SliceStretchMode stretch_mode;
} NineSlice;

int nice_slice_stretch(int x, int src_w, int dst_w, SliceStretchMode mode) {
    switch (mode) {
        case SLICE_MODE_TILE:
            return x % src_w;
        case SLICE_MODE_STRETCH:
            return (x * src_w) / dst_w;
    }
    return 0;
}

void nine_slice_draw(Bitmap* dst, NineSlice slice, int x, int y, int w, int h) {
    blit_rect(dst, slice.bitmap, (RectInt){0, 0, slice.left, slice.top}, (Vector2Int){x, y}, COLOR_WHITE);
    blit_rect(dst, slice.bitmap, (RectInt){slice.bitmap->size.x - slice.right, 0, slice.right, slice.top}, (Vector2Int){x + (w - slice.right), y}, COLOR_WHITE);
    blit_rect(dst, slice.bitmap, (RectInt){0, slice.bitmap->size.y - slice.bottom, slice.left, slice.bottom}, (Vector2Int){x, y + (h - slice.bottom)}, COLOR_WHITE);
    blit_rect(dst, slice.bitmap, (RectInt){slice.bitmap->size.x - slice.right, slice.bitmap->size.y - slice.bottom, slice.right, slice.bottom}, (Vector2Int){x + (w - slice.right), y + (h - slice.bottom)}, COLOR_WHITE);

    int border_src_w = slice.bitmap->size.x - (slice.left + slice.right);
    int border_src_h = slice.bitmap->size.y - (slice.top + slice.bottom);
    int border_dst_w = w - (slice.left + slice.right);
    int border_dst_h = h - (slice.top + slice.bottom);

    uint32_t* srcptr_top = slice.bitmap->pixels + slice.left;
    uint32_t* dstptr_top = dst->pixels + (y * dst->size.x) + x + slice.left;
    for (int yi = 0; yi < slice.top; ++yi) {
        for (int xi = 0; xi < border_dst_w; ++xi) {
            int src_x = nice_slice_stretch(xi, border_src_w, border_dst_w, slice.stretch_mode);
            dstptr_top[xi] = srcptr_top[src_x];
        }
        srcptr_top += slice.bitmap->size.x;
        dstptr_top += dst->size.x;
    }

    uint32_t* srcptr_bottom = slice.bitmap->pixels + slice.left + slice.bitmap->size.x * (slice.bitmap->size.y - slice.bottom);
    uint32_t* dstptr_bottom = dst->pixels + ((y + h - slice.bottom) * dst->size.x) + x + slice.left;
    for (int yi = 0; yi < slice.bottom; ++yi) {
        for (int xi = 0; xi < border_dst_w; ++xi) {
            int src_x = nice_slice_stretch(xi, border_src_w, border_dst_w, slice.stretch_mode);
            dstptr_bottom[xi] = srcptr_bottom[src_x];
        }
        srcptr_bottom += slice.bitmap->size.x;
        dstptr_bottom += dst->size.x;
    }

    for (int yi = 0; yi < border_dst_h; ++yi) {
        int src_y = nice_slice_stretch(yi, border_src_h, border_dst_h, slice.stretch_mode);
        uint32_t* srcptr_left = slice.bitmap->pixels + (slice.top + src_y) * slice.bitmap->size.x;
        uint32_t* dstptr_left = dst->pixels + (y + slice.top + yi) * dst->size.x + x;
        for (int xi = 0; xi < slice.left; ++xi) {
            dstptr_left[xi] = srcptr_left[xi];
        }
    }

    for (int yi = 0; yi < border_dst_h; ++yi) {
        int src_y = nice_slice_stretch(yi, border_src_h, border_dst_h, slice.stretch_mode);
        uint32_t* srcptr_right = slice.bitmap->pixels + (slice.top + src_y) * slice.bitmap->size.x + (slice.bitmap->size.x - slice.right);
        uint32_t* dstptr_right = dst->pixels + (y + slice.top + yi) * dst->size.x + (x + w - slice.right);
        for (int xi = 0; xi < slice.right; ++xi) {
            dstptr_right[xi] = srcptr_right[xi];
        }
    }

    uint32_t* srcptr_middle = slice.bitmap->pixels + slice.left + slice.top * slice.bitmap->size.x;
    uint32_t* dstptr_middle = dst->pixels + (y + slice.top) * dst->size.x + (x + slice.left);
    for (int yi = 0; yi < border_dst_h; ++yi) {
        for (int xi = 0; xi < border_dst_w; ++xi) {
            int src_x = nice_slice_stretch(xi, border_src_w, border_dst_w, slice.stretch_mode);
            int src_y = nice_slice_stretch(yi, border_src_h, border_dst_h, slice.stretch_mode);
            int src_index = src_x + src_y * slice.bitmap->size.x;
            dstptr_middle[xi] = slice.bitmap->pixels[(slice.top * slice.bitmap->size.x) + slice.left + src_index];
        }
        dstptr_middle += dst->size.x;
    }
}

Bitmap* nine_slice_bake(const char* id, NineSlice slice, int x, int y, int w, int h) {
    Bitmap* bmp = create_bitmap(w, h);
    nine_slice_draw(bmp, slice, x, y, w, h);
    if (id != NULL) {
		assets_add_bitmap(id, bmp);
	}
	return bmp;
}


#endif // CHAO_NINE_SLICE_H
