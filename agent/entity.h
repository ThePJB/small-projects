#pragma once

typedef enum {
    ENT_CITY,
    ENT_DUDE,
    NUM_ENTS,
} entity_type;

typedef struct {
    int x;
    int y;
    entity_type et;
    int team;

    float carried_food;
    float health;

    bool alive;
} entity;

