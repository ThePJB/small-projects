#include "mymath.h"
#include <math.h>

float fastfloor(float x) {
    if (x > 0) {
        return (int)x;
    } else if (x < 0) {
        return (int)(x-1);
    } else {
        return 0;
    }
}

float fast_abs(float x) {
    return x > 0 ? x : -x;
}

float frac(float x) {
    return x - fastfloor(x);
}

float lerp(float a, float b, float t) {
    return t*b + (1-t)*a;
}
float bilinear(float a, float b, float c, float d, float t1, float t2) {
    return lerp(lerp(a,b,t1),lerp(c,d,t1),t2);
}

float bilinear_dydx(float a, float b, float c, float d, float t1, float t2) {
    return lerp((a-b), (c-d), t2);
}
float bilinear_dydz(float a, float b, float c, float d, float t1, float t2) {
    return lerp((a-c), (b-d), t1);
}
vec3s bilinear_normal(float a, float b, float c, float d, float t1, float t2) {
    float dydx = bilinear_dydx(a, b, c, d, t1, t2);
    float dydz = bilinear_dydz(a, b, c, d, t1, t2);

    vec3s tangent_x = {
        1,
        dydx,
        0
    };

    vec3s tangent_z = {
        0,
        dydz,
        1
    };

    return glms_vec3_cross(tangent_x, tangent_z);
}


float trilinear(float a, float b, float c, float d, float e, float f, float g, float h, float x, float y, float z) {
    return lerp(bilinear(a,b,c,d,x,y), bilinear(e, f, g, h, x, y), z);
}

float u3(float x) {
    return x*x*(3-2*x);
}
float u3d(float x) {
    return 6*x*(1-x);
}
float interp3(float a, float b, float t) {
    return lerp(a,b,u3(t));
}

float u5(float x) {
    return x*x*x*(10-3*x*(5-2*x));
}
float interp5(float a, float b, float t) {
    return lerp(a,b,u5(t));
}
float bilinear3(float a, float b, float c, float d, float t1, float t2) {
    return interp3(interp3(a,b,t1),interp3(c,d,t1),t2);
}

float trilinear3(float a, float b, float c, float d, float e, float f, float g, float h, float x, float y, float z) {
    return interp3(bilinear3(a,b,c,d,x,y), bilinear3(e, f, g, h, x, y), z);
}

vec3s normal_from_verts(vec3s a, vec3s b, vec3s c) {
    vec3s ab = glms_vec3_sub(b,a);
    vec3s bc = glms_vec3_sub(c,a);
    return glms_vec3_normalize(glms_vec3_cross(bc, ab));
}

float ellipse_radius(float a, float b, float theta) {
    float st = sinf(theta);
    float ct = cosf(theta);
    return a*b / sqrtf(a*a*st*st+b*b*ct*ct);
}