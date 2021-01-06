#include "proc_tree.h"
#include "util.h"

tree_parameters tree_start(vec3s base_pos, int trunk_sides, vec3s foliage_colour, vec3s trunk_colour) {
    const int array_initial_size = 5;
    tree_parameters tp = {
        .base_pos = base_pos,
        .trunk_sides = trunk_sides,
        .foliage = malloc(sizeof(foliage) * array_initial_size),
        .trunk = malloc(sizeof(trunk_segment) * array_initial_size),
        .n_foliage = 0,
        .foliage_size = array_initial_size,
        .n_trunk_segments = 0,
        .trunk_segments_size = array_initial_size,
        .foliage_colour = foliage_colour,
        .trunk_colour = trunk_colour,
    };
    return tp;
}

void tree_push_trunk_segment(tree_parameters *tp, trunk_segment ts) {
    if (tp->n_trunk_segments >= tp->trunk_segments_size) {
        tp->trunk_segments_size *= 2;
        tp->trunk = realloc(tp->trunk, sizeof(trunk_segment) * tp->trunk_segments_size);
    }
    tp->trunk[tp->n_trunk_segments++] = ts;
}

void tree_push_foliage(tree_parameters *tp, foliage fp) {
    if (tp->n_foliage >= tp->foliage_size) {
        tp->foliage_size *= 2;
        tp->foliage = realloc(tp->foliage, sizeof(foliage) * tp->foliage_size);
    }
    tp->foliage[tp->n_foliage++] = fp;
}

// todo use tree start
// or remove it and use a separate thing that just acts on meshes
void tree_push_to_mesh(PNC_Mesh *m, tree_parameters tp) {

    // Trunk
    // todo add cone case
    for (int i = 0; i < tp.n_trunk_segments; i++) {
        pnc_push_trunc_cone(m, tp.trunk_colour, 
            tp.trunk[i].top.position, tp.trunk[i].top.axis,
            tp.trunk[i].bot.position, tp.trunk[i].bot.axis,
            tp.trunk[i].top.radius, tp.trunk[i].bot.radius,
            tp.trunk_sides
        );
    }


    // Foliage
    if (tp.foliage_type == FT_CONE) {
        for (int i = 0; i < tp.n_foliage; i++) {
            // todo noise_cone
            // todo phase
            pnc_push_cone(m, tp.foliage_colour,
                tp.foliage[i].pos_top, tp.foliage[i].pos_base, 
                glms_vec3_normalize(glms_vec3_sub(tp.foliage[i].pos_top, tp.foliage[i].pos_base)),
                tp.foliage[i].radius, tp.foliage[i].sides
            );
        }
    } else {
        for (int i = 0; i < tp.n_foliage; i++) {
            vec3s center = glms_vec3_lerp(tp.foliage[i].pos_top, tp.foliage[i].pos_base, 0.5);
            vec3s axis = glms_vec3_sub(tp.foliage[i].pos_top, tp.foliage[i].pos_base);
            float d = tp.foliage[i].radius;
            float h = d;
            float circ_poly = tp.foliage[i].sides;


            pnc_push_ellipsoid(m, tp.foliage_colour, center, axis, d, h, circ_poly, circ_poly);
        }
    }
    
}
/*

PNC_Mesh example_pine_tree() {

    PNC_Mesh m = pnc_new();
    tree_parameters tp = tree_start(
        (vec3s) {0,0,0},
        8,
        (vec3s) {0.1,  0.6, 0.05},
        (vec3s) {0.4,  0.4, 0}
    );

    tree_push_trunk_segment(&tp, (trunk_cross_section) {
        .position = (vec3s) {0, 0, 0},
        .axis = (vec3s) {0, 1, 0},
        .radius = 0.2
    });

    tree_push_trunk_segment(&tp, (trunk_cross_section) {
        .position = (vec3s) {0, 0.2, 0},
        .axis = (vec3s) {0, 1, 0},
        .radius = 0.1
    });

    tree_push_trunk_segment(&tp, (trunk_cross_section) {
        .position = (vec3s) {0, 1.0, 0},
        .axis = (vec3s) {0, 1, 0},
        .radius = 0
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0, 0.7, 0},
        .pos_base = (vec3s) {0, 0.4, 0},
        .sides = 7,
        .radius = 0.4
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0, 0.85, 0},
        .pos_base = (vec3s) {0, 0.6, 0},
        .radius = 0.25,
        .sides = 6,
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0, 1.0, 0},
        .pos_base = (vec3s) {0, 0.8, 0},
        .radius = 0.1,
        .sides = 5,
    });

    tree_push_to_mesh(&m, tp);

    pnc_upload(&m);

    return m;
}
*/

PNC_Mesh example_branch_tree() {
    
    PNC_Mesh m = pnc_new();
    
    tree_parameters tp = tree_start(
        (vec3s) {0,0,0},
        8,
        (vec3s) {0.1,  0.6, 0.05},
        (vec3s) {0.4,  0.4, 0}
    );

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .top = (trunk_cross_section) {
            .position = (vec3s) {0, 0.2, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.1
        },
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.2
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.2, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.1
        },
        .top = (trunk_cross_section) {
            .position = (vec3s) {0.1, 1.0, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.2, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.1
        },
        .top = (trunk_cross_section) {
            .position = (vec3s) {-0.3, 0.8, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0
        },
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0.1, 1.0, 0},
        .pos_base = (vec3s) {0.1, 0.8, 0},
        .radius = 0.3,
        .sides = 6,
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {-0.3, 0.8, 0},
        .pos_base = (vec3s) {-0.3, 0.6, 0},
        .radius = 0.2,
        .sides = 5,
    });

    tree_push_to_mesh(&m, tp);

    pnc_upload(&m);

    return m;
}

PNC_Mesh example_bottle_tree() {
    
    PNC_Mesh m = pnc_new();
    
    tree_parameters tp = tree_start(
        (vec3s) {0,0,0},
        8,
        (vec3s) {0.1,  0.6, 0.05},
        (vec3s) {0.4,  0.4, 0}
    );

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .top = (trunk_cross_section) {
            .position = (vec3s) {0, 0.3, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.3
        },
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.2
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.3, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.3
        },
        .top = (trunk_cross_section) {
            .position = (vec3s) {0, 0.6, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.2
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .top = (trunk_cross_section) {
            .position = (vec3s) {0, 0.8, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.1
        },
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.6, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.2
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.8, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.1
        },
        .top = (trunk_cross_section) {
            .position = (vec3s) {0, 1, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .top = (trunk_cross_section) {
            .position = (vec3s) {0.4, 0.7, 0},
            .axis = (vec3s) {1, 1, 0},
            .radius = 0.01
        },
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0, 0.6, 0},
            .axis = (vec3s) {0, 1, 0},
            .radius = 0.2
        },
    });

    tree_push_trunk_segment(&tp, (trunk_segment) {
        .bot = (trunk_cross_section) {
            .position = (vec3s) {0.4, 0.7, 0},
            .axis = (vec3s) {1, 1, 0},
            .radius = 0.01
        },
        .top = (trunk_cross_section) {
            .position = (vec3s) {0.6, 0.8, 0},
            .axis = (vec3s) {0.5, 1, 0},
            .radius = 0
        },
    });



    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0, 1.0, 0},
        .pos_base = (vec3s) {0, 0.8, 0},
        .radius = 0.3,
        .sides = 8,
    });

    tree_push_foliage(&tp, (foliage) {
        // nyi parameters
        .phase = 0,
        .noise_h = 0,
        .noise_r = 0,
        .noise_t = 0,

        .pos_top = (vec3s) {0.6, 0.8, 0},
        .pos_base = (vec3s) {0.6, 0.7, 0},
        .radius = 0.1,
        .sides = 7,
    });

    tree_push_to_mesh(&m, tp);

    pnc_upload(&m);

    return m;
}

// looks pretty good except sometimes the trunk gets twisted
// hard to say why that happens especially cause im not exactly sure why it doesn't twist most of the time
// lost limbs: did it by accident but its actually kind of legit (gum trees)
// tree grow?
// tree grow implicitly? age as a paramter and for different age it will be consistent
// trees seed
// implicit lods as well

void l_tree(int seed, tree_parameters *tp, trunk_cross_section tcs, float t) {
    const float branch_range = 0.6;
    const float t_increment = 0.15;
    //const float distance = 0.4;
    const float up_tendency = 0.1;
    const float bi_chance = 0.5;
    const float thickness = 0.2;

    float distance = 0.4 + 0.1*t;

    if (t < 0) {
        tree_push_foliage(tp, (foliage) {
            .sides = 6,
            .radius = 0.4,
            .pos_top = (vec3s) {
                .x = tcs.position.x,
                .y = tcs.position.y + 0.2,
                .z = tcs.position.z,
            },
            .pos_base = (vec3s) {
                .x = tcs.position.x,
                .y = tcs.position.y - 0.2,
                .z = tcs.position.z,
            }
        });
        return;
    };

    // always bifurcate
    vec3s new_axis = tcs.axis;
    new_axis.x += hash_floatn(seed+1234, -branch_range, branch_range);
    new_axis.z += hash_floatn(seed+5678, -branch_range, branch_range);
    new_axis = glms_vec3_normalize(new_axis);
    vec3s opposing_new_axis = glms_vec3_rotate(new_axis, M_PI, tcs.axis);
    
    new_axis = glms_vec3_lerp(new_axis, (vec3s){0,1,0}, up_tendency);
    opposing_new_axis = glms_vec3_lerp(opposing_new_axis, (vec3s){0,1,0}, up_tendency);

    // still normalized i think?

    float new_radius = thickness * t;
    vec3s new_pos = glms_vec3_add(tcs.position, glms_vec3_scale(new_axis, distance));
    vec3s new_opp_pos = glms_vec3_add(tcs.position, glms_vec3_scale(opposing_new_axis, distance));

    trunk_cross_section new_cs = (trunk_cross_section) {
        .position = new_pos,
        .radius = new_radius,
        .axis = new_axis,
    };

    trunk_cross_section new_opp_cs = (trunk_cross_section) {
        .position = new_opp_pos,
        .radius = new_radius,
        .axis = opposing_new_axis,
    };

    tree_push_trunk_segment(tp, (trunk_segment) {
        .bot = tcs,
        .top = new_cs
    });



    l_tree(hash(seed+98766), tp, new_cs, t - t_increment);
    if (hash_floatn(seed+986482, 0, 1) < bi_chance) {
        tree_push_trunk_segment(tp, (trunk_segment) {
            .bot = tcs,
            .top = new_opp_cs
        });
        l_tree(hash(seed+23489234), tp, new_opp_cs, t - t_increment);
    }
}

//todo branch stages
// no branch below t = whatever
// also bifurcations contrasted with just side branches
// eg bifurcations low but not side branches
// random tree different parameters for these things
// force branches to angle away?
// the real questions is what to do about leaves etc
// deferred rendering procedural leaves/bark texture?

// maybe my bark is just cause of how discontinuous the branches are

void tree_continue(tree_parameters *tp, trunk_cross_section tcs, float t) {
    //const float cull_branch_below = 0.005;
    const float branch_range = 0.7;
    const float t_trunk = 0.2;
    const float t_branch = 0.3;

    printf("tree t: %.3f\n", t);

    // param: branch size ratio
    if (t < 0) {
        printf("bad t < 0 %.2f\n", t);
    }

    //if (t < 1 && tcs.radius > cull_branch_below) {
    if (t < 1) {
        vec3s new_axis = glms_vec3_lerp(tcs.axis, (vec3s){0,1,0}, 0.05);
        float new_radius = 0.2 * (1 - t);
        //float new_length = 1.0 * (1 - t);
        float new_length = 0.6;
        printf(" radius: %.3f\n", new_radius);

        vec3s new_position = (vec3s) {
            .x = tcs.position.x + tcs.axis.x * new_length,
            .y = tcs.position.y + tcs.axis.y * new_length,
            .z = tcs.position.z + tcs.axis.z * new_length,
        };

        trunk_cross_section new_cs = (trunk_cross_section) {
            //.radius = tcs.radius * 0.9,
            .radius =new_radius,
            .position = new_position,
            .axis = new_axis,
        };
        tree_push_trunk_segment(tp, (trunk_segment) {
            .bot = tcs,
            .top = new_cs,
        });
        tree_continue(tp, new_cs, t + t_trunk);

        // Branching etc
        const float branch_chance = 0.4;
        const float bifurcate_chance = 0.2;

        float r = rand_floatn(0,1);

        if (r < branch_chance) {
            printf("branch from t: %.3f\n", t);
            vec3s new_axis = tcs.axis;
            new_axis.x += rand_floatn(-branch_range, branch_range);
            new_axis.z += rand_floatn(-branch_range, branch_range);
            new_axis = glms_vec3_normalize(new_axis);

            trunk_cross_section new_cs = (trunk_cross_section) {
            //.radius = tcs.radius * 0.5,
            .radius = new_radius,
            //.radius = 0.1, // right so this basically doesnt matter
            .position = new_position,
            .axis = new_axis,
        };
        tree_push_trunk_segment(tp, (trunk_segment) {
            .bot = tcs,
            .top = new_cs,
        });
        tree_continue(tp, new_cs, t + 0.5 * (1 - t)); // it hass to be this but it looks fine

        } else if (r - branch_chance < bifurcate_chance) {



        }
    } else {
        // top of tree
        vec3s new_pos = (vec3s) {
                .x = tcs.position.x + tcs.axis.x * 0.1,
                .y = tcs.position.y + tcs.axis.y * 0.1,
                .z = tcs.position.z + tcs.axis.z * 0.1,
            };
        trunk_cross_section new_cs = (trunk_cross_section) {
            .radius = 0,
            .position = new_pos,
            .axis = tcs.axis,
        };
        tree_push_foliage(tp, (foliage) {
            .sides = 6,
            .radius = 0.2,
            .pos_base = tcs.position,
            .pos_top = new_pos,
        });
    }
}


// todo something good
/*
so like if it just keeps track of the last point all we put is the next point


todo trunk method for leaves too to make fat bulbs

oo make a bottle tree

*/