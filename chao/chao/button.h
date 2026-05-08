#ifndef CHAO_BUTTON_H
#define CHAO_BUTTON_H

typedef enum {
    BPA_PUSH = 0,
    BPA_TINT,
    BPA_NONE
} ButtonPressAnim;

typedef struct {
    Bitmap* frame;
    Bitmap* icon;
    AsciiFont* font;
    char text[128];
    bool enabled;
    Vector2Int pos;
    Vector2 pivot;
    Vector2Int icon_offset;
    uint32_t icon_color;
    bool icon_flip_h;
    bool icon_flip_v;
    ButtonPressAnim press_anim;
    bool is_down;
} Button;

Button button_create(Bitmap* frame, Bitmap* icon, AsciiFont* font, const char* text) {
    Button button = {
        .frame = frame,
        .icon = icon,
        .font = font,
        .enabled = true,
    	.is_down = false,
    };
    if (text) {
		strcpy(button.text, text);
	} else {
		button.text[0] = 0;
	}
    button.pos = (Vector2Int){-9999, -9999};
    button.pivot = (Vector2){0.5f, 0.5f};
    button.icon_offset = VECTOR2I_ZERO;
    button.icon_color = COLOR_WHITE;
    button.press_anim = BPA_PUSH;
    return button;
}

bool button_mouse_over(Button* button) {
    if (!button->enabled) {
        return false;
    }
    return input.mouse.x >= button->pos.x && input.mouse.x <= button->pos.x + button->frame->size.x && input.mouse.y >= button->pos.y && input.mouse.y <= button->pos.y + button->frame->size.y;
}

bool button_is_down(Button* button) {
    return input.mouse.pressed && button_mouse_over(button);
}

bool button_is_pressed(Button* button) {
    return input.mouse.just_released && button_mouse_over(button);
}

void button_draw(Bitmap* dst, Button* button, int x, int y) {
    int w = button->frame->size.x;
    int h = button->frame->size.y;
    
    x -= button->pivot.x * w;
    y -= button->pivot.y * h;
    button->pos.x = x;
    button->pos.y = y;
    
    int top_y = y;
    
    switch (button->press_anim) {
        case BPA_PUSH: {
            top_y -= ((button->is_down || !button->enabled) ? 0 : 2);
            
            blit_rect(dst, button->frame, (RectInt){0,0,w,h}, (Vector2Int){x, y}, COLOR_WHITE);
            blit_rect(dst, button->frame, (RectInt){0,0,w,h}, (Vector2Int){x, top_y}, COLOR_WHITE);
            break;
        }
        case BPA_TINT: {
            uint32_t tint = button->is_down ? COLOR_LIGHTGRAY : COLOR_WHITE;
            blit_rect(dst, button->frame, (RectInt){0,0,w,h}, (Vector2Int){x, top_y}, tint);
            break;
        }
        case BPA_NONE: {
            blit_rect(dst, button->frame, (RectInt){0,0,w,h}, (Vector2Int){x, top_y}, COLOR_WHITE);
        }
    }
    
    if (button->icon) {
    	Vector2Int pos = {
			x + (w - button->icon->size.x) / 2,
			top_y + (h - button->icon->size.y) / 2
		};
        pos.x += button->icon_offset.x;
        pos.y += button->icon_offset.y;
        blit_ex(dst, button->icon, BMP_RECT(button->icon), pos, button->icon_flip_h, button->icon_flip_v, button->icon_color, 1.0f);

    }
    
    if (button->font) {
        Vector2Int s = ascii_font_get_size(button->font, button->text, -1);
        int tx = x + (w - s.x) / 2;
        int ty = top_y + (h - s.y) / 2;
        ascii_font_draw(dst, button->font, tx, ty, COLOR_WHITE, button->text);
    }
}

void button_update(Button* button, float dt) {
	button->is_down = button_is_down(button);
}

#endif // CHAO_BUTTON_H
