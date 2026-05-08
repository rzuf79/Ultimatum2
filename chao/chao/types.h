#ifndef CHAO_TYPES_H
#define CHAO_TYPES_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* forward declaration crap */
static inline float clamp01(float x);
/**/

typedef enum {
    BLEND_NONE,
    BLEND_ALPHA,
    BLEND_ADDITIVE,
    BLEND_MULTIPLY
} BlendMode;

typedef enum {
    REPEAT_ONCE = 0,
    REPEAT_LOOP,
    REPEAT_BOUNCE,
    REPEAT_BOUNCE_ONCE
} RepeatMode;

typedef struct {
    int x, y;
} Vector2Int;

typedef struct {
    float x, y;
} Vector2;

typedef struct {
    int x, y, z;
} Vector3Int;

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float x, y, z, w;
} Vector4;

typedef struct {
    float x, y, z, w;
} Quaternion;

typedef struct {
    float x, y, w, h;
} Rect;

typedef struct {
    int x, y, w, h;
} RectInt;

typedef struct {
    float x[2], y[2], origin[2];
} Transform2;

typedef float Matrix4[4][4];

typedef struct {
    uint32_t* pixels;
    Vector2Int size;
    float* z_buffer;
} Bitmap;

typedef struct {
    Vector3* vertices;
    Vector3* normals;
    Vector2* uvs;
    Vector3Int* tris;
    int tris_count;
    Vector3Int* tri_uvs;
    Vector3Int* tri_normals;
    Bitmap* texture;
} Model3D;

typedef struct {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    Matrix4 view;
    Matrix4 projection;
} Camera3D;

const Vector3 VECTOR3_ZERO = {0,0,0};
const Vector3 VECTOR3_ONE = {1,1,1};
const Vector3 VECTOR3_UP = {0,1,0};

const Vector2Int VECTOR2I_ZERO = {0, 0};
const Vector2Int VECTOR2I_ONE = {1, 1};
const Vector2 VECTOR2_ZERO = {0.0f, 0.0f};
const Vector2 VECTOR2_ONE = {1.0f, 1.0f};

#define rect_point_in(rect, x, y) (((x) >= (rect).x) && ((x) <= (rect).x + (rect).w) && ((y) >= (rect).y) && ((y) <= (rect).y + (rect).h))


static inline bool vector2int_compare(const Vector2Int* a, const Vector2Int* b) {
    return a->x == b->x && a->y == b->y;
}
static inline Vector2Int vector2int_minus(const Vector2Int* v) {
    return (Vector2Int){-v->x, -v->y};
}

/* VECTOR2 */
static inline float vector2_length(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}
static inline float vector2_distance(Vector2 a, Vector2 b) {
    Vector2 v = { b.x - a.x, b.y - a.y };
    return vector2_length(v);
}
static inline void vector2_normalize(Vector2* v) {
    float length = vector2_length(*v);
    if (length == 0.0f) return;
    v->x /= length;
    v->y /= length;
}
static inline Vector2 vector2_get_normalized(Vector2 v) {
    float length = vector2_length(v);
    if (length == 0.0f) return (Vector2){0.0f, 0.0f};
    return (Vector2){v.x/length, v.y/length};
}
static inline Vector2 vector2_direction(Vector2 v1, Vector2 v2) {
    Vector2 v = { v2.x - v1.x, v2.y - v1.y };
    vector2_normalize(&v);
    return v;
}
static inline Vector2 vector2_add(Vector2 a, Vector2 b) {
    return (Vector2){a.x + b.x, a.y + b.y};
}
static inline Vector2 vector2_subtract(Vector2 a, Vector2 b) {
    return (Vector2){a.x - b.x, a.y - b.y};
}
static inline float vector2_dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}
static inline Vector2 vector2_scale(Vector2 v, float scalar) {
    return (Vector2){v.x * scalar, v.y * scalar};
}
static inline Vector2 vector2_minus(Vector2 v) {
    return (Vector2){-v.x, -v.y};
}
static inline void vector2_rotate(Vector2* v, float rads) {
    float c = cosf(rads);
    float s = sinf(rads);
    float x = v->x * c - v->y * s;
    float y = v->x * s + v->y * c;
    v->x = x;
    v->y = y;
}

/* VECTOR3 */
static inline float vector3_length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
static inline Vector3 vector3_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}
static inline Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}
static inline Vector2 vector2_multiply(Vector2 a, Vector2 b) {
    return (Vector2){a.x * b.x, a.y * b.y};
}
static inline Vector2 vector2_multiply_scalar(Vector2 a, const float scalar) {
    return (Vector2){a.x * scalar, a.y * scalar};
}
static inline float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static inline Vector3 vector3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
}
static inline Vector3 vector3_scale(Vector3 v, float scalar) {
    return (Vector3){v.x * scalar, v.y * scalar, v.z * scalar};
}
static inline float vector3_distance(Vector3 v1, Vector3 v2) {
    Vector3 v = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    return vector3_length(v);
}
static inline void vector3_normalize(Vector3* v) {
    float len = vector3_length(*v);
    if (len == 0) return;
    v->x /= len;
    v->y /= len;
    v->z /= len;
}
static inline Vector3 vector3_get_normalized(Vector3 v) {
    float length = vector3_length(v);
    if (length == 0.0f) return (Vector3){0.0f, 0.0f, 0.0f};
    return (Vector3){v.x / length, v.y / length, v.z / length};
}
static inline Vector3 vector3_direction(Vector3 v1, Vector3 v2) {
    Vector3 v = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    return vector3_get_normalized(v);
}
static inline Vector3 vector3_multiply(Vector3 a, Vector3 b) {
    return (Vector3){a.x * b.x, a.y * b.y, a.z * b.z};
}
static inline Vector3 vector3_multiply_scalar(Vector3 a, const float scalar) {
    return (Vector3){a.x * scalar, a.y * scalar, a.z * scalar};
}
static inline Vector3 vector3_minus(Vector3 v) {
    return (Vector3){-v.x, -v.y, -v.z};
}
static inline void vector3_transform(Vector3* out, const Matrix4 m)
{
    float x = out->x;
    float y = out->y;
    float z = out->z;

    out->x = x * m[0][0] + y * m[1][0] + z * m[2][0];
    out->y = x * m[0][1] + y * m[1][1] + z * m[2][1];
    out->z = x * m[0][2] + y * m[1][2] + z * m[2][2];
}

/* VECTOR3INT */
static inline bool vector3_int_compare(Vector3Int* a, Vector3Int* b) {
	return a->x == b->x && a->y == b->y && a->z == b->z;
}

/* VECTOR4 */
static inline float vector4_length(Vector4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
static inline float vector4_distance(Vector4 a, Vector4 b) {
    Vector4 v = { b.x - a.x, b.y - a.y, b.z - a.z, b.w - a.w };
    return vector4_length(v);
}
static inline Vector4 vector4_get_normalized(Vector4 v) {
    float length = vector4_length(v);
    if (length == 0.0f) return (Vector4){0.0f, 0.0f, 0.0f, 0.0f};
    return (Vector4){v.x / length, v.y / length, v.z / length, v.w / length};
}
static inline Vector4 vector4_add(Vector4 a, Vector4 b) {
    return (Vector4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}
static inline Vector4 vector4_subtract(Vector4 a, Vector4 b) {
    return (Vector4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}
static inline Vector4 vector4_multiply(Vector4 a, Vector4 b) {
    return (Vector4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}
static inline Vector4 vector4_multiply_scalar(Vector4 a, const float scalar) {
    return (Vector4){a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar};
}
static inline float vector4_dot(Vector4 a, Vector4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
static inline Vector4 vector4_scale(Vector4 v, float scalar) {
    return (Vector4){v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar};
}
static inline Vector4 vector4_minus(Vector4 v) {
    return (Vector4){-v.x, -v.y, -v.z, -v.w};
}
static inline Vector4 vector4_cross(Vector4 a, Vector4 b) {
    return (Vector4){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
        .w = 0.0f
    };
}
static inline Vector4 vector4_rotate(const Vector4 v, const Vector4 axis, float angle) {
    float sinAngle = sinf(-angle);
    float cosAngle = cosf(-angle);
    
    Vector4 axis_sin = vector4_multiply_scalar(axis, sinAngle);
    Vector4 axis_cos = vector4_multiply_scalar(axis, 1.0f - cosAngle);
    
    Vector4 term1 = vector4_cross(v, axis_sin);
    Vector4 term2 = vector4_multiply_scalar(v, cosAngle);
    Vector4 term3 = vector4_multiply_scalar(axis, vector4_dot(v, axis_cos));
    
    Vector4 tmp = vector4_add(term2, term3);
    Vector4 result = vector4_add(term1, tmp);
    result.w = v.w;
    return result;
}
static inline Vector4 vector4_lerp(Vector4 a, Vector4 b, float v) {
    Vector4 lerped = vector4_subtract(b, a);
    lerped = vector4_multiply_scalar(lerped, v);
    return vector4_add(lerped, a);
}
static inline void vector4_transform(Vector4* v, const Matrix4 m)
{
    float x = v->x, y = v->y, z = v->z, w = v->w;

    v->x = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3]*w;
    v->y = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3]*w;
    v->z = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3]*w;
    v->w = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3]*w;
}
static inline Vector4 vector4_get_transformed(Vector4 v, const Matrix4 m) {
    return (Vector4) {
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
        m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
    };
}
static inline void vector4_perspective_divide(Vector4* v) {
    v->x /= v->w;
    v->y /= v->w;
    v->z /= v->w;
}
void matrix4_fast_inverse_transpose_3x3(Matrix4 m) {
    float a = m[0][0], b = m[1][0], c = m[2][0];
    float d = m[0][1], e = m[1][1], f = m[2][1];
    float g = m[0][2], h = m[1][2], i = m[2][2];

    m[0][0] = a; m[0][1] = d; m[0][2] = g;
    m[1][0] = b; m[1][1] = e; m[1][2] = h;
    m[2][0] = c; m[2][1] = f; m[2][2] = i;

    m[0][3] = m[1][3] = m[2][3] = 0.0f;
    m[3][0] = m[3][1] = m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

/* TRANSFORM2 */
static inline Transform2 transform2_create(float x, float y) {
    return (Transform2) {
        .x = {1.f, 0.f},
        .y = {0.f, 1.f},
        .origin = {x, y}
    };
}
static inline Transform2 transform2_duplicate(const Transform2* m) {
    Transform2 new_transform = *m;
    return new_transform;
}
Transform2 transform2_multiply(const Transform2* m, const Transform2* parent) {
    Transform2 new_transform = {0};
    new_transform.x[0] = parent->x[0] * m->x[0] + parent->y[0] * m->x[1];
    new_transform.x[1] = parent->x[1] * m->x[0] + parent->y[1] * m->x[1];
    new_transform.y[0] = parent->x[0] * m->y[0] + parent->y[0] * m->y[1];
    new_transform.y[1] = parent->x[1] * m->y[0] + parent->y[1] * m->y[1];
    new_transform.origin[0] = parent->x[0] * m->origin[0] + parent->y[0] * m->origin[1] + parent->origin[0];
    new_transform.origin[1] = parent->x[1] * m->origin[0] + parent->y[1] * m->origin[1] + parent->origin[1];
    return new_transform;
}
static inline float transform2_get_scale_x(const Transform2* m) {
    return sqrtf(m->x[0] * m->x[0] + m->y[0] * m->y[0]);
}
static inline float transform2_get_scale_y(const Transform2* m) {
    return sqrtf(m->x[1] * m->x[1] + m->y[1] * m->y[1]);
}
static inline void transform2_set_scale_x(Transform2* m, float value) {
    float current_scale = transform2_get_scale_x(m);
    m->x[0] = (m->x[0] / current_scale) * value;
    m->y[0] = (m->y[0] / current_scale) * value;
}
static inline void transform2_set_scale_y(Transform2* m, float value) {
    float current_scale = transform2_get_scale_y(m);
    m->x[1] = (m->x[1] / current_scale) * value;
    m->y[1] = (m->y[1] / current_scale) * value;
}
static inline float transform2_get_rotation(const Transform2* m) {
    return atan2f(m->x[1], m->x[0]);
}
static inline void transform2_set_rotation(Transform2* m, float radians) {
    float scale_x = transform2_get_scale_x(m);
    float scale_y = transform2_get_scale_y(m);
    float cos_r = cosf(radians);
    float sin_r = sinf(radians);
    m->x[0] = cos_r * scale_x;
    m->y[0] = -sin_r * scale_x;
    m->x[1] = sin_r * scale_y;
    m->y[1] = cos_r * scale_y;
}
static inline void transform2_set_x(Transform2* m, float value) {
    m->origin[0] = value;
}
static inline void transform2_set_y(Transform2* m, float value) {
    m->origin[1] = value;
}
static inline float transform2_get_x(const Transform2* m) {
    return m->origin[0];
}
static inline float transform2_get_y(const Transform2* m) {
    return m->origin[1];
}

/* MATRIX4 */
#define MATRIX4_SET(m, x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4, w1, w2, w3, w4) \
    do { \
        m[0][0] = (x1); m[0][1] = (x2); m[0][2] = (x3); m[0][3] = (x4); \
        m[1][0] = (y1); m[1][1] = (y2); m[1][2] = (y3); m[1][3] = (y4); \
        m[2][0] = (z1); m[2][1] = (z2); m[2][2] = (z3); m[2][3] = (z4); \
        m[3][0] = (w1); m[3][1] = (w2); m[3][2] = (w3); m[3][3] = (w4); \
    } while (0)

#define MATRIX4_COPY(to, from) \
    do { \
        for (int i = 0; i < 4; ++i) { \
            for (int j = 0; j < 4; ++j) { \
                to[i][j] = from[i][j]; \
            } \
        } \
    } while (0);

static inline void matrix4_identity(Matrix4 m) {
    MATRIX4_SET(m,
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}
static inline void matrix4_set_screen_space_transform(Matrix4 m, float half_width, float half_height) {
    MATRIX4_SET(m,
        half_width, 0, 0, half_width,
        0, -half_height, 0, half_height,
        0, 0, 1, 0, 
        0, 0, 0, 1);
}
static inline void matrix4_set_translation(Matrix4 m, float x, float y, float z) {
    MATRIX4_SET(m,
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1);
}
void matrix4_mul(Matrix4 m1, Matrix4 m2) {
    Matrix4 out;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out[i][j] = m1[i][0] * m2[0][j] +
                        m1[i][1] * m2[1][j] +
                        m1[i][2] * m2[2][j] +
                        m1[i][3] * m2[3][j];
        }
    }
    MATRIX4_COPY(m1, out);
}
void matrix4_set_axis_angle(Matrix4 m, float x, float y, float z, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    MATRIX4_SET(m,
        c+x*x*(1-c), x*y*(1-c)-z*s, x*z*(1-c)+y*s, 0,
        y*x*(1-c)+z*s, c+y*y*(1-c) ,y*z*(1-c)-x*s, 0,
        z*x*(1-c)-y*s, z*y*(1-c)+x*s, c+z*z*(1-c), 0,
        0, 0, 0, 1);
}
void matrix4_set_euler(Matrix4 m, float x, float y, float z) {
    Matrix4 rx, ry, rz;
    float sinx = sinf(x);
    float cosx = cosf(x);
    float siny = sinf(y);
    float cosy = cosf(y);
    float sinz = sinf(z);
    float cosz = cosf(z);
    MATRIX4_SET(rz,
        cosz, -sinz, 0, 0,
        sinz, cosz, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    MATRIX4_SET(rx,
        1, 0, 0, 0,
        0, cosx, -sinx, 0,
        0, sinx, cosx, 0,
        0, 0, 0, 1);
    MATRIX4_SET(ry,
        cosy, 0, -siny, 0,
        0, 1, 0, 0,
        siny, 0, cosy, 0,
        0, 0, 0, 1);
    matrix4_mul(ry, rx);
    matrix4_mul(rz, ry);
    MATRIX4_COPY(m, rz);
}
static inline void matrix4_set_rotation_vectors(Matrix4 m, Vector4 forward, Vector4 up, Vector4 right) {
    MATRIX4_SET(m,
        right.x, right.y, right.z, 0,
        up.x, up.y, up.z, 0,
        forward.x, forward.y, forward.z, 0,
        0, 0, 0, 1);
}
void matrix4_set_rotation_forward_up(Matrix4 m, Vector4 forward, Vector4 up) {
    Vector4 f = vector4_get_normalized(forward);
    Vector4 r = vector4_get_normalized(up);
    r = vector4_cross(r, f);
    Vector4 u = vector4_cross(f, r);
    matrix4_set_rotation_vectors(m, f, u, r);
}
static inline void matrix4_set_scale(Matrix4 m, float x, float y, float z) {
    MATRIX4_SET(m,
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
}
void matrix4_set_perspective(Matrix4 m, float fov, float aspect_ratio, float z_near, float z_far) {
    float tan_half_fov = tanf(fov / 2.0f);
    float z_range = z_far - z_near;
    float m22 = -(z_far + z_near) / z_range;
    float m23 = -(2.0f * z_far * z_near) / z_range;
    MATRIX4_SET(m,
            1.0f / (tan_half_fov * aspect_ratio), 0, 0, 0,
            0, 1.0f / tan_half_fov, 0, 0,
            0, 0, m22, m23,
            0, 0, -1, 0);
}
void matrix4_set_orthographic(Matrix4 m, float left, float right, float bottom, float top, float near, float far) {
    float width = right - left;
    float height = top - bottom;
    float depth = far - near;
    MATRIX4_SET(m,
        2.f / width, 0, 0, -(right + left) / width,
        0, 2.f / height, 0, -(top + bottom) / height,
        0, 0, -2.f / depth, -(far + near) / depth,
        0, 0, 0, 1);
}
void matrix4_look_at(Matrix4 m, Vector3 eye, Vector3 target, Vector3 up)
{
    Vector3 f = {
        target.x - eye.x,
        target.y - eye.y,
        target.z - eye.z
    };
    vector3_normalize(&f);

    Vector3 r = vector3_cross(f, up);
    vector3_normalize(&r);
    Vector3 u = vector3_cross(r, f);

    MATRIX4_SET(m,
        r.x,    u.x,    -f.x,   -vector3_dot(r, eye),
        r.y,    u.y,    -f.y,   -vector3_dot(u, eye),
        r.z,    u.z,    -f.z,   vector3_dot(f, eye),
        0,      0,      0,      1
    );
}

/* QUATERNION */
static inline Quaternion quaternion_create(Vector4 axis, float angle) {
    float s = sinf(angle * 0.5f);
    float c = cosf(angle * 0.5f);
    Quaternion q = { axis.x * s, axis.y * s, axis.z * s, c };
    return q;
}
static inline float quaternion_length(Quaternion q) {
    return sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
static inline Quaternion quaternion_get_normalized(Quaternion q) {
    float l = quaternion_length(q);
    Quaternion r = { q.x/l, q.y/l, q.z/l, q.w/l };
    return r;
}
static inline void quaternion_normalize(Quaternion* q) {
    float l = quaternion_length(*q);
    q->x/=l; q->y/=l; q->z/=l; q->w/=l;
}
static inline Quaternion quaternion_get_conjugated(Quaternion q) {
    Quaternion r = { -q.x, -q.y, -q.z, q.w };
    return r;
}
static inline void quaternion_conjugate(Quaternion* q) {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
}
static inline Quaternion quaternion_get_multiplied(Quaternion a, Quaternion b) {
    float w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
    float x = a.x*b.w + a.w*b.x + a.y*b.z - a.z*b.y;
    float y = a.y*b.w + a.w*b.y + a.z*b.x - a.x*b.z;
    float z = a.z*b.w + a.w*b.z + a.x*b.y - a.y*b.x;
    Quaternion r = { x,y,z,w };
    return r;
}
static inline void quaternion_multiply(Quaternion* a, const Quaternion b) {
    Quaternion r = quaternion_get_multiplied(*a, b);
    *a = r;
}
static inline Quaternion quaternion_get_multiplied_scalar(Quaternion q, float s) {
    Quaternion r = { q.x*s, q.y*s, q.z*s, q.w*s };
    return r;
}
static inline void quaternion_multiply_scalar(Quaternion* q, float s) {
    q->x *= s;
    q->y *= s;
    q->z *= s;
    q->w *= s;
}
static inline Quaternion quaternion_get_multiplied_vector4(const Quaternion q, Vector4 v) {
    float w = -q.x*v.x - q.y*v.y - q.z*v.z;
    float x =  q.w*v.x + q.y*v.z - q.z*v.y;
    float y =  q.w*v.y + q.z*v.x - q.x*v.z;
    float z =  q.w*v.z + q.x*v.y - q.y*v.x;
    Quaternion r = { x,y,z,w };
    return r;
}
static inline Quaternion quaternion_get_add(Quaternion a, Quaternion b) {
    Quaternion r = { a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w };
    return r;
}
static inline void quaternion_add(Quaternion* a, const Quaternion* b) {
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
    a->w += b->w;
}
static inline Quaternion quaternion_get_sub(Quaternion a, Quaternion b) {
    Quaternion r = { a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w };
    return r;
}
static inline void quaternion_sub(Quaternion* a, const Quaternion* b) {
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
    a->w -= b->w;
}
static inline float quaternion_dot(Quaternion a, Quaternion b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
static inline Quaternion quaternion_nlerp(const Quaternion* a, const Quaternion* dest, float t, int shortest) {
    Quaternion d = *dest;
    if(shortest && quaternion_dot(*a, *dest) < 0) {
        d.x = -d.x; d.y = -d.y; d.z = -d.z; d.w = -d.w;
    }
    Quaternion r = quaternion_get_sub(d, *a);
    r = quaternion_get_multiplied_scalar(r, t);
    r = quaternion_get_add(r, *a);
    return quaternion_get_normalized(r);
}
Quaternion quaternion_slerp(const Quaternion* a, const Quaternion* dest, float t, int shortest) {
    float cos = quaternion_dot(*a, *dest);
    Quaternion d = *dest;
    if(shortest && cos < 0) {
        cos = -cos;
        d.x = -d.x; d.y = -d.y; d.z = -d.z; d.w = -d.w;
    }
    if(fabsf(cos) >= 1 - 1e-3f)
        return quaternion_nlerp(a, &d, t, 0);
    float sinv = sqrtf(1 - cos*cos);
    float angle = atan2f(sinv, cos);
    float invSin = 1.0f / sinv;
    float fs = sinf((1 - t) * angle) * invSin;
    float fd = sinf(t * angle) * invSin;
    Quaternion r1 = quaternion_get_multiplied_scalar(*a, fs);
    Quaternion r2 = quaternion_get_multiplied_scalar(d, fd);
    Quaternion out = quaternion_get_add(r1, r2);
    return out;
}
Quaternion quaternion_from_matrix(const Matrix4 m) {
    float t = m[0][0] + m[1][1] + m[2][2];
    float x,y,z,w;
    if(t > 0) {
        float s = 0.5f / sqrtf(t + 1.0f);
        w = 0.25f / s;
        x = (m[1][2] - m[2][1]) * s;
        y = (m[2][0] - m[0][2]) * s;
        z = (m[0][1] - m[1][0]) * s;
    } else if(m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
        float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
        w = (m[1][2] - m[2][1]) / s;
        x = 0.25f * s;
        y = (m[1][0] + m[0][1]) / s;
        z = (m[2][0] + m[0][2]) / s;
    } else if(m[1][1] > m[2][2]) {
        float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
        w = (m[2][0] - m[0][2]) / s;
        x = (m[1][0] + m[0][1]) / s;
        y = 0.25f * s;
        z = (m[2][1] + m[1][2]) / s;
    } else {
        float s = 2.0f * sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]);
        w = (m[0][1] - m[1][0]) / s;
        x = (m[2][0] + m[0][2]) / s;
        y = (m[1][2] + m[2][1]) / s;
        z = 0.25f * s;
    }
    float l = sqrtf(x*x + y*y + z*z + w*w);
    Quaternion q = { x/l, y/l, z/l, w/l };
    return q;
}
Vector4 quaternion_rotate(const Quaternion* q, Vector4 v) {
    Quaternion p = { v.x, v.y, v.z, 0 };
    Quaternion qc = quaternion_get_conjugated(*q);
    Quaternion t = quaternion_get_multiplied(*q, p);
    t = quaternion_get_multiplied(t, qc);
    Vector4 r = { t.x, t.y, t.z, v.w };
    return r;
}
Vector4 quaternion_forward(const Quaternion* q) {
    Vector4 v = {0,0,1,1};
    return quaternion_rotate(q, v);
}
Vector4 quaternion_back(const Quaternion* q) {
    Vector4 v = {0,0,-1,1};
    return quaternion_rotate(q, v);
}
Vector4 quaternion_up(const Quaternion* q) {
    Vector4 v = {0,1,0,1};
    return quaternion_rotate(q, v);
}
Vector4 quaternion_down(const Quaternion* q) {
    Vector4 v = {0,-1,0,1};
    return quaternion_rotate(q, v);
}
Vector4 quaternion_right(const Quaternion* q) {
    Vector4 v = {1,0,0,1};
    return quaternion_rotate(q, v);
}
Vector4 quaternion_left(const Quaternion* q) {
    Vector4 v = {-1,0,0,1};
    return quaternion_rotate(q, v);
}
static inline int quaternion_equals(const Quaternion* a, const Quaternion* b) {
    return a->x==b->x && a->y==b->y && a->z==b->z && a->w==b->w;
}

/* MODEL3D */
void model3d_free(Model3D* model) {
	if (model != NULL) {
		free(model->vertices);
		free(model->normals);
		free(model->uvs);
		free(model->tris);
		free(model->tri_uvs);
		free(model->tri_normals);

		model->vertices = NULL;
		model->normals = NULL;
		model->uvs = NULL;
		model->tris = NULL;
		model->tri_uvs = NULL;
		model->tri_normals = NULL;
	}
}
Model3D* model3d_create_quad(Vector2 size) {
    Model3D* m = (Model3D*)malloc(sizeof(Model3D));

    float hw = size.x * 0.5f;
    float hh = size.y * 0.5f;

    m->vertices = malloc(sizeof(Vector3) * 4);
    m->normals = malloc(sizeof(Vector3) * 4);
    m->uvs = malloc(sizeof(Vector2) * 4);
    m->tris = malloc(sizeof(Vector3Int) * 2);
    m->tri_uvs = malloc(sizeof(Vector3Int) * 2);
    m->tri_normals = malloc(sizeof(Vector3Int) * 2);
    m->tris_count = 2;

    m->vertices[0] = (Vector3){ -hw, -hh, 0 };
    m->vertices[1] = (Vector3){  hw, -hh, 0 };
    m->vertices[2] = (Vector3){  hw,  hh, 0 };
    m->vertices[3] = (Vector3){ -hw,  hh, 0 };

    m->uvs[0] = (Vector2){0,1};
    m->uvs[1] = (Vector2){1,1};
    m->uvs[2] = (Vector2){1,0};
    m->uvs[3] = (Vector2){0,0};

    for (int i = 0; i < 4; i++) {
        m->normals[i] = (Vector3){0,0,-1};
	}

    m->tris[0] = (Vector3Int){0, 1, 2};
    m->tris[1] = (Vector3Int){0, 2, 3};

    m->tri_uvs[0] = (Vector3Int){0,1,2};
    m->tri_uvs[1] = (Vector3Int){0,2,3};

    m->tri_normals[0] = (Vector3Int){0,1,2};
    m->tri_normals[1] = (Vector3Int){0,2,3};

    m->texture = NULL;

    return m;
}

/* CAMERA3D */
static inline void camera3d_set_perspective(Matrix4 m, float fov, float aspect_ratio, float z_near, float z_far) {
    matrix4_set_perspective(m, fov, aspect_ratio, z_near, z_far);
}
static inline void camera3d_update(Camera3D* cam) {
    matrix4_look_at(cam->view, cam->position, cam->target, cam->up);
}
#endif // CHAO_TYPES_H
