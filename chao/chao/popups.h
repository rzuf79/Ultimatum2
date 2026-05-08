#ifndef CHAO_POPUP_H
#define CHAO_POPUP_H

#define POPUPS_STACK_SIZE 8

typedef struct {
	Bitmap* frame;
	Bitmap* content;
	Bitmap* background;
	Vector2Int size;
} Popup;

Popup* popups_stack[POPUPS_STACK_SIZE];

Popup* popup_create(Bitmap* frame_bitmap, Vector2Int size, int frame_margins) {
	Popup* new_popup = (Popup*)malloc(sizeof(Popup));

	NineSlice frame_slice = {
		.bitmap = frame_bitmap,
		.top = frame_margins,
		.bottom = frame_margins,
		.left = frame_margins,
		.right = frame_margins,
		.stretch_mode = SLICE_MODE_STRETCH,
	};

	new_popup->frame = nine_slice_bake(NULL, frame_slice, 0, 0, size.x, size.y);

	return new_popup;
}

void popups_draw() {
	for (int i = 0; i < POPUPS_STACK_SIZE; ++i) {
		Popup* popup = popups_stack[i];
		if (popup != NULL) {
			blit(chao_canvas, popup->frame, (screen_size.x - popup->size.x) / 2, (screen_size.y - popup->size.y) / 2, COLOR_WHITE);
		}
	}
}

void popup_free(Popup* popup) {
	for (int i = 0; i < POPUPS_STACK_SIZE; ++i) {
		if (popups_stack[i] == popup) {
			popups_stack[i] = NULL;
		}
	}
	free_bitmap(popup->frame);
	free_bitmap(popup->content);
	free(popup);
}

void popups_remove_all() {
	for (int i = 0; i < POPUPS_STACK_SIZE; ++i) {
		if (popups_stack[i] != NULL) {
			popup_free(popups_stack[i]);
		}
		popups_stack[i] = NULL;
	}
}

void popups_on_window_resize() {
}

#endif // CHAO_POPUP_H
