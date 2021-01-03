#pragma once

#include <cglm/struct.h>
#include "pnc.h"

typedef struct {
    vec3s position;
    vec3s axis;
    float radius;
} trunk_cross_section;

typedef struct {
    trunk_cross_section top;
    trunk_cross_section bot;
} trunk_segment;

typedef struct {
    vec3s pos_top;
    vec3s pos_base;
    float radius;
    float phase;
    float noise_r;
    float noise_t;
    float noise_h;
    int sides;
} foliage;

typedef enum {
    FT_CONE,
    FT_ELLIPSOID,
} foliage_type;

typedef struct {
    trunk_segment *trunk;
    int n_trunk_segments;
    int trunk_segments_size;

    foliage *foliage;
    int n_foliage;
    int foliage_size;

    int trunk_sides;

    vec3s foliage_colour;
    vec3s trunk_colour;

    vec3s base_pos;

    foliage_type foliage_type;
} tree_parameters;

tree_parameters tree_start(vec3s base_pos, int trunk_sides, vec3s foliage_colour, vec3s trunk_colour);

void tree_push_trunk_segment(tree_parameters *tp, trunk_segment ts);
void tree_push_foliage(tree_parameters *tp, foliage fp);
void tree_params_free(tree_parameters *tp);

void tree_push_to_mesh(PNC_Mesh *m, tree_parameters tp);

void tree_continue(tree_parameters *tp, trunk_cross_section tcs, float t);
void l_tree(tree_parameters *tp, trunk_cross_section tcs, float t);



PNC_Mesh example_pine_tree();
PNC_Mesh example_branch_tree();
PNC_Mesh example_bottle_tree();