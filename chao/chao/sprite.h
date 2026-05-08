#ifndef CHAO_SPRITE_H
#define CHAO_SPRITE_H

#include "renderer.h"
#include "types.h"

#define MAX_SPRITE_FRAMES 32
#define MAX_SPRITE_ANIMS 16

typedef struct {
	char name[128];
	short frames[MAX_SPRITE_FRAMES];
	short num_frames;
	float frame_delay;
	RepeatMode repeat;
} SpriteAnim;

typedef struct Sprite Sprite;
struct Sprite {
    Bitmap* bitmap;
    Transform2 transform;
    float* x;
    float* y;
    Vector2 pivot;
    bool flip_h;
    bool flip_v;

    uint32_t color;

    Vector2Int frame_size;
    unsigned short frame;

    SpriteAnim anims[MAX_SPRITE_ANIMS];
    int num_anims;
    int current_anim;
    float anim_timer;
	short anim_frame;
    bool anim_playing;
	signed char anim_direction;

    struct Sprite* children[10];

};

Sprite* sprite_create(Bitmap* bitmap, float x, float y) {
    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));

    sprite->bitmap = bitmap;
    sprite->transform = transform2_create(x, y);
    sprite->x = &sprite->transform.origin[0];
    sprite->y = &sprite->transform.origin[1];
    sprite->pivot = (Vector2){ 0.f, 0.f };
    sprite->flip_h = false;
    sprite->flip_v = false;
    sprite->color = COLOR_WHITE;
    sprite->frame_size = (Vector2Int){ bitmap->size.x, bitmap->size.y };
    sprite->frame = 0;
    sprite->num_anims = 0;
    sprite->current_anim = -1;
    sprite->anim_timer = 0.f;
    sprite->anim_playing = false;
    sprite->anim_direction = 1;

    return sprite;
}

void sprite_free(Sprite* sprite) {
    free(sprite);
}

RectInt sprite_get_src_rect(Sprite* sprite) {
	int num_x_frames = sprite->bitmap->size.x / sprite->frame_size.x;
	int frame_x = sprite->frame % num_x_frames;
	int frame_y = sprite->frame / num_x_frames;

	 return (RectInt) {
		frame_x * sprite->frame_size.x,
		frame_y * sprite->frame_size.y,
		sprite->frame_size.x,
		sprite->frame_size.y
	};
}

void sprite_draw(Bitmap* dst, Sprite* sprite, Vector2 offset) {
	Vector2 scale = { transform2_get_scale_x(&sprite->transform), transform2_get_scale_y(&sprite->transform) };
    float rotation = transform2_get_rotation(&sprite->transform);
	RectInt rect = sprite_get_src_rect(sprite);

    if (fapprox(scale.x, 1.0f) && fapprox(scale.y, 1.0f) && fapprox(rotation, 0.0f)) {
        Vector2Int pos = {
        	round((*sprite->x + offset.x) - sprite->frame_size.x * sprite->pivot.x),
    		round((*sprite->y + offset.y) - sprite->frame_size.y * sprite->pivot.y)
		};

    	blit_ex(dst, sprite->bitmap, rect, pos, sprite->flip_h, sprite->flip_v, sprite->color, 1.0f);
    } else {
        Vector2Int pos = {
        	round(*sprite->x + offset.x),
    		round(*sprite->y + offset.y)
		};
		blit_transformed(dst, sprite->bitmap, rect, pos, scale, rotation, sprite->pivot, sprite->color, 1.0f);
    }
}

void sprite_update(Sprite* sprite, float dt) {
	if (sprite->current_anim != -1 && sprite->anim_playing) {
		SpriteAnim* anim = &sprite->anims[sprite->current_anim];
		sprite->anim_timer += dt;
		if (sprite->anim_timer >= anim->frame_delay) {
			sprite->anim_frame += sprite->anim_direction;
			if (sprite->anim_frame >= anim->num_frames || sprite->anim_frame < 0) {
				switch (anim->repeat) {
					case REPEAT_ONCE:
						sprite->anim_playing = false;
						sprite->anim_frame -= sprite->anim_direction;
						break;
					case REPEAT_LOOP:
						sprite->anim_frame = sprite->anim_direction > 0 ? 0 : anim->num_frames - 1;
						break;
					case REPEAT_BOUNCE:
						sprite->anim_direction = -sprite->anim_direction;
						sprite->anim_frame += sprite->anim_direction;
						break;
					case REPEAT_BOUNCE_ONCE:
						sprite->anim_direction = -sprite->anim_direction;
						sprite->anim_frame += sprite->anim_direction;
						if (sprite->anim_direction > 0) {
							sprite->anim_playing = false;
						}
						break;
				}
			}
			sprite->frame = anim->frames[sprite->anim_frame];
			sprite->anim_timer = 0.0f;
		}
	}
}

void sprite_draw_child(Sprite* sprite, Transform2 transform) {
}

SpriteAnim* sprite_add_anim(Sprite* sprite, const char* name, float frame_delay, short* frames, RepeatMode repeat) {
	SpriteAnim* anim = &sprite->anims[sprite->num_anims];
	strcpy(anim->name, name);
	anim->frame_delay = frame_delay;
	anim->num_frames = 0;
	sprite->anim_frame = 0;
	while (frames[anim->num_frames] != ARR_END) {
		anim->num_frames ++;
	}
	for (int i = 0; i < anim->num_frames; ++i) {
		anim->frames[i] = frames[i];
	}
	sprite->num_anims ++;
	anim->repeat = repeat;

	return anim;
}

void sprite_play_anim(Sprite* sprite, const char* name, bool force) {
	for (int i = 0; i < sprite->num_anims; ++i) {
		if (strcmp(sprite->anims[i].name, name) == 0) {
			if (sprite->current_anim != i || force) {
				sprite->current_anim = i;
				sprite->frame = sprite->anims[i].frames[0];
				sprite->anim_timer = 0.f;
				sprite->anim_frame = 0;
				sprite->anim_playing = true;
				sprite->anim_direction = 1;
			}
			break;
		}
	}
}

Vector2 sprite_get_position(Sprite* sprite) {
	return (Vector2){
    	sprite->transform.origin[0],
    	sprite->transform.origin[1],
	};
}

#endif // CHAO_SPRITE_H
