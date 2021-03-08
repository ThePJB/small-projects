#include "coolmath.h"

// normal % is remainder not mod
int cm_mod(int x, int y) {
  //if (y == 0) exit(-1);
  //if (y == -1) return 0;
  int m = x % y;
  if (m < 0) {
    m = (y < 0) ? m - y : m + y;
  }
  return m;
}

float cm_floor(float x) {
    if (x > 0) {
        return (int)x;
    } else if (x < 0) {
        return (int)(x-1);
    } else {
        return 0;
    }
}

float cm_abs(float x) {
    return x > 0 ? x : -x;
}

float cm_frac(float x) {
    return x - cm_floor(x);
}

// good polynomials t = t*t(3-2*t)
// t = t*t*t(6*t*t - 15*t + 10)
float cm_lerp(float a, float b, float t) {
    return t*b + (1-t)*a;
}

float cm_unlerp(float a, float b, float t) {
    return (t - a) / (b - a);
}

float cm_bilinear(float a1, float a2,float b1, float b2, float tnum, float talpha) {
    return cm_lerp(cm_lerp(a1, a2, tnum), cm_lerp(b1, b2, tnum), talpha);
}

float cm_bilinear3(float a1, float a2,float b1, float b2, float tnum, float talpha) {
    return cm_bilinear(a1, a2, b1, b2, tnum*tnum*(3-2*tnum), talpha*talpha*(3-2*talpha));
}

float cm_remap(float prev_lower, float prev_upper, float new_lower, float new_upper, float a) {
    return cm_lerp(new_lower, new_upper, cm_unlerp(prev_lower, prev_upper, a));
}

float cm_slow_start(float t) {
    return t*t;
}

float cm_slow_stop(float t) {
    return 1 - cm_slow_start(1-t);
}

float cm_slow_start2(float t) {
    return cm_slow_start(cm_slow_start(t));
}

float cm_slow_stop2(float t) {
    return cm_slow_stop(cm_slow_stop(t));
}

float cm_slow_start_stop(float t) {
    return cm_lerp(
        cm_slow_start(t),
        cm_slow_stop(t),
        t
    );
}

float cm_slow_start_stop2(float t) {
    return cm_slow_start_stop(cm_slow_start_stop(t));
}