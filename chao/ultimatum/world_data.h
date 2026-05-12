#ifndef ULTIMATUM_WORLD_DATA_H
#define ULTIMATUM_WORLD_DATA_H

#include <string.h>
#include "entity.h"

enum {
    U2_MAX_STARTER_ITEMS = 4,
};

typedef struct {
    CharacterClass class_id;
    const char* name;
    const char* description;
    const char* player_tile_name;
    int base_hp;
    int base_mp;
    const char* base_damage;
    int armor_class;
    int strength;
    int dexterity;
    int mind;
    int physical;
    int subterfuge;
    int knowledge;
    int communication;
    int starter_gold;
    int starter_food;
    const char* starter_weapon_id;
    const char* starter_armor_id;
    const char* starter_offhand_id;
    const char* starter_item_ids[U2_MAX_STARTER_ITEMS];
    int starter_item_amounts[U2_MAX_STARTER_ITEMS];
} U2ClassTemplate;

typedef struct {
    const char* id;
    CharacterClass class_id;
    int base_hp;
    int base_mp;
    const char* base_damage;
    int armor_class;
    int strength;
    int dexterity;
    int mind;
    int physical;
    int subterfuge;
    int knowledge;
    int communication;
    int reward_gold;
    int reward_exp;
} U2MonsterTemplate;

typedef enum {
    U2_SPELL_DAMAGE_BOLT = 0,
    U2_SPELL_HEAL_SELF,
    U2_SPELL_BLINK_FORWARD,
} U2SpellEffect;

typedef struct {
    const char* id;
    CharacterClass class_id;
    const char* name;
    const char* description;
    int mp_cost;
    int range;
    U2SpellEffect effect;
    const char* power_dice;
} U2SpellDef;

typedef struct {
    const char* source_map_id;
    int source_x;
    int source_y;
    const char* destination_map_id;
    int destination_x;
    int destination_y;
} U2TransitionDef;

typedef struct {
    const char* id;
    const char* source_map_id;
    int source_x;
    int source_y;
    const char* destination_map_id;
    int destination_x;
    int destination_y;
    int schedule_offset;
} U2TimeGateDef;

typedef struct {
    const char* id;
    const char* surface_map_id;
    int surface_x;
    int surface_y;
    bool tower_mode;
    const char* name;
    int level_count;
    const char* theme_id;
    const char* guardian_monster_template_id;
    const char* guardian_name;
    int final_reward_gold;
    const char* final_bonus_item_id;
} U2DungeonPresetDef;

typedef struct {
    const char* map_id;
    const char* id;
    const char* name;
    U2EntityKind kind;
    CharacterClass class_id;
    const char* monster_template_id;
    const char* tile_name;
    int tile_x;
    int tile_y;
    bool solid;
    bool hostile;
    bool roaming;
    int roam_radius;
    U2InteractionKind interaction_kind;
    const char* dialogue_id;
} U2SpawnDef;

typedef struct {
    const char* id;
    const char* text;
} U2DialogueDef;

typedef enum {
    U2_SERVICE_NONE = 0,
    U2_SERVICE_SHOP,
    U2_SERVICE_INN,
    U2_SERVICE_HEALER,
    U2_SERVICE_SANCTUARY,
} U2ServiceKind;

typedef struct {
    const char* entity_id;
    U2ServiceKind kind;
    const char* title;
    const char* description;
    int primary_cost;
    int secondary_cost;
    int tertiary_cost;
} U2ServiceDef;

typedef struct {
    const char* service_entity_id;
    const char* label;
    const char* item_id;
    int price;
    int food_value;
} U2ShopStockDef;

static const U2ClassTemplate u2_class_templates[] = {
    {
        .class_id = CLASS_FIGHTER,
        .name = "Fighter",
        .description = "Front-line warrior with the best staying power.",
        .player_tile_name = "warrior",
        .base_hp = 14,
        .base_mp = 0,
        .base_damage = "1d8",
        .armor_class = 11,
        .strength = 15,
        .dexterity = 11,
        .mind = 8,
        .physical = 4,
        .subterfuge = 1,
        .knowledge = 0,
        .communication = 1,
        .starter_gold = 75,
        .starter_food = 120,
        .starter_weapon_id = "short_sword",
        .starter_armor_id = "leather_armor",
        .starter_offhand_id = "wooden_shield",
        .starter_item_ids = { "short_sword", "leather_armor", "wooden_shield", "trail_ration" },
        .starter_item_amounts = { 1, 1, 1, 3 },
    },
    {
        .class_id = CLASS_ROGUE,
        .name = "Rogue",
        .description = "Quick and sharp, with the best subterfuge.",
        .player_tile_name = "thief",
        .base_hp = 11,
        .base_mp = 0,
        .base_damage = "1d6",
        .armor_class = 10,
        .strength = 11,
        .dexterity = 15,
        .mind = 9,
        .physical = 2,
        .subterfuge = 4,
        .knowledge = 1,
        .communication = 2,
        .starter_gold = 60,
        .starter_food = 100,
        .starter_weapon_id = "dagger",
        .starter_armor_id = "leather_armor",
        .starter_offhand_id = NULL,
        .starter_item_ids = { "dagger", "leather_armor", "trail_ration", NULL },
        .starter_item_amounts = { 1, 1, 3, 0 },
    },
    {
        .class_id = CLASS_MAGI,
        .name = "Magi",
        .description = "Fragile now, dangerous once spells arrive.",
        .player_tile_name = "wizard",
        .base_hp = 8,
        .base_mp = 6,
        .base_damage = "1d4",
        .armor_class = 9,
        .strength = 8,
        .dexterity = 11,
        .mind = 15,
        .physical = 0,
        .subterfuge = 1,
        .knowledge = 4,
        .communication = 2,
        .starter_gold = 50,
        .starter_food = 80,
        .starter_weapon_id = "oak_staff",
        .starter_armor_id = "traveler_robe",
        .starter_offhand_id = NULL,
        .starter_item_ids = { "oak_staff", "traveler_robe", "spellbook", "trail_ration" },
        .starter_item_amounts = { 1, 1, 1, 2 },
    },
    {
        .class_id = CLASS_CLERIC,
        .name = "Cleric",
        .description = "Balanced body and mind with dependable defenses.",
        .player_tile_name = "cleric",
        .base_hp = 12,
        .base_mp = 4,
        .base_damage = "1d6",
        .armor_class = 10,
        .strength = 12,
        .dexterity = 10,
        .mind = 13,
        .physical = 2,
        .subterfuge = 0,
        .knowledge = 2,
        .communication = 4,
        .starter_gold = 65,
        .starter_food = 110,
        .starter_weapon_id = "mace",
        .starter_armor_id = "traveler_robe",
        .starter_offhand_id = "wooden_shield",
        .starter_item_ids = { "mace", "traveler_robe", "wooden_shield", "trail_ration" },
        .starter_item_amounts = { 1, 1, 1, 3 },
    },
};

static const size_t u2_class_templates_count = sizeof(u2_class_templates) / sizeof(u2_class_templates[0]);

static const U2MonsterTemplate u2_monster_templates[] = {
    { "orc", CLASS_NONE, 9, 0, "1d6", 10, 12, 10, 6, 2, 0, 0, 0, 12, 18 },
    { "gremlin", CLASS_NONE, 7, 0, "1d4", 10, 9, 14, 7, 1, 3, 0, 0, 8, 12 },
    { "viper", CLASS_NONE, 6, 0, "1d4+1", 10, 10, 13, 4, 1, 1, 0, 0, 6, 10 },
    { "serpent_matriarch", CLASS_NONE, 22, 0, "1d8+2", 12, 15, 14, 6, 4, 1, 0, 0, 26, 60 },
    { "moon_warden", CLASS_NONE, 20, 6, "1d6+3", 13, 11, 14, 15, 2, 1, 3, 0, 28, 64 },
};

static const size_t u2_monster_templates_count = sizeof(u2_monster_templates) / sizeof(u2_monster_templates[0]);

static const U2SpellDef u2_spell_defs[] = {
    { "arc_bolt", CLASS_MAGI, "Arc Bolt", "Choose a direction and loose a crackling bolt down that line.", 3, 4, U2_SPELL_DAMAGE_BOLT, "1d8" },
    { "phase_step", CLASS_MAGI, "Phase Step", "Slip forward through the air to the furthest clear tile ahead.", 2, 3, U2_SPELL_BLINK_FORWARD, NULL },
    { "mend", CLASS_CLERIC, "Mend", "Call restorative grace to heal your own wounds.", 3, 0, U2_SPELL_HEAL_SELF, "1d8" },
};

static const size_t u2_spell_defs_count = sizeof(u2_spell_defs) / sizeof(u2_spell_defs[0]);

static const U2TransitionDef u2_transition_defs[] = {
    { "bc1423", 36, 29, "towneBasko", 31, 63 },
    { "towneBasko", 31, 63, "bc1423", 36, 29 },
    { "bc1423", 35, 22, "castleBritish", 13, 13 },
    { "castleBritish", 13, 13, "bc1423", 35, 22 },
    { "bc1423", 38, 46, "villageOfThePreppies", 30, 63 },
    { "villageOfThePreppies", 30, 63, "bc1423", 38, 46 },
};

static const size_t u2_transition_defs_count = sizeof(u2_transition_defs) / sizeof(u2_transition_defs[0]);

static const U2TimeGateDef u2_time_gate_defs[] = {
    { "bc_europe", "bc1423", 16, 16, "ad1990", 28, 12, 4 },
    { "bc_south_america", "bc1423", 42, 24, "aftermath", 8, 11, 11 },
    { "bc_asia", "bc1423", 48, 24, "pangea", 34, 8, 17 },
    { "bc_north_america", "bc1423", 19, 52, "legends", 29, 56, 22 },

    { "ad_great_britain", "ad1990", 28, 12, "bc1423", 16, 16, 2 },

    { "aftermath_nw_america", "aftermath", 8, 11, "ad1990", 28, 12, 6 },
    { "aftermath_north_asia", "aftermath", 52, 24, "bc1423", 16, 16, 14 },
    { "aftermath_south_asia", "aftermath", 50, 34, "pangea", 34, 8, 20 },

    { "pangea_eurasia", "pangea", 34, 8, "bc1423", 48, 24, 1 },
    { "pangea_greenland", "pangea", 47, 28, "legends", 29, 56, 8 },
    { "pangea_africa", "pangea", 20, 37, "aftermath", 8, 11, 13 },
    { "pangea_south", "pangea", 36, 56, "ad1990", 28, 12, 19 },

    { "legends_1", "legends", 29, 56, "pangea", 47, 28, 0 },
    { "legends_2", "legends", 31, 56, "bc1423", 48, 24, 6 },
    { "legends_3", "legends", 33, 56, "ad1990", 28, 12, 12 },
    { "legends_4", "legends", 35, 56, "aftermath", 8, 11, 18 },
};

static const size_t u2_time_gate_defs_count = sizeof(u2_time_gate_defs) / sizeof(u2_time_gate_defs[0]);

static const U2DungeonPresetDef u2_dungeon_preset_defs[] = {
    { "serpentpit", "bc1423", 25, 5, false, "Serpent Pit", 5, "serpent", "serpent_matriarch", "Serpent Matriarch", 320, "scale_armor" },
    { "moontower", "bc1423", 24, 40, true, "Moon Tower", 5, "moon", "moon_warden", "Moon Warden", 320, "wizard_staff" },
};

static const size_t u2_dungeon_preset_defs_count = sizeof(u2_dungeon_preset_defs) / sizeof(u2_dungeon_preset_defs[0]);

static const U2SpawnDef u2_spawn_defs[] = {
    { "bc1423", "loop_guard", "Loop Guard", U2_ENTITY_KIND_NPC, CLASS_FIGHTER, NULL, "guard", 12, 17, true, false, false, 0, U2_INTERACTION_TALK, "loop_guard" },
    { "bc1423", "weathered_sign", "Weathered Sign", U2_ENTITY_KIND_NPC, CLASS_NONE, NULL, "signpost", 58, 51, true, false, false, 0, U2_INTERACTION_SIGN, "weathered_sign" },
    { "bc1423", "orc_raider", "Orc Raider", U2_ENTITY_KIND_MONSTER, CLASS_NONE, "orc", "orc", 35, 33, true, true, true, 6, U2_INTERACTION_NONE, NULL },
    { "bc1423", "gremlin_scout", "Gremlin Scout", U2_ENTITY_KIND_MONSTER, CLASS_NONE, "gremlin", "gremlin", 40, 34, true, true, true, 7, U2_INTERACTION_NONE, NULL },
    { "bc1423", "swamp_viper", "Swamp Viper", U2_ENTITY_KIND_MONSTER, CLASS_NONE, "viper", "viper", 34, 36, true, true, true, 5, U2_INTERACTION_NONE, NULL },

    { "towneBasko", "basko_guard", "Towne Guard", U2_ENTITY_KIND_NPC, CLASS_FIGHTER, NULL, "guard", 31, 3, true, false, false, 0, U2_INTERACTION_TALK, "basko_guard" },
    { "towneBasko", "basko_merchant", "Street Merchant", U2_ENTITY_KIND_NPC, CLASS_ROGUE, NULL, "merchant", 17, 47, true, false, false, 0, U2_INTERACTION_TALK, "basko_merchant" },
    { "towneBasko", "basko_sign", "Town Sign", U2_ENTITY_KIND_NPC, CLASS_NONE, NULL, "signpost", 47, 14, true, false, false, 0, U2_INTERACTION_SIGN, "basko_sign" },

    { "castleBritish", "castle_guard", "Castle Guard", U2_ENTITY_KIND_NPC, CLASS_FIGHTER, NULL, "guard2", 15, 14, true, false, false, 0, U2_INTERACTION_TALK, "castle_guard" },
    { "castleBritish", "lord_british", "Lord British", U2_ENTITY_KIND_NPC, CLASS_FIGHTER, NULL, "british", 44, 41, true, false, false, 0, U2_INTERACTION_TALK, "lord_british" },
    { "castleBritish", "castle_marker", "Castle Marker", U2_ENTITY_KIND_NPC, CLASS_NONE, NULL, "signpost", 17, 39, true, false, false, 0, U2_INTERACTION_SIGN, "castle_marker" },

    { "villageOfThePreppies", "prep_host", "Prep Host", U2_ENTITY_KIND_NPC, CLASS_CLERIC, NULL, "jester", 41, 37, true, false, false, 0, U2_INTERACTION_TALK, "prep_host" },
    { "villageOfThePreppies", "prep_seer", "Prep Seer", U2_ENTITY_KIND_NPC, CLASS_MAGI, NULL, "wizard", 41, 41, true, false, false, 0, U2_INTERACTION_TALK, "prep_seer" },
    { "villageOfThePreppies", "yacht_sign", "Yacht Sign", U2_ENTITY_KIND_NPC, CLASS_NONE, NULL, "signpost", 34, 41, true, false, false, 0, U2_INTERACTION_SIGN, "yacht_sign" },
};

static const size_t u2_spawn_defs_count = sizeof(u2_spawn_defs) / sizeof(u2_spawn_defs[0]);

static const U2DialogueDef u2_dialogue_defs[] = {
    { "loop_guard", "Keep walking west and the world will bring you right back to me." },
    { "weathered_sign", "A weathered sign reads: HEX-E-POO. Some traditions deserve to survive the port." },
    { "basko_guard", "Welcome to Towne Basko. Ozy finally reopened the counter, so stock up while you can." },
    { "basko_merchant", "Step up to the counter and mind the ledgers." },
    { "basko_sign", "Towne Basko. WEAPONS LTD and OZY ARMOURY are currently decorative." },
    { "castle_guard", "Castle British is calm today. Try not to track swamp mud through the halls." },
    { "lord_british", "Rule well, Traveler. The microlite kingdom will need you soon enough." },
    { "castle_marker", "A brass plaque reads: Audience chamber to the south. Content pending." },
    { "prep_host", "Welcome to the Village of the Preppies. A clean room costs extra, naturally." },
    { "prep_seer", "The seer predicts roguelike towers in your future, but not before a little treatment." },
    { "yacht_sign", "YACHTS. Sailing is not implemented yet, but the signage is excellent." },
};

static const size_t u2_dialogue_defs_count = sizeof(u2_dialogue_defs) / sizeof(u2_dialogue_defs[0]);

static const U2ServiceDef u2_service_defs[] = {
    { "basko_merchant", U2_SERVICE_SHOP, "Ozy's Counter", "Food packs and starter gear for travelers heading back into the wilds.", 0, 0, 0 },
    { "lord_british", U2_SERVICE_SANCTUARY, "Royal Sanctuary", "Royal hospitality can restore your body, spirit, or both.", 18, 10, 8 },
    { "prep_host", U2_SERVICE_INN, "Prep Guest House", "A polished bed and proper linens. Naturally, they are not free.", 16, 0, 0 },
    { "prep_seer", U2_SERVICE_HEALER, "Seer's Remedies", "The tonics taste strange, but they mend wounds and soothe weary minds.", 9, 7, 0 },
};

static const size_t u2_service_defs_count = sizeof(u2_service_defs) / sizeof(u2_service_defs[0]);

static const U2ShopStockDef u2_shop_stock_defs[] = {
    { "basko_merchant", "Food Pack (+40 food)", "trail_ration", 12, 40 },
    { "basko_merchant", NULL, "lesser_healing_potion", 16, 0 },
    { "basko_merchant", NULL, "focus_tonic", 16, 0 },
    { "basko_merchant", NULL, "dagger", 18, 0 },
    { "basko_merchant", NULL, "short_sword", 26, 0 },
    { "basko_merchant", NULL, "oak_staff", 18, 0 },
    { "basko_merchant", NULL, "leather_armor", 24, 0 },
    { "basko_merchant", NULL, "wooden_shield", 18, 0 },
};

static const size_t u2_shop_stock_defs_count = sizeof(u2_shop_stock_defs) / sizeof(u2_shop_stock_defs[0]);

static const U2ClassTemplate* u2_find_class_template(CharacterClass class_id) {
    for (size_t i = 0; i < u2_class_templates_count; ++i) {
        if (u2_class_templates[i].class_id == class_id) {
            return &u2_class_templates[i];
        }
    }
    return NULL;
}

static const U2MonsterTemplate* u2_find_monster_template(const char* id) {
    if (id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_monster_templates_count; ++i) {
        if (strcmp(u2_monster_templates[i].id, id) == 0) {
            return &u2_monster_templates[i];
        }
    }

    return NULL;
}

static int u2_get_spell_count_for_class(CharacterClass class_id) {
    int count = 0;

    for (size_t i = 0; i < u2_spell_defs_count; ++i) {
        if (u2_spell_defs[i].class_id == class_id) {
            count++;
        }
    }

    return count;
}

static const U2SpellDef* u2_get_spell_for_class_by_index(CharacterClass class_id, int index) {
    int current = 0;

    for (size_t i = 0; i < u2_spell_defs_count; ++i) {
        if (u2_spell_defs[i].class_id != class_id) {
            continue;
        }
        if (current == index) {
            return &u2_spell_defs[i];
        }
        current++;
    }

    return NULL;
}

static const U2SpellDef* u2_find_spell_def(const char* id) {
    if (id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_spell_defs_count; ++i) {
        if (strcmp(u2_spell_defs[i].id, id) == 0) {
            return &u2_spell_defs[i];
        }
    }

    return NULL;
}

static const U2ClassTemplate* u2_get_class_template_by_index(int index) {
    if (index < 0 || index >= (int)u2_class_templates_count) {
        return NULL;
    }
    return &u2_class_templates[index];
}

static const U2SpawnDef* u2_find_spawn_def(const char* id) {
    if (id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_spawn_defs_count; ++i) {
        if (strcmp(u2_spawn_defs[i].id, id) == 0) {
            return &u2_spawn_defs[i];
        }
    }

    return NULL;
}

static const U2TransitionDef* u2_find_transition(const char* map_id, int tile_x, int tile_y) {
    for (size_t i = 0; i < u2_transition_defs_count; ++i) {
        const U2TransitionDef* transition = &u2_transition_defs[i];
        if (strcmp(transition->source_map_id, map_id) == 0 &&
            transition->source_x == tile_x &&
            transition->source_y == tile_y) {
            return transition;
        }
    }
    return NULL;
}

static const U2TimeGateDef* u2_find_time_gate(const char* map_id, int tile_x, int tile_y) {
    if (map_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_time_gate_defs_count; ++i) {
        const U2TimeGateDef* time_gate = &u2_time_gate_defs[i];
        if (strcmp(time_gate->source_map_id, map_id) == 0 &&
            time_gate->source_x == tile_x &&
            time_gate->source_y == tile_y) {
            return time_gate;
        }
    }

    return NULL;
}

static const U2DungeonPresetDef* u2_find_dungeon_preset(
    const char* surface_map_id,
    int surface_x,
    int surface_y,
    bool tower_mode
) {
    if (surface_map_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_dungeon_preset_defs_count; ++i) {
        const U2DungeonPresetDef* preset = &u2_dungeon_preset_defs[i];
        if (preset->tower_mode == tower_mode &&
            preset->surface_x == surface_x &&
            preset->surface_y == surface_y &&
            strcmp(preset->surface_map_id, surface_map_id) == 0) {
            return preset;
        }
    }

    return NULL;
}

static const U2TransitionDef* u2_find_first_transition_from_map(const char* map_id) {
    for (size_t i = 0; i < u2_transition_defs_count; ++i) {
        if (strcmp(u2_transition_defs[i].source_map_id, map_id) == 0) {
            return &u2_transition_defs[i];
        }
    }
    return NULL;
}

static const U2DialogueDef* u2_find_dialogue(const char* id) {
    if (id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_dialogue_defs_count; ++i) {
        if (strcmp(u2_dialogue_defs[i].id, id) == 0) {
            return &u2_dialogue_defs[i];
        }
    }
    return NULL;
}

static const U2ServiceDef* u2_find_service_def(const char* entity_id) {
    if (entity_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < u2_service_defs_count; ++i) {
        if (strcmp(u2_service_defs[i].entity_id, entity_id) == 0) {
            return &u2_service_defs[i];
        }
    }

    return NULL;
}

static int u2_get_shop_stock_count(const char* service_entity_id) {
    int count = 0;

    for (size_t i = 0; i < u2_shop_stock_defs_count; ++i) {
        if (strcmp(u2_shop_stock_defs[i].service_entity_id, service_entity_id) == 0) {
            count++;
        }
    }

    return count;
}

static const U2ShopStockDef* u2_get_shop_stock_by_index(const char* service_entity_id, int index) {
    int current = 0;

    for (size_t i = 0; i < u2_shop_stock_defs_count; ++i) {
        if (strcmp(u2_shop_stock_defs[i].service_entity_id, service_entity_id) != 0) {
            continue;
        }

        if (current == index) {
            return &u2_shop_stock_defs[i];
        }

        current++;
    }

    return NULL;
}

#endif // ULTIMATUM_WORLD_DATA_H
