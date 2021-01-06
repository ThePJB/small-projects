#include "camera3p.h"

mat4s cam3p_view_mat(camera_3rd_person c3p) {
    vec3s u = (vec3s) {0,0,1};
    
    u = glms_vec3_rotate(u, c3p.yaw, c3p.up);

    // pitch needs to rotate against right vector
    // right is forward x up
    vec3s forward = glms_vec3_sub(c3p.target, u);
    vec3s right = glms_vec3_cross(forward, c3p.up);

    u = glms_vec3_rotate(u, c3p.pitch, glms_vec3_normalize(right));
    
    
    vec3s pos = glms_vec3_sub(c3p.target, glms_vec3_scale(u, c3p.distance));
    //vec3s up = glms_vec3_rotate(c3p.up, c3p.pitch, (vec3s) {0,0,1});
    return glms_lookat(pos, c3p.target, glms_vec3_cross(u, right));
}

void cam3p_update_held(
        camera_3rd_person *cam,

        bool pitch_up,
        bool pitch_down,
        bool yaw_left,
        bool yaw_right,
        bool zoom_in,
        bool zoom_out,

        float rot_amount,
        float zoom_amount) {
    
    if (pitch_up) {
        cam->pitch += rot_amount;
    }
    if (pitch_down) {
        cam->pitch -= rot_amount;
    }
    if (yaw_left) {
        cam->yaw += rot_amount;
    }
    if (yaw_right) {
        cam->yaw -= rot_amount;
    }
    if (zoom_in) {
        cam->distance += zoom_amount;
    }
    if (zoom_out) {
        cam->distance -= zoom_amount;
    }
}