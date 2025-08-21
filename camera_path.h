#ifndef CAMERA_PATH_H
#define CAMERA_PATH_H

#include <math.h>

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float w, x, y, z;
} Quat;

typedef struct {
    Vec3 position;
    Quat rotation;
} CameraKeyframe;

// Simple helper: make vec3
static inline Vec3 make_vec3(float x, float y, float z) {
    Vec3 v = {x,y,z};
    return v;
}

// Simple helper: make quat
static inline Quat make_quat(float w, float x, float y, float z) {
    Quat q = {w,x,y,z};
    return q;
}

// Normalize quaternion
static inline Quat quat_normalize(Quat q) {
    float mag = sqrtf(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
    Quat res = {q.w/mag, q.x/mag, q.y/mag, q.z/mag};
    return res;
}

// Slerp interpolation
static inline Quat quat_slerp(Quat a, Quat b, float t) {
    float dot = a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
    if (dot < 0.0f) {
        b.w = -b.w; b.x = -b.x; b.y = -b.y; b.z = -b.z;
        dot = -dot;
    }
    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        Quat result = {
            a.w + t*(b.w - a.w),
            a.x + t*(b.x - a.x),
            a.y + t*(b.y - a.y),
            a.z + t*(b.z - a.z)
        };
        return quat_normalize(result);
    }
    float theta_0 = acosf(dot);
    float theta = theta_0 * t;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);
    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    Quat result = {
        (a.w * s0) + (b.w * s1),
        (a.x * s0) + (b.x * s1),
        (a.y * s0) + (b.y * s1),
        (a.z * s0) + (b.z * s1)
    };
    return result;
}

// Lerp interpolation
static inline Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    Vec3 v = {
        a.x + (b.x - a.x)*t,
        a.y + (b.y - a.y)*t,
        a.z + (b.z - a.z)*t
    };
    return v;
}

// Rotate a forward vector (0,0,-1) by quaternion
static inline Vec3 quat_forward(Quat q) {
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    Vec3 f = {
        2*(xz + wy),
        2*(yz - wx),
        1 - 2*(xx + yy)
    };
    return f;
}

// Example path with 3 keyframes
static CameraKeyframe camera_path[] = {
    { {0,0,0}, {1,0,0,0} },
    { {5,0,0}, {0.9239f, 0, 0.3827f, 0} }, // 45° rotation around Y
    { {5,5,0}, {0.7071f, 0, 0.7071f, 0} }  // 90° rotation around Y
};
static int camera_path_length = 3;

#endif
