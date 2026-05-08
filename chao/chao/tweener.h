#ifndef CHAO_TWEENER_H
#define CHAO_TWEENER_H

#include <math.h>

#define MAX_TWEENS 256
#define MAX_DELAYED_CALLS 64

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

typedef enum {
    EASE_LINEAR,
    EASE_SINE_IN,
    EASE_SINE_OUT,
    EASE_SINE_IN_OUT,
    EASE_BACK_IN,
    EASE_BACK_OUT,
    EASE_BACK_IN_OUT,
    EASE_BOUNCE_IN,
    EASE_BOUNCE_OUT,
    EASE_BOUNCE_IN_OUT,
    EASE_ELASTIC_IN,
    EASE_ELASTIC_OUT,
    EASE_ELASTIC_IN_OUT,
} Easing;

typedef enum {
	TWEEN_TYPE_INT,
	TWEEN_TYPE_FLOAT,
	TWEEN_TYPE_VECTOR2,
	TWEEN_TYPE_COLOR,
} TweenType;

typedef enum {
	TWEEN_REPEAT_ONCE,
	TWEEN_REPEAT_LOOP,
	TWEEN_REPEAT_BOUNCE,
	TWEEN_REPEAT_BOUNCE_ONCE,
} TweenRepeatMode;

typedef union {
	int int_value;
	float float_value;
	Vector2 vector2_value;
	uint32_t color_value;
} TweenedValue;

typedef union {
	int* int_value;
	float* float_value;
	Vector2* vector2_value;
	uint32_t* color_value;
} TweenedValuePointer;

typedef struct {
	bool active;
	TweenType type;
	Easing easing;
	TweenRepeatMode repeat_mode;
	TweenedValue from;
	TweenedValue to;
	TweenedValuePointer target;
	float timer;
	float duration;
	float delay;
	signed char direction;
} Tween;

typedef struct {
	float timer;
	float delay;
	bool looped;
	void (*callback)();
} DelayedCall;

Tween _tweens[MAX_TWEENS];
DelayedCall _delayed_calls[MAX_DELAYED_CALLS];

float tweener_ease(float v, Easing easing);
void tweener_update_value(int i);

void tweener_update(float dt) {
	for (int i = 0; i < MAX_DELAYED_CALLS; ++i) {
		if (_delayed_calls[i].callback == NULL) {
			continue;
		}

		_delayed_calls[i].timer += dt;
		if (_delayed_calls[i].timer >= _delayed_calls[i].delay) {
			(*_delayed_calls[i].callback)();
			if (_delayed_calls[i].looped) {
				_delayed_calls[i].timer = 0.0f;
			} else {
				_delayed_calls[i].callback = NULL;
			}
		}
	}

    for (int i = 0; i < MAX_TWEENS; ++i) {
        if (!_tweens[i].active) {
            continue;
        }
        
        if (_tweens[i].delay > 0.0f) {
            _tweens[i].delay -= dt;
            continue;
        }
        
        _tweens[i].timer += dt;
        
        if (_tweens[i].timer >= _tweens[i].duration) {
            _tweens[i].timer = _tweens[i].duration;
            
            switch (_tweens[i].repeat_mode) {
                case TWEEN_REPEAT_ONCE:
                    _tweens[i].active = false;
                    break;
            	case TWEEN_REPEAT_LOOP:
            	   _tweens[i].timer = 0.0f;
            	   break;
            	case TWEEN_REPEAT_BOUNCE:
            	   _tweens[i].timer = 0.0f;
            	   _tweens[i].direction = -_tweens[i].direction;
            	   break;
            	case TWEEN_REPEAT_BOUNCE_ONCE:
            	   _tweens[i].repeat_mode = TWEEN_REPEAT_ONCE;
            	   _tweens[i].timer = 0.0f;
            	   _tweens[i].direction = -_tweens[i].direction;
            	   break;
            }
        }
        
        tweener_update_value(i);
    }
}

void tweener_remove_all() {
	for (int i = 0; i < MAX_TWEENS; ++i) {
		_tweens[i].active = false;
	}
	for (int i = 0; i < MAX_TWEENS; ++i) {
    	_tweens[i].active = false;
	}
}

void tweener_remove(void* target) {
	for (int i = 0; i < MAX_TWEENS; ++i) {
	   switch (_tweens[i].type) {
            case TWEEN_TYPE_INT: 
                if (_tweens[i].target.int_value == (int*)target) {
                    _tweens[i].active = false;
                }
                break;
            case TWEEN_TYPE_FLOAT: 
                if (_tweens[i].target.float_value == (float*)target) {
                    _tweens[i].active = false;
                }
                break;
            case TWEEN_TYPE_VECTOR2: 
                if (_tweens[i].target.vector2_value == (Vector2*)target) {
                    _tweens[i].active = false;
                }
                break;
            case TWEEN_TYPE_COLOR: 
                if (_tweens[i].target.color_value == (uint32_t*)target) {
                    _tweens[i].active = false;
                }
                break;
	   }
   }
}

int tweener_init_tween(TweenType type, float duration, float delay, Easing easing, TweenRepeatMode repeat_mode) {
	int i = 0;
	for (i = 0; i < MAX_TWEENS; ++i) {
		if (!_tweens[i].active) {
			break;
		}
	}
	_tweens[i].type = type;
	_tweens[i].active = true;
	_tweens[i].timer = 0.0f;
	_tweens[i].duration = duration;
	_tweens[i].delay = delay;
	_tweens[i].easing = easing;
	_tweens[i].repeat_mode = repeat_mode;
	_tweens[i].direction = 1;

	return i;
}

void tweener_wrap_up_tween(int i) {
	if (_tweens[i].delay <= 0.f) {
		tweener_update_value(i);
	}
}

Tween* tween_int(int *value, int to, float duration, float delay, Easing easing, TweenRepeatMode repeat_mode) {
	int i = tweener_init_tween(TWEEN_TYPE_INT, duration, delay, easing, repeat_mode);
	_tweens[i].from.int_value = *value;
	_tweens[i].to.int_value = to; 
	_tweens[i].target.int_value = value;
	tweener_wrap_up_tween(i);
	return &_tweens[i];
}


Tween* tween_float(float *value, float to, float duration, float delay, Easing easing, TweenRepeatMode repeat_mode) {
	int i = tweener_init_tween(TWEEN_TYPE_FLOAT, duration, delay, easing, repeat_mode);
	_tweens[i].from.float_value = *value;
	_tweens[i].to.float_value = to; 
	_tweens[i].target.float_value = value;
	tweener_wrap_up_tween(i);
	return &_tweens[i];
}

Tween* tween_vector2(Vector2 *value, Vector2 to, float duration, float delay, Easing easing, TweenRepeatMode repeat_mode) {
	int i = tweener_init_tween(TWEEN_TYPE_VECTOR2, duration, delay, easing, repeat_mode);
	_tweens[i].from.vector2_value = *value;
	_tweens[i].to.vector2_value = to; 
	_tweens[i].target.vector2_value = value;
	tweener_wrap_up_tween(i);
	return &_tweens[i];
}

Tween* tween_color(uint32_t *value, uint32_t to, float duration, float delay, Easing easing, TweenRepeatMode repeat_mode) {
	int i = tweener_init_tween(TWEEN_TYPE_COLOR, duration, delay, easing, repeat_mode);
	_tweens[i].from.color_value = *value;
	_tweens[i].to.color_value = to;
	_tweens[i].target.color_value = value;
	tweener_wrap_up_tween(i);
	return &_tweens[i];
}

void tweener_update_value(int i) {
	float v = tweener_ease(_tweens[i].timer / _tweens[i].duration, _tweens[i].easing);

	if (_tweens[i].direction == -1) {
		v = 1.f - v;
	}

	switch (_tweens[i].type) {
		case TWEEN_TYPE_INT: {
			int a = _tweens[i].from.int_value;
			int b = _tweens[i].to.int_value;
			*(_tweens[i].target.int_value) = lerp(a, b, v);
			break;
		}
		case TWEEN_TYPE_FLOAT: {
			float a = _tweens[i].from.float_value;
			float b = _tweens[i].to.float_value;
			*(_tweens[i].target.float_value) = lerp(a, b, v);
			break;
		}
		case TWEEN_TYPE_VECTOR2: {
			Vector2 v1 = _tweens[i].from.vector2_value;
			Vector2 v2 = _tweens[i].to.vector2_value;
			_tweens[i].target.vector2_value->x = lerp(v1.x, v2.x, v);
			_tweens[i].target.vector2_value->y = lerp(v1.y, v2.y, v);
			break;
		}
		case TWEEN_TYPE_COLOR: {
			uint32_t c1 = _tweens[i].from.color_value;
			uint32_t c2 = _tweens[i].to.color_value;
			*(_tweens[i].target.color_value) = lerp_color(c1, c2, v);
		}
	}
}

float tweener_ease(float v, Easing easing) {
    switch (easing) {
        case EASE_SINE_IN:
            return 1 - cos((v * PI) / 2);

        case EASE_SINE_OUT:
            return sin((v * PI) / 2);

        case EASE_SINE_IN_OUT:
            return -(cos(PI * v) - 1) / 2;

        case EASE_BACK_IN: {
            float c1 = 1.70158f;
            float c3 = c1 + 1;
            return c3 * v * v * v - c1 * v * v;
        }

        case EASE_BACK_OUT: {
            float c1 = 1.70158f;
            float c3 = c1 + 1;
            return 1 + c3 * pow(v - 1, 3) + c1 * pow(v - 1, 2);
        }

        case EASE_BACK_IN_OUT: {
            float c1 = 1.70158f;
            float c2 = c1 * 1.525f;
            return v < 0.5f ? (pow(2 * v, 2) * ((c2 + 1) * 2 * v - c2)) / 2 
                            : (pow(2 * v - 2, 2) * ((c2 + 1) * (2 * v - 2) + c2) + 2) / 2;
        }

        case EASE_BOUNCE_IN:
            return 1 - tweener_ease(1 - v, EASE_BOUNCE_OUT);

        case EASE_BOUNCE_OUT:
            if (v < 1 / 2.75f) {
                return 7.5625f * v * v;
            } else if (v < 2 / 2.75f) {
                v -= 1.5f / 2.75f;
                return 7.5625f * v * v + 0.75f;
            } else if (v < 2.5f / 2.75f) {
                v -= 2.25f / 2.75f;
                return 7.5625f * v * v + 0.9375f;
            } else {
                v -= 2.625f / 2.75f;
                return 7.5625f * v * v + 0.984375f;
            }

        case EASE_BOUNCE_IN_OUT:
            return v < 0.5f 
                ? (1 - tweener_ease(1 - 2 * v, EASE_BOUNCE_OUT)) / 2 
                : (1 + tweener_ease(2 * v - 1, EASE_BOUNCE_OUT)) / 2;

        case EASE_ELASTIC_IN:
            return v == 0 ? 0 
                 : v == 1 ? 1 
                 : -pow(2, 10 * v - 10) * sin((v * 10 - 10.75f) * ((2 * PI) / 3));

        case EASE_ELASTIC_OUT:
            return v == 0 ? 0 
                 : v == 1 ? 1 
                 : pow(2, -10 * v) * sin((v * 10 - 0.75f) * ((2 * PI) / 3)) + 1;

        case EASE_ELASTIC_IN_OUT:
            return v == 0 ? 0 
                 : v == 1 ? 1 
                 : v < 0.5f 
                     ? -(pow(2, 20 * v - 10) * sin((20 * v - 11.125f) * ((2 * PI) / 4.5f))) / 2 
                     : (pow(2, -20 * v + 10) * sin((20 * v - 11.125f) * ((2 * PI) / 4.5f))) / 2 + 1;

        default:
            return v;
    }
}

DelayedCall* tweener_delayed_call(float delay, void(*callback)()) {
	for (int i = 0; i < MAX_DELAYED_CALLS; ++i) {
		if (_delayed_calls[i].callback == NULL) {
			_delayed_calls[i].callback = callback;
			_delayed_calls[i].delay = delay;
			_delayed_calls[i].timer = 0.0f;
			_delayed_calls[i].looped = false;
			return &_delayed_calls[i];
		}
	}
	return NULL;
}

#endif // CHAO_TWEENER_H
