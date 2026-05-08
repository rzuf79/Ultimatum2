#ifndef CHAO_UTILS_H
#define CHAO_UTILS_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ARR_END -1
#define ARR(...) ((short[]){__VA_ARGS__, ARR_END})

#define deg_to_rad(degs) ((degs) * M_PI / 180.0)
#define rad_to_deg(rads) ((rads) * 180.0 / M_PI)

#define sign(T, x) ((T)((x) > 0) - (T)((x) < 0))

#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)
#define clamp(a, min, max) (a < min ? min : a > max ? max : a)
#define clamp01(a) (a < 0 ? 0 : a > 1 ? 1 : a)

#define move_towards(from, to, delta) ((fabs((to) - (from)) <= (delta)) ? (to) : ((to) > (from) ? (from) + (delta) : (from) - (delta)))

#define swap(T, a, b) do { T t = a; a = b; b = t; } while(0)

#define FORMAT_TEXT(buffer, fmt)              \
    do {                                      \
        va_list args;                         \
        va_start(args, fmt);                  \
        vsnprintf(buffer, sizeof(buffer), fmt, args); \
        va_end(args);                         \
    } while (0)

static inline bool fapprox(float a, float b) {
    return fabs(a - b) < 0.001f;
}

static inline int wrap_int(int a, int min, int max) {
    while (a < min) a += max-min;
    while (a > max) a -= max-min;
    return a;
}

static inline float lerp(float start, float end, float t) {
    return start + (end - start) * t;
}

static inline float inverse_lerp(float start, float end, float current) {
    if (start == end) return 0.0f;
    float t = (current - start) / (end - start);
    return fmaxf(0.0f, fminf(1.0f, t));
}

static inline float lerp_angle(float start, float end, float t) {
    start = fmod(start, 2 * M_PI);
    end = fmod(end, 2 * M_PI);

    float delta = end - start;

    if (delta > M_PI) {
        delta -= 2 * M_PI;
    } else if (delta < -M_PI) {
        delta += 2 * M_PI;
    }

    return start + delta * t;
}

static inline uint32_t lerp_color(uint32_t color1, uint32_t color2, float v) {
    uint8_t r1 = (color1 >> 24) & 0xFF;
    uint8_t g1 = (color1 >> 16) & 0xFF;
    uint8_t b1 = (color1 >> 8)  & 0xFF;
    uint8_t a1 = color1         & 0xFF;

    uint8_t r2 = (color2 >> 24) & 0xFF;
    uint8_t g2 = (color2 >> 16) & 0xFF;
    uint8_t b2 = (color2 >> 8)  & 0xFF;
    uint8_t a2 = color2         & 0xFF;

    uint8_t r = (uint8_t)round(r1 + (r2 - r1) * v);
    uint8_t g = (uint8_t)round(g1 + (g2 - g1) * v);
    uint8_t b = (uint8_t)round(b1 + (b2 - b1) * v);
    uint8_t a = (uint8_t)round(a1 + (a2 - a1) * v);

    return (r << 24) | (g << 16) | (b << 8) | a;
}

static inline uint32_t mix_colors3(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t u1, uint32_t u2, uint32_t det) {
    if (det == 0) return 0;

    uint32_t u3 = det - u1 - u2;

    uint8_t r1 = (c1 >> 24) & 0xFF;
    uint8_t g1 = (c1 >> 16) & 0xFF;
    uint8_t b1 = (c1 >> 8)  & 0xFF;
    uint8_t a1 =  c1        & 0xFF;

    uint8_t r2 = (c2 >> 24) & 0xFF;
    uint8_t g2 = (c2 >> 16) & 0xFF;
    uint8_t b2 = (c2 >> 8)  & 0xFF;
    uint8_t a2 =  c2        & 0xFF;

    uint8_t r3 = (c3 >> 24) & 0xFF;
    uint8_t g3 = (c3 >> 16) & 0xFF;
    uint8_t b3 = (c3 >> 8)  & 0xFF;
    uint8_t a3 =  c3        & 0xFF;

    uint8_t r = (uint8_t)((r1 * u1 + r2 * u2 + r3 * u3 + det / 2) / det);
    uint8_t g = (uint8_t)((g1 * u1 + g2 * u2 + g3 * u3 + det / 2) / det);
    uint8_t b = (uint8_t)((b1 * u1 + b2 * u2 + b3 * u3 + det / 2) / det);
    uint8_t a = (uint8_t)((a1 * u1 + a2 * u2 + a3 * u3 + det / 2) / det);

    return (r << 24) | (g << 16) | (b << 8) | a;
}

static inline uint32_t string_to_hex_color(const char *str) {
    if (str[0] == '#') {
        str++;
    }
    return (uint32_t)strtol(str, NULL, 16);
}

static inline void randomize() {
    srand((unsigned int)time(NULL));
}

static inline void shuffle_array(void *array, size_t length, size_t element_size) {
    char *arr = array;
    char *temp = malloc(element_size);
    if (!temp) return;
    for (size_t i = length - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        memcpy(temp, arr + i * element_size, element_size);
        memcpy(arr + i * element_size, arr + j * element_size, element_size);
        memcpy(arr + j * element_size, temp, element_size);
    }
    free(temp);
}

static inline void shuffle_pointer_array(void **arr, size_t length) {
    for (size_t i = length - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        void *temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

#define get_array_random(array, length) (array[rand() % (length)])

static inline int coin_flip(int chance) {
    return (rand() % 100) < chance;
}

static inline float rand_range(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

static inline int randi_range(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

static inline unsigned long hash(const char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}
    

#endif // CHAO_UTILS_H
