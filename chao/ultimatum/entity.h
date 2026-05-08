#ifndef ULTIMATUM_ENTITY_H
#define ULTIMATUM_ENTITY_H

#include <string.h>
#include "../chao/chao.h"
#include "generated_map_data.h"
#include "d20.h"

enum {
    U2_MAX_ENTITIES = 64,
};

typedef enum {
    U2_ENTITY_KIND_NONE = 0,
    U2_ENTITY_KIND_PLAYER,
    U2_ENTITY_KIND_NPC,
    U2_ENTITY_KIND_MONSTER,
} U2EntityKind;

typedef enum {
    U2_FACING_SOUTH = 0,
    U2_FACING_WEST,
    U2_FACING_NORTH,
    U2_FACING_EAST,
} U2Facing;

typedef enum {
    U2_INTERACTION_NONE = 0,
    U2_INTERACTION_TALK,
    U2_INTERACTION_SIGN,
} U2InteractionKind;

typedef struct {
    bool active;
    const char* id;
    const char* name;
    U2EntityKind kind;
    const U2TileDef* tile;
    int tile_x;
    int tile_y;
    int spawn_tile_x;
    int spawn_tile_y;
    bool solid;
    bool hostile;
    bool roaming;
    int roam_radius;
    U2Facing facing;
    U2InteractionKind interaction_kind;
    const char* dialogue_id;
    Character sheet;
} U2Entity;

static inline int u2_entity_wrap_coord(int value, int size) {
    int wrapped = value % size;
    return wrapped < 0 ? wrapped + size : wrapped;
}

static const U2TileDef* u2_entity_find_tile_by_name(const char* tile_name) {
    if (tile_name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_tile_defs_count; ++i) {
        if (strcmp(u2_tile_defs[i].name, tile_name) == 0) {
            return &u2_tile_defs[i];
        }
    }

    return NULL;
}

static void u2_entity_reset(U2Entity* entity) {
    memset(entity, 0, sizeof(*entity));
}

static bool u2_entity_is_valid(const U2Entity* entity) {
    return entity != NULL && entity->active && entity->tile != NULL;
}

static void u2_entity_init(U2Entity* entity, const char* id, const char* name, U2EntityKind kind, const char* tile_name, int tile_x, int tile_y) {
    u2_entity_reset(entity);
    entity->active = true;
    entity->id = id;
    entity->name = name;
    entity->kind = kind;
    entity->tile = u2_entity_find_tile_by_name(tile_name);
    entity->tile_x = tile_x;
    entity->tile_y = tile_y;
    entity->spawn_tile_x = tile_x;
    entity->spawn_tile_y = tile_y;
    entity->solid = false;
    entity->hostile = false;
    entity->roaming = false;
    entity->roam_radius = 0;
    entity->facing = U2_FACING_SOUTH;
    entity->interaction_kind = U2_INTERACTION_NONE;
    entity->dialogue_id = NULL;
}

static void u2_entity_set_character_sheet(
    U2Entity* entity,
    CharacterClass class,
    int base_hp,
    int base_mp,
    const char* base_damage,
    int armor_class,
    int strength,
    int dexterity,
    int mind,
    int physical,
    int subterfuge,
    int knowledge,
    int communication
) {
    memset(&entity->sheet, 0, sizeof(entity->sheet));

    entity->sheet.class = class;
    entity->sheet.base_hp = base_hp;
    entity->sheet.base_mp = base_mp;
    entity->sheet.base_damage = (char*)base_damage;
    entity->sheet.armor_class = armor_class;
    entity->sheet.stats[STAT_STR] = strength;
    entity->sheet.stats[STAT_DEX] = dexterity;
    entity->sheet.stats[STAT_MIND] = mind;
    entity->sheet.skills[SKILL_PHYSICAL] = physical;
    entity->sheet.skills[SKILL_SUBTERFUGE] = subterfuge;
    entity->sheet.skills[SKILL_KNOWLEDGE] = knowledge;
    entity->sheet.skills[SKILL_COMMUNICATION] = communication;

    d20_character_init(&entity->sheet);
    entity->sheet.level = 1;
    entity->sheet.hp = d20_get_max_hp(&entity->sheet);
    entity->sheet.mp = d20_get_max_mp(&entity->sheet);
}

static void u2_entity_clamp_to_map(U2Entity* entity, const U2MapDef* map) {
    if (!u2_entity_is_valid(entity) || map == NULL) {
        return;
    }

    if (map->looped) {
        entity->tile_x = u2_entity_wrap_coord(entity->tile_x, map->width);
        entity->tile_y = u2_entity_wrap_coord(entity->tile_y, map->height);
    } else {
        entity->tile_x = clamp(entity->tile_x, 0, map->width - 1);
        entity->tile_y = clamp(entity->tile_y, 0, map->height - 1);
    }
}

static void u2_entity_move_by(U2Entity* entity, const U2MapDef* map, int dx, int dy) {
    if (!u2_entity_is_valid(entity)) {
        return;
    }

    if (dx < 0) entity->facing = U2_FACING_WEST;
    if (dx > 0) entity->facing = U2_FACING_EAST;
    if (dy < 0) entity->facing = U2_FACING_NORTH;
    if (dy > 0) entity->facing = U2_FACING_SOUTH;

    entity->tile_x += dx;
    entity->tile_y += dy;
    u2_entity_clamp_to_map(entity, map);
}

#endif // ULTIMATUM_ENTITY_H
