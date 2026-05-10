#ifndef GAME_H
#define GAME_H

#include <math.h>
#include <string.h>
#include "../chao/chao.h"
#include "session.h"
#include "world_data.h"

enum {
    U2_VIEW_WIDTH = 320,
    U2_VIEW_HEIGHT = 240,
    U2_HUD_WIDTH = 104,
    U2_LOG_HEIGHT = 38,
    U2_PANEL_MARGIN = 8,
};

static const float U2_STEP_DELAY = 0.20f;
static const float U2_SPRINT_STEP_DELAY = 0.10f;
static const float U2_LOG_MESSAGE_DURATION = 2.75f;
static const float U2_CAMERA_SMOOTHNESS = 12.0f;
static const float U2_MESSAGE_REVEAL_CHARS_PER_SECOND = 120.0f;
static const float U2_HIT_FLASH_DURATION = 0.20f;
static const float U2_HIT_BLINK_INTERVAL = 0.05f;
static const float U2_DAMAGE_POPUP_DURATION = 0.55f;
static const float U2_DAMAGE_POPUP_RISE_PIXELS = 14.0f;
static const int U2_STEPS_PER_FOOD_DEPLETION = 4;
static const int U2_STEPS_PER_FOOD_REGEN = 7;
static const int U2_MONSTER_CHASE_RADIUS = 8;
static const char* U2_SAVE_FILE_PATH = "savegame.u2sav";

enum {
    U2_SAVE_ID_MAX = 32,
    U2_SAVE_DAMAGE_MAX = 16,
    U2_SAVE_MAGIC_SIZE = 4,
    U2_SAVE_VERSION = 1,
};

enum {
    U2_MAX_HIT_FLASHES = 16,
    U2_MAX_DAMAGE_POPUPS = 16,
    U2_DAMAGE_POPUP_TEXT_MAX = 16,
};

static AsciiFont* font;
static Bitmap* font_bitmap;
static int u2_cached_save_exists = -1;

typedef struct {
    char item_id[U2_SAVE_ID_MAX];
    int amount;
} U2SavedInventorySlot;

typedef struct {
    int class_id;
    int base_hp;
    int base_mp;
    char base_damage[U2_SAVE_DAMAGE_MAX];
    int armor_class;
    int hp;
    int mp;
    int stats[3];
    int skills[4];
    int level;
    int exp;
    int stat_points;
    char equipment_ids[SLOT_END][U2_SAVE_ID_MAX];
} U2SavedCharacter;

typedef struct {
    int active;
    char id[U2_SAVE_ID_MAX];
    int tile_x;
    int tile_y;
    int facing;
    int hp;
    int mp;
} U2SavedSceneEntity;

typedef struct {
    char magic[U2_SAVE_MAGIC_SIZE];
    int version;
    char player_name[U2_PLAYER_NAME_MAX];
    int chosen_class;
    char current_map_id[U2_SAVE_ID_MAX];
    int player_tile_x;
    int player_tile_y;
    int player_facing;
    int gold;
    int food;
    int food_depletion_step_counter;
    int food_regen_step_counter;
    U2SavedCharacter player;
    int inventory_count;
    U2SavedInventorySlot inventory[U2_MAX_INVENTORY_SLOTS];
    int scene_entity_count;
    U2SavedSceneEntity scene_entities[U2_MAX_SCENE_ENTITIES];
} U2SaveData;

typedef struct {
    bool active;
    bool target_player;
    char entity_id[U2_SAVE_ID_MAX];
    float timer;
    float duration;
} U2HitFlashState;

typedef struct {
    bool active;
    float tile_x;
    float tile_y;
    float timer;
    float duration;
    uint32_t base_color;
    char text[U2_DAMAGE_POPUP_TEXT_MAX];
} U2DamagePopupState;

typedef struct {
    int total_damage;
    int successful_hits;
    int glancing_hits;
    int misses;
    bool any_attack_attempted;
} U2AttackResolution;

typedef struct {
    const U2TileDef* tile_lookup[256];
    float camera_x;
    float camera_y;
    NineSlice frame_slice;
    U2HitFlashState hit_flashes[U2_MAX_HIT_FLASHES];
    U2DamagePopupState damage_popups[U2_MAX_DAMAGE_POPUPS];
    U2GameSession session;
} UltimatumGameState;

static UltimatumGameState game_state;

static char u2_hex_digit(int value) {
    return (char)(value < 10 ? ('0' + value) : ('A' + (value - 10)));
}

static bool u2_try_parse_hex_digit(char c, unsigned char* out_value) {
    if (out_value == NULL) {
        return false;
    }

    if (c >= '0' && c <= '9') {
        *out_value = (unsigned char)(c - '0');
        return true;
    }
    if (c >= 'A' && c <= 'F') {
        *out_value = (unsigned char)(10 + (c - 'A'));
        return true;
    }
    if (c >= 'a' && c <= 'f') {
        *out_value = (unsigned char)(10 + (c - 'a'));
        return true;
    }

    return false;
}

static char* u2_encode_save_data_hex(const U2SaveData* save_data) {
    const unsigned char* bytes = (const unsigned char*)save_data;
    const size_t byte_count = sizeof(*save_data);
    char* encoded = (char*)malloc(byte_count * 2 + 1);

    if (save_data == NULL || encoded == NULL) {
        free(encoded);
        return NULL;
    }

    for (size_t i = 0; i < byte_count; ++i) {
        encoded[i * 2] = u2_hex_digit((bytes[i] >> 4) & 0x0F);
        encoded[i * 2 + 1] = u2_hex_digit(bytes[i] & 0x0F);
    }
    encoded[byte_count * 2] = '\0';
    return encoded;
}

static bool u2_decode_save_data_hex(U2SaveData* save_data, const char* encoded) {
    unsigned char* bytes = (unsigned char*)save_data;
    const size_t byte_count = sizeof(*save_data);
    const size_t encoded_length = encoded != NULL ? strlen(encoded) : 0;

    if (save_data == NULL || encoded == NULL || encoded_length != byte_count * 2) {
        return false;
    }

    for (size_t i = 0; i < byte_count; ++i) {
        unsigned char high = 0;
        unsigned char low = 0;

        if (!u2_try_parse_hex_digit(encoded[i * 2], &high) ||
            !u2_try_parse_hex_digit(encoded[i * 2 + 1], &low)) {
            return false;
        }

        bytes[i] = (unsigned char)((high << 4) | low);
    }

    return true;
}

static void u2_copy_string(char* dst, size_t dst_size, const char* src) {
    if (dst == NULL || dst_size == 0) {
        return;
    }

    snprintf(dst, dst_size, "%s", src != NULL ? src : "");
}

static int u2_world_view_width(void) {
    return screen_size.x - U2_HUD_WIDTH;
}

static int u2_world_view_height(void) {
    return screen_size.y;
}

static void u2_clear_hit_feedback(void) {
    memset(game_state.hit_flashes, 0, sizeof(game_state.hit_flashes));
    memset(game_state.damage_popups, 0, sizeof(game_state.damage_popups));
}

static U2HitFlashState* u2_claim_hit_flash_slot(void) {
    for (int i = 0; i < U2_MAX_HIT_FLASHES; ++i) {
        if (!game_state.hit_flashes[i].active) {
            return &game_state.hit_flashes[i];
        }
    }

    return &game_state.hit_flashes[0];
}

static U2DamagePopupState* u2_claim_damage_popup_slot(void) {
    for (int i = 0; i < U2_MAX_DAMAGE_POPUPS; ++i) {
        if (!game_state.damage_popups[i].active) {
            return &game_state.damage_popups[i];
        }
    }

    return &game_state.damage_popups[0];
}

static void u2_spawn_hit_flash(const U2Entity* entity, bool target_player) {
    U2HitFlashState* flash = NULL;

    if ((entity == NULL && !target_player) || (entity != NULL && entity->id == NULL && !target_player)) {
        return;
    }

    flash = u2_claim_hit_flash_slot();
    memset(flash, 0, sizeof(*flash));
    flash->active = true;
    flash->target_player = target_player;
    flash->duration = U2_HIT_FLASH_DURATION;

    if (!target_player && entity != NULL) {
        u2_copy_string(flash->entity_id, sizeof(flash->entity_id), entity->id);
    }
}

static void u2_spawn_damage_popup(float tile_x, float tile_y, uint32_t base_color, const char* text) {
    U2DamagePopupState* popup = NULL;

    if (text == NULL || text[0] == '\0') {
        return;
    }

    popup = u2_claim_damage_popup_slot();
    memset(popup, 0, sizeof(*popup));
    popup->active = true;
    popup->tile_x = tile_x;
    popup->tile_y = tile_y;
    popup->duration = U2_DAMAGE_POPUP_DURATION;
    popup->base_color = base_color;
    u2_copy_string(popup->text, sizeof(popup->text), text);
}

static void u2_spawn_entity_damage_popup(const U2Entity* entity, uint32_t base_color, const char* text) {
    if (!u2_entity_is_valid(entity)) {
        return;
    }

    u2_spawn_damage_popup((float)entity->tile_x, (float)entity->tile_y, base_color, text);
}

static void u2_spawn_player_damage_popup(uint32_t base_color, const char* text) {
    u2_spawn_damage_popup((float)game_state.session.player.tile_x, (float)game_state.session.player.tile_y, base_color, text);
}

static void u2_update_hit_feedback(float dt) {
    for (int i = 0; i < U2_MAX_HIT_FLASHES; ++i) {
        U2HitFlashState* flash = &game_state.hit_flashes[i];
        if (!flash->active) {
            continue;
        }

        flash->timer += dt;
        if (flash->timer >= flash->duration) {
            memset(flash, 0, sizeof(*flash));
        }
    }

    for (int i = 0; i < U2_MAX_DAMAGE_POPUPS; ++i) {
        U2DamagePopupState* popup = &game_state.damage_popups[i];
        if (!popup->active) {
            continue;
        }

        popup->timer += dt;
        if (popup->timer >= popup->duration) {
            memset(popup, 0, sizeof(*popup));
        }
    }
}

static float u2_wrapf(float value, float size) {
    if (size <= 0.0f) {
        return 0.0f;
    }

    while (value < 0.0f) {
        value += size;
    }
    while (value >= size) {
        value -= size;
    }

    return value;
}

static float u2_wrap_deltaf(float from, float to, float size) {
    float delta = to - from;
    if (size <= 0.0f) {
        return delta;
    }
    while (delta > size * 0.5f) {
        delta -= size;
    }
    while (delta < -size * 0.5f) {
        delta += size;
    }
    return delta;
}

static void u2_build_tile_lookup(void) {
    memset(game_state.tile_lookup, 0, sizeof(game_state.tile_lookup));

    for (size_t i = 0; i < u2_tile_defs_count; ++i) {
        const unsigned char symbol = (unsigned char)u2_tile_defs[i].symbol;
        if (game_state.tile_lookup[symbol] == NULL) {
            game_state.tile_lookup[symbol] = &u2_tile_defs[i];
        }
    }
}

static int u2_current_map_pixel_width(void) {
    return game_state.session.current_map == NULL ? 0 : game_state.session.current_map->width * U2_TILE_SIZE;
}

static int u2_current_map_pixel_height(void) {
    return game_state.session.current_map == NULL ? 0 : game_state.session.current_map->height * U2_TILE_SIZE;
}

static bool u2_scene_is_looped(void) {
    return game_state.session.scene_type == U2_SCENE_OVERWORLD;
}

static const char* u2_class_name(CharacterClass class_id) {
    const U2ClassTemplate* class_template = u2_find_class_template(class_id);
    return class_template != NULL ? class_template->name : "None";
}

static const char* u2_slot_name(Slot slot) {
    switch (slot) {
        case SLOT_HAND_RIGHT: return "Weapon";
        case SLOT_HAND_LEFT: return "Offhand";
        case SLOT_BODY: return "Body";
        case SLOT_HEAD: return "Head";
        case SLOT_ACCESSORY: return "Accessory";
        case SLOT_END:
        default:
            return "Slot";
    }
}

static bool u2_get_item_equip_slot(const Item* item, Slot* out_slot) {
    if (item == NULL || out_slot == NULL) {
        return false;
    }

    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            *out_slot = SLOT_HAND_RIGHT;
            return true;
        case ITEM_TYPE_SHIELD:
            *out_slot = SLOT_HAND_LEFT;
            return true;
        case ITEM_TYPE_ARMOR:
            *out_slot = SLOT_BODY;
            return true;
        case ITEM_TYPE_HAT:
            *out_slot = SLOT_HEAD;
            return true;
        default:
            return false;
    }
}

static bool u2_item_is_consumable(const Item* item) {
    if (item == NULL) {
        return false;
    }

    return item->type == ITEM_TYPE_FOOD || item->type == ITEM_TYPE_CONSUMABLE;
}

static int u2_get_player_spell_count(void) {
    return u2_get_spell_count_for_class(game_state.session.chosen_class);
}

static const U2SpellDef* u2_get_player_spell_by_index(int index) {
    return u2_get_spell_for_class_by_index(game_state.session.chosen_class, index);
}

static const U2SpellDef* u2_get_targeting_spell(void) {
    return u2_find_spell_def(game_state.session.spell_targeting.spell_id);
}

static const U2SpellDef* u2_get_active_projectile_spell(void) {
    return u2_find_spell_def(game_state.session.spell_projectile.spell_id);
}

static const U2TileDef* u2_get_tile_at(const U2MapDef* map, int tile_x, int tile_y) {
    if (map == NULL || tile_x < 0 || tile_y < 0 || tile_x >= map->width || tile_y >= map->height) {
        return NULL;
    }

    const unsigned char symbol = (unsigned char)map->rows[tile_y][tile_x];
    return game_state.tile_lookup[symbol];
}

static const char* u2_get_blocked_message(const U2TileDef* tile) {
    if (tile == NULL) {
        return "You cannot go that way.";
    }
    if (strcmp(tile->name, "water") == 0) {
        return "The water blocks your way.";
    }
    if (strcmp(tile->name, "mountains") == 0) {
        return "The mountains are impassable.";
    }
    if (strcmp(tile->name, "wall") == 0 || strcmp(tile->name, "door") == 0) {
        return "The wall bars your way.";
    }
    return "That way is blocked.";
}

static const char* u2_get_unimplemented_message(const U2TileDef* tile) {
    if (tile == NULL) {
        return NULL;
    }
    if (strcmp(tile->name, "dungeon") == 0 || strcmp(tile->name, "tower") == 0) {
        return "Roguelike dungeons and towers are not implemented yet.";
    }
    if (strcmp(tile->name, "portal") == 0) {
        return "Portals and planetary travel are not implemented yet.";
    }
    return NULL;
}

static void u2_draw_tile(const U2TileDef* tile, int x, int y) {
    if (tile == NULL) {
        fill_rect(chao_canvas, x, y, U2_TILE_SIZE, U2_TILE_SIZE, COLOR_MAGENTA);
        draw_rect(chao_canvas, x, y, U2_TILE_SIZE, U2_TILE_SIZE, 1, COLOR_BLACK);
        return;
    }

    RectInt src = {
        tile->atlas_x * U2_TILE_SIZE,
        tile->atlas_y * U2_TILE_SIZE,
        U2_TILE_SIZE,
        U2_TILE_SIZE
    };

    blit_rect(chao_canvas, AGB("tiles"), src, (Vector2Int){x, y}, tile->tint);
}

static void u2_draw_panel(int x, int y, int w, int h) {
    fill_rect(chao_canvas, x + 3, y + 3, w - 6, h - 6, COLOR_BLACK);
    nine_slice_draw(chao_canvas, game_state.frame_slice, x, y, w, h);
}

static void u2_draw_wrapped_text(int x, int y, int max_width, uint32_t color, const char* text) {
    char* wrapped = ascii_font_break_lines(font, max_width, "%s", text);
    ascii_font_draw(chao_canvas, font, x, y, color, "%s", wrapped);
    free(wrapped);
}

static Vector2Int u2_get_wrapped_text_size(int max_width, const char* text) {
    Vector2Int size = { 0, 0 };
    char* wrapped = ascii_font_break_lines(font, max_width, "%s", text);
    size = ascii_font_get_size(font, wrapped, -1);
    free(wrapped);
    return size;
}

static void u2_get_revealed_wrapped_message_text(int max_width, char* dst, size_t dst_size) {
    char* wrapped = ascii_font_break_lines(font, max_width, "%s", game_state.session.message.text);
    const int visible_chars = game_state.session.message.visible_chars;
    size_t out_i = 0;
    int revealed_raw_chars = 0;

    for (size_t i = 0; wrapped[i] != '\0' && out_i + 1 < dst_size; ++i) {
        if (wrapped[i] == '\n') {
            dst[out_i++] = '\n';
            continue;
        }

        if (revealed_raw_chars >= visible_chars) {
            break;
        }

        dst[out_i++] = wrapped[i];
        revealed_raw_chars++;
    }

    dst[out_i] = '\0';
    free(wrapped);
}

static void u2_get_facing_delta(U2Facing facing, int* out_dx, int* out_dy) {
    *out_dx = 0;
    *out_dy = 0;

    switch (facing) {
        case U2_FACING_WEST: *out_dx = -1; break;
        case U2_FACING_EAST: *out_dx = 1; break;
        case U2_FACING_NORTH: *out_dy = -1; break;
        case U2_FACING_SOUTH: *out_dy = 1; break;
    }
}

static bool u2_get_offset_tile(int start_x, int start_y, int dx, int dy, int distance, int* out_x, int* out_y) {
    int tile_x = start_x + dx * distance;
    int tile_y = start_y + dy * distance;

    if (u2_scene_is_looped()) {
        tile_x = u2_entity_wrap_coord(tile_x, game_state.session.current_map->width);
        tile_y = u2_entity_wrap_coord(tile_y, game_state.session.current_map->height);
    } else if (tile_x < 0 || tile_y < 0 || tile_x >= game_state.session.current_map->width || tile_y >= game_state.session.current_map->height) {
        return false;
    }

    *out_x = tile_x;
    *out_y = tile_y;
    return true;
}

static bool u2_is_counter_tile(const U2TileDef* tile) {
    if (tile == NULL) {
        return false;
    }

    return strcmp(tile->name, "deskLeft") == 0 ||
        strcmp(tile->name, "desk") == 0 ||
        strcmp(tile->name, "deskRight") == 0;
}

static uint32_t u2_get_food_color(void) {
    return game_state.session.food > 0 ? COLOR_WHITE : COLOR_RED;
}

static const U2ServiceDef* u2_get_active_service_def(void) {
    return u2_find_service_def(game_state.session.active_service_entity_id);
}

static void u2_clamp_player_resources(void) {
    game_state.session.player.sheet.hp = clamp(
        game_state.session.player.sheet.hp,
        0,
        d20_get_max_hp(&game_state.session.player.sheet)
    );
    game_state.session.player.sheet.mp = clamp(
        game_state.session.player.sheet.mp,
        0,
        d20_get_max_mp(&game_state.session.player.sheet)
    );
}

static void u2_close_service_panel(void) {
    game_state.session.panel = U2_PANEL_NONE;
    game_state.session.active_service_entity_id = NULL;
    game_state.session.service_selection_index = 0;
}

static void u2_open_service_panel(const U2Entity* entity) {
    const U2ServiceDef* service = u2_find_service_def(entity->id);
    if (service == NULL) {
        return;
    }

    game_state.session.panel = U2_PANEL_SERVICE;
    game_state.session.active_service_entity_id = service->entity_id;
    game_state.session.service_selection_index = 0;
}

static void u2_open_inventory_panel(void) {
    game_state.session.panel = U2_PANEL_INVENTORY;
    game_state.session.inventory_selection_index = clamp(
        game_state.session.inventory_selection_index,
        0,
        max(0, game_state.session.inventory.count - 1)
    );
}

static void u2_open_spell_panel(void) {
    game_state.session.panel = U2_PANEL_SPELLS;
    game_state.session.spell_selection_index = clamp(
        game_state.session.spell_selection_index,
        0,
        max(0, u2_get_player_spell_count() - 1)
    );
}

static void u2_begin_spell_targeting(const U2SpellDef* spell) {
    if (spell == NULL) {
        return;
    }

    game_state.session.panel = U2_PANEL_NONE;
    game_state.session.spell_targeting.active = true;
    game_state.session.spell_targeting.spell_id = spell->id;
    u2_session_show_message(&game_state.session, false, 0.0f, "Choose a direction for %s.", spell->name);
}

static void u2_scene_clear_entities(void) {
    u2_session_clear_scene_entities(&game_state.session);
}

static U2PersistentEntityState* u2_get_persistent_entity_state(const char* id, bool create_if_missing) {
    U2PersistentEntityState* empty_slot = NULL;

    if (id == NULL) {
        return NULL;
    }

    for (int i = 0; i < U2_MAX_ENTITIES; ++i) {
        U2PersistentEntityState* state = &game_state.session.persistent_entities[i];
        if (state->in_use && state->id != NULL && strcmp(state->id, id) == 0) {
            return state;
        }
        if (empty_slot == NULL && !state->in_use) {
            empty_slot = state;
        }
    }

    if (!create_if_missing || empty_slot == NULL) {
        return NULL;
    }

    memset(empty_slot, 0, sizeof(*empty_slot));
    empty_slot->in_use = true;
    empty_slot->id = id;
    return empty_slot;
}

static void u2_seed_persistent_entity_state_from_entity(U2PersistentEntityState* state, const U2Entity* entity) {
    if (state == NULL || entity == NULL) {
        return;
    }

    state->seeded = true;
    state->active = entity->active;
    state->tile_x = entity->tile_x;
    state->tile_y = entity->tile_y;
    state->facing = entity->facing;
    state->hp = entity->sheet.hp;
    state->mp = entity->sheet.mp;
}

static void u2_sync_persistent_entity_state_from_entity(const U2Entity* entity) {
    U2PersistentEntityState* state = NULL;

    if (entity == NULL || entity->id == NULL) {
        return;
    }

    state = u2_get_persistent_entity_state(entity->id, true);
    if (state == NULL) {
        return;
    }

    u2_seed_persistent_entity_state_from_entity(state, entity);
}

static void u2_mark_persistent_entity_inactive(const char* id) {
    U2PersistentEntityState* state = u2_get_persistent_entity_state(id, true);
    if (state == NULL) {
        return;
    }

    state->seeded = true;
    state->active = false;
}

static void u2_assign_spawn_sheet(U2Entity* entity, const U2SpawnDef* spawn_def) {
    const U2MonsterTemplate* monster_template = spawn_def != NULL ? u2_find_monster_template(spawn_def->monster_template_id) : NULL;

    if (monster_template != NULL) {
        u2_entity_set_character_sheet(
            entity,
            monster_template->class_id,
            monster_template->base_hp,
            monster_template->base_mp,
            monster_template->base_damage,
            monster_template->armor_class,
            monster_template->strength,
            monster_template->dexterity,
            monster_template->mind,
            monster_template->physical,
            monster_template->subterfuge,
            monster_template->knowledge,
            monster_template->communication
        );
        return;
    }

    if (spawn_def != NULL) {
        const U2ClassTemplate* class_template = u2_find_class_template(spawn_def->class_id);
        if (class_template != NULL) {
            u2_entity_set_character_sheet(
                entity,
                class_template->class_id,
                class_template->base_hp,
                class_template->base_mp,
                class_template->base_damage,
                class_template->armor_class,
                class_template->strength,
                class_template->dexterity,
                class_template->mind,
                class_template->physical,
                class_template->subterfuge,
                class_template->knowledge,
                class_template->communication
            );
            return;
        }
    }

    memset(&entity->sheet, 0, sizeof(entity->sheet));
    entity->sheet.class = CLASS_NONE;
}

static void u2_spawn_scene_entity_from_def(const U2SpawnDef* spawn_def) {
    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        U2Entity* entity = &game_state.session.scene_entities[i];
        if (!entity->active) {
            U2PersistentEntityState* persistent_state = u2_get_persistent_entity_state(spawn_def->id, true);

            u2_entity_init(entity, spawn_def->id, spawn_def->name, spawn_def->kind, spawn_def->tile_name, spawn_def->tile_x, spawn_def->tile_y);
            entity->solid = spawn_def->solid;
            entity->hostile = spawn_def->hostile;
            entity->roaming = spawn_def->roaming;
            entity->roam_radius = spawn_def->roam_radius;
            entity->interaction_kind = spawn_def->interaction_kind;
            entity->dialogue_id = spawn_def->dialogue_id;
            u2_assign_spawn_sheet(entity, spawn_def);

            if (persistent_state != NULL) {
                if (!persistent_state->seeded) {
                    u2_seed_persistent_entity_state_from_entity(persistent_state, entity);
                }

                if (!persistent_state->active) {
                    u2_entity_reset(entity);
                    return;
                }

                entity->tile_x = persistent_state->tile_x;
                entity->tile_y = persistent_state->tile_y;
                entity->facing = persistent_state->facing;
                entity->sheet.hp = max(0, persistent_state->hp);
                entity->sheet.mp = max(0, persistent_state->mp);
                u2_entity_clamp_to_map(entity, game_state.session.current_map);
            }

            return;
        }
    }
}

static void u2_rebuild_scene_entities(void) {
    u2_scene_clear_entities();

    if (game_state.session.current_map == NULL) {
        return;
    }

    for (size_t i = 0; i < u2_spawn_defs_count; ++i) {
        if (strcmp(u2_spawn_defs[i].map_id, game_state.session.current_map->id) == 0) {
            u2_spawn_scene_entity_from_def(&u2_spawn_defs[i]);
        }
    }
}

static void u2_set_player_position(int tile_x, int tile_y) {
    game_state.session.player.tile_x = tile_x;
    game_state.session.player.tile_y = tile_y;

    if (game_state.session.current_map != NULL) {
        if (u2_scene_is_looped()) {
            game_state.session.player.tile_x = u2_entity_wrap_coord(game_state.session.player.tile_x, game_state.session.current_map->width);
            game_state.session.player.tile_y = u2_entity_wrap_coord(game_state.session.player.tile_y, game_state.session.current_map->height);
        } else {
            game_state.session.player.tile_x = clamp(game_state.session.player.tile_x, 0, game_state.session.current_map->width - 1);
            game_state.session.player.tile_y = clamp(game_state.session.player.tile_y, 0, game_state.session.current_map->height - 1);
        }
    }
}

static void u2_get_player_camera_target(float* out_x, float* out_y) {
    if (!u2_entity_is_valid(&game_state.session.player) || game_state.session.current_map == NULL) {
        *out_x = 0.0f;
        *out_y = 0.0f;
        return;
    }

    *out_x = (game_state.session.player.tile_x * U2_TILE_SIZE) - (u2_world_view_width() / 2.0f) + (U2_TILE_SIZE / 2.0f);
    *out_y = (game_state.session.player.tile_y * U2_TILE_SIZE) - (u2_world_view_height() / 2.0f) + (U2_TILE_SIZE / 2.0f);

    if (u2_scene_is_looped()) {
        *out_x = u2_wrapf(*out_x, (float)u2_current_map_pixel_width());
        *out_y = u2_wrapf(*out_y, (float)u2_current_map_pixel_height());
    } else {
        *out_x = clamp(*out_x, 0.0f, max(0.0f, (float)(u2_current_map_pixel_width() - u2_world_view_width())));
        *out_y = clamp(*out_y, 0.0f, max(0.0f, (float)(u2_current_map_pixel_height() - u2_world_view_height())));
    }
}

static void u2_snap_camera_to_player(void) {
    float target_x = 0.0f;
    float target_y = 0.0f;
    u2_get_player_camera_target(&target_x, &target_y);
    game_state.camera_x = target_x;
    game_state.camera_y = target_y;
}

static void u2_update_camera(float dt) {
    float target_x = 0.0f;
    float target_y = 0.0f;
    const float alpha = 1.0f - expf(-U2_CAMERA_SMOOTHNESS * dt);

    u2_get_player_camera_target(&target_x, &target_y);

    if (u2_scene_is_looped()) {
        game_state.camera_x += u2_wrap_deltaf(game_state.camera_x, target_x, (float)u2_current_map_pixel_width()) * alpha;
        game_state.camera_y += u2_wrap_deltaf(game_state.camera_y, target_y, (float)u2_current_map_pixel_height()) * alpha;
        game_state.camera_x = u2_wrapf(game_state.camera_x, (float)u2_current_map_pixel_width());
        game_state.camera_y = u2_wrapf(game_state.camera_y, (float)u2_current_map_pixel_height());
    } else {
        game_state.camera_x += (target_x - game_state.camera_x) * alpha;
        game_state.camera_y += (target_y - game_state.camera_y) * alpha;
        game_state.camera_x = clamp(game_state.camera_x, 0.0f, max(0.0f, (float)(u2_current_map_pixel_width() - u2_world_view_width())));
        game_state.camera_y = clamp(game_state.camera_y, 0.0f, max(0.0f, (float)(u2_current_map_pixel_height() - u2_world_view_height())));
    }
}

static void u2_load_scene(const char* map_id, int player_x, int player_y) {
    const U2MapDef* map = u2_find_map(map_id);
    if (map == NULL) {
        return;
    }

    u2_clear_hit_feedback();
    game_state.session.current_map = map;
    game_state.session.scene_type = map->looped ? U2_SCENE_OVERWORLD : U2_SCENE_INTERIOR;
    u2_close_service_panel();
    u2_set_player_position(player_x, player_y);
    u2_rebuild_scene_entities();
    u2_snap_camera_to_player();
}

static void u2_equip_item_if_present(Character* sheet, Slot slot, const char* item_id) {
    if (item_id != NULL) {
        sheet->equipment[slot] = items_get(item_id);
    }
}

static void u2_restore_player_hp(void) {
    game_state.session.player.sheet.hp = d20_get_max_hp(&game_state.session.player.sheet);
}

static void u2_restore_player_mp(void) {
    game_state.session.player.sheet.mp = d20_get_max_mp(&game_state.session.player.sheet);
}

static void u2_restore_player_fully(void) {
    u2_restore_player_hp();
    u2_restore_player_mp();
}

static void u2_update_food_for_step(void) {
    game_state.session.food_depletion_step_counter++;
    if (game_state.session.food_depletion_step_counter >= U2_STEPS_PER_FOOD_DEPLETION) {
        game_state.session.food_depletion_step_counter = 0;
        if (game_state.session.food > 0) {
            game_state.session.food--;
        }
    }

    game_state.session.food_regen_step_counter++;
    if (game_state.session.food_regen_step_counter < U2_STEPS_PER_FOOD_REGEN) {
        return;
    }

    game_state.session.food_regen_step_counter = 0;

    if (game_state.session.food > 0 &&
        game_state.session.player.sheet.hp < d20_get_max_hp(&game_state.session.player.sheet)) {
        game_state.session.player.sheet.hp++;
    }
}

static void u2_configure_player_from_template(const U2ClassTemplate* class_template) {
    u2_entity_init(
        &game_state.session.player,
        "player",
        game_state.session.player_name,
        U2_ENTITY_KIND_PLAYER,
        class_template->player_tile_name,
        16,
        17
    );
    game_state.session.player.solid = true;

    u2_entity_set_character_sheet(
        &game_state.session.player,
        class_template->class_id,
        class_template->base_hp,
        class_template->base_mp,
        class_template->base_damage,
        class_template->armor_class,
        class_template->strength,
        class_template->dexterity,
        class_template->mind,
        class_template->physical,
        class_template->subterfuge,
        class_template->knowledge,
        class_template->communication
    );

    memset(game_state.session.player.sheet.equipment, 0, sizeof(game_state.session.player.sheet.equipment));
    u2_equip_item_if_present(&game_state.session.player.sheet, SLOT_HAND_RIGHT, class_template->starter_weapon_id);
    u2_equip_item_if_present(&game_state.session.player.sheet, SLOT_BODY, class_template->starter_armor_id);
    u2_equip_item_if_present(&game_state.session.player.sheet, SLOT_HAND_LEFT, class_template->starter_offhand_id);

    u2_inventory_clear(&game_state.session.inventory);
    for (int i = 0; i < U2_MAX_STARTER_ITEMS; ++i) {
        if (class_template->starter_item_ids[i] != NULL && class_template->starter_item_amounts[i] > 0) {
            u2_inventory_add_by_id(
                &game_state.session.inventory,
                class_template->starter_item_ids[i],
                (size_t)class_template->starter_item_amounts[i]
            );
        }
    }

    game_state.session.gold = class_template->starter_gold;
    game_state.session.food = class_template->starter_food;
    game_state.session.chosen_class = class_template->class_id;
}

static void u2_start_new_game(const U2ClassTemplate* class_template) {
    u2_session_init(&game_state.session);
    game_state.session.started = true;
    game_state.session.flow_state = U2_FLOW_PLAYING;
    u2_configure_player_from_template(class_template);
    u2_load_scene("bc1423", 34, 31);
    u2_session_show_message(
        &game_state.session,
        false,
        U2_LOG_MESSAGE_DURATION,
        "Welcome, %s %s. Walk onto towns, castles, and villages to enter them.",
        game_state.session.player_name,
        class_template->name
    );
}

static U2Entity* u2_find_scene_entity_at(int tile_x, int tile_y) {
    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        U2Entity* entity = &game_state.session.scene_entities[i];
        if (u2_entity_is_valid(entity) && entity->tile_x == tile_x && entity->tile_y == tile_y) {
            return entity;
        }
    }
    return NULL;
}

static U2Entity* u2_find_scene_entity_at_excluding(int tile_x, int tile_y, const U2Entity* excluded_entity) {
    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        U2Entity* entity = &game_state.session.scene_entities[i];
        if (entity == excluded_entity) {
            continue;
        }
        if (u2_entity_is_valid(entity) && entity->tile_x == tile_x && entity->tile_y == tile_y) {
            return entity;
        }
    }
    return NULL;
}

static U2Entity* u2_find_scene_entity_by_id(const char* id) {
    if (id == NULL) {
        return NULL;
    }

    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        U2Entity* entity = &game_state.session.scene_entities[i];
        if (u2_entity_is_valid(entity) && entity->id != NULL && strcmp(entity->id, id) == 0) {
            return entity;
        }
    }

    return NULL;
}

static bool u2_is_hostile_entity(const U2Entity* entity) {
    return u2_entity_is_valid(entity) && (entity->hostile || entity->kind == U2_ENTITY_KIND_MONSTER);
}

static void u2_append_textf(char* dst, size_t dst_size, const char* fmt, ...) {
    va_list args;
    size_t length = 0;

    if (dst == NULL || dst_size == 0 || fmt == NULL) {
        return;
    }

    length = strlen(dst);
    if (length >= dst_size - 1) {
        return;
    }

    va_start(args, fmt);
    vsnprintf(dst + length, dst_size - length, fmt, args);
    va_end(args);
}

static void u2_set_combat_log(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vsnprintf(game_state.session.combat.log, sizeof(game_state.session.combat.log), fmt, args);
    va_end(args);
}

static U2Entity* u2_get_combat_source_entity(void) {
    if (!game_state.session.combat.active || game_state.session.combat.source_entity_id[0] == '\0') {
        return NULL;
    }

    return u2_find_scene_entity_by_id(game_state.session.combat.source_entity_id);
}

static const U2MonsterTemplate* u2_find_monster_template_for_entity(const U2Entity* entity) {
    const U2SpawnDef* spawn_def = NULL;

    if (entity == NULL || entity->id == NULL) {
        return NULL;
    }

    spawn_def = u2_find_spawn_def(entity->id);
    return spawn_def != NULL ? u2_find_monster_template(spawn_def->monster_template_id) : NULL;
}

static int u2_get_hostile_reward_gold(const U2Entity* entity) {
    const U2MonsterTemplate* monster_template = u2_find_monster_template_for_entity(entity);
    return monster_template != NULL ? monster_template->reward_gold : 0;
}

static int u2_get_hostile_reward_exp(const U2Entity* entity) {
    const U2MonsterTemplate* monster_template = u2_find_monster_template_for_entity(entity);
    return monster_template != NULL ? monster_template->reward_exp : 0;
}

static void u2_get_attack_feedback_text(const U2AttackResolution* resolution, char* dst, size_t dst_size) {
    if (dst == NULL || dst_size == 0) {
        return;
    }

    dst[0] = '\0';
    if (resolution == NULL || !resolution->any_attack_attempted) {
        return;
    }

    if (resolution->total_damage > 0) {
        snprintf(dst, dst_size, "%d", resolution->total_damage);
    } else if (resolution->glancing_hits > 0) {
        snprintf(dst, dst_size, "0");
    } else {
        snprintf(dst, dst_size, "MISS");
    }
}

static void u2_apply_attack_feedback_to_player(const U2AttackResolution* resolution) {
    char feedback_text[U2_DAMAGE_POPUP_TEXT_MAX];

    u2_get_attack_feedback_text(resolution, feedback_text, sizeof(feedback_text));
    if (feedback_text[0] == '\0') {
        return;
    }

    if (resolution->successful_hits > 0 || resolution->glancing_hits > 0) {
        u2_spawn_hit_flash(NULL, true);
    }
    u2_spawn_player_damage_popup(
        resolution->successful_hits > 0 ? COLOR_WHITE : COLOR_LIGHTGRAY,
        feedback_text
    );
}

static void u2_apply_attack_feedback_to_entity(const U2Entity* entity, const U2AttackResolution* resolution) {
    char feedback_text[U2_DAMAGE_POPUP_TEXT_MAX];

    if (!u2_entity_is_valid(entity)) {
        return;
    }

    u2_get_attack_feedback_text(resolution, feedback_text, sizeof(feedback_text));
    if (feedback_text[0] == '\0') {
        return;
    }

    if (resolution->successful_hits > 0 || resolution->glancing_hits > 0) {
        u2_spawn_hit_flash(entity, false);
    }
    u2_spawn_entity_damage_popup(
        entity,
        resolution->successful_hits > 0 ? COLOR_WHITE : COLOR_LIGHTGRAY,
        feedback_text
    );
}

static void u2_award_player_exp(int exp_reward, char* out_message, size_t out_message_size) {
    Character* player = &game_state.session.player.sheet;
    const int starting_level = player->level;
    const int starting_stat_points = player->stat_points;

    if (out_message != NULL && out_message_size > 0) {
        out_message[0] = '\0';
    }

    if (exp_reward <= 0) {
        return;
    }

    player->exp += exp_reward;

    while (player->exp >= d20_get_exp_for_level(player->level + 1)) {
        d20_advance_level(player);
    }

    if (player->level > starting_level) {
        u2_restore_player_fully();
        if (out_message != NULL && out_message_size > 0) {
            snprintf(out_message, out_message_size, "Level up! You are now level %d.", player->level);
            if (player->stat_points > starting_stat_points) {
                u2_append_textf(out_message, out_message_size, " %d stat points await a future training system.", player->stat_points);
            }
        }
    }
}

static U2AttackResolution u2_perform_attack_sequence(Character* attacker, Character* target) {
    U2AttackResolution resolution = { 0 };
    int attack_bonus = 0;

    if (attacker == NULL || target == NULL) {
        return resolution;
    }

    attack_bonus = d20_get_attack_bonus(attacker, BONUS_MELEE);

    for (int i = 0; i < d20_get_number_of_attacks(attacker); ++i) {
        int damage = -1;
        bool used_right_hand = false;

        if (target->hp <= 0) {
            break;
        }

        if (d20_get_item_type(attacker, SLOT_HAND_RIGHT) != ITEM_TYPE_SHIELD) {
            damage = d20_perform_single_attack(attacker, target, SLOT_HAND_RIGHT, attack_bonus - (i * 5));
            resolution.any_attack_attempted = true;
            if (damage > 0) {
                resolution.total_damage += damage;
                resolution.successful_hits++;
            } else if (damage == 0) {
                resolution.glancing_hits++;
            } else {
                resolution.misses++;
            }
            used_right_hand = true;
        }

        if (target->hp <= 0) {
            break;
        }

        if (d20_get_item_type(attacker, SLOT_HAND_LEFT) != ITEM_TYPE_SHIELD &&
            (d20_is_dual_wielding(attacker) || !used_right_hand)) {
            damage = d20_perform_single_attack(attacker, target, SLOT_HAND_LEFT, attack_bonus - (i * 5));
            resolution.any_attack_attempted = true;
            if (damage > 0) {
                resolution.total_damage += damage;
                resolution.successful_hits++;
            } else if (damage == 0) {
                resolution.glancing_hits++;
            } else {
                resolution.misses++;
            }
        }
    }

    if (target->hp < 0) {
        target->hp = 0;
    }
    return resolution;
}

static void u2_handle_player_defeat(void) {
    u2_restore_player_fully();
    u2_load_scene("castleBritish", 31, 14);
    u2_session_show_message(
        &game_state.session,
        false,
        U2_LOG_MESSAGE_DURATION,
        "You collapse. You are carried to Castle British and restored to full strength."
    );
}

static void u2_handle_hostile_defeat(U2Entity* entity) {
    char level_up_message[U2_MESSAGE_TEXT_MAX];
    const int reward_gold = u2_get_hostile_reward_gold(entity);
    const int reward_exp = u2_get_hostile_reward_exp(entity);

    if (entity == NULL) {
        return;
    }

    if (entity->id != NULL) {
        u2_mark_persistent_entity_inactive(entity->id);
    }

    game_state.session.gold += reward_gold;
    u2_award_player_exp(reward_exp, level_up_message, sizeof(level_up_message));
    u2_entity_reset(entity);

    if (level_up_message[0] != '\0') {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s", level_up_message);
    }
}

static void u2_resolve_hostile_bump_attack(U2Entity* entity) {
    U2AttackResolution resolution = { 0 };

    if (!u2_is_hostile_entity(entity)) {
        return;
    }

    resolution = u2_perform_attack_sequence(&entity->sheet, &game_state.session.player.sheet);
    u2_sync_persistent_entity_state_from_entity(entity);
    u2_apply_attack_feedback_to_player(&resolution);

    if (game_state.session.player.sheet.hp <= 0) {
        u2_handle_player_defeat();
    }
}

static void u2_resolve_player_bump_attack(U2Entity* entity) {
    U2AttackResolution resolution = { 0 };

    if (!u2_is_hostile_entity(entity)) {
        return;
    }

    resolution = u2_perform_attack_sequence(&game_state.session.player.sheet, &entity->sheet);
    u2_apply_attack_feedback_to_entity(entity, &resolution);

    if (entity->sheet.hp <= 0) {
        u2_handle_hostile_defeat(entity);
        return;
    }

    u2_sync_persistent_entity_state_from_entity(entity);
}

static int u2_get_axis_delta_toward_target(int from, int to, int size, bool looped) {
    int delta = to - from;

    if (looped && size > 0) {
        if (delta > size / 2) {
            delta -= size;
        } else if (delta < -(size / 2)) {
            delta += size;
        }
    }

    return delta;
}

static bool u2_is_monster_walkable_tile(const U2TileDef* tile) {
    if (tile == NULL || !tile->passable) {
        return false;
    }

    return strcmp(tile->name, "town") != 0 &&
        strcmp(tile->name, "castle") != 0 &&
        strcmp(tile->name, "village") != 0 &&
        strcmp(tile->name, "portal") != 0 &&
        strcmp(tile->name, "dungeon") != 0 &&
        strcmp(tile->name, "tower") != 0 &&
        strcmp(tile->name, "signpost") != 0;
}

static bool u2_is_within_entity_roam_radius(const U2Entity* entity, int target_x, int target_y) {
    int delta_x = 0;
    int delta_y = 0;

    if (entity == NULL || !entity->roaming || entity->roam_radius <= 0 || game_state.session.current_map == NULL) {
        return true;
    }

    delta_x = u2_get_axis_delta_toward_target(
        entity->spawn_tile_x,
        target_x,
        game_state.session.current_map->width,
        u2_scene_is_looped()
    );
    delta_y = u2_get_axis_delta_toward_target(
        entity->spawn_tile_y,
        target_y,
        game_state.session.current_map->height,
        u2_scene_is_looped()
    );

    return abs(delta_x) <= entity->roam_radius && abs(delta_y) <= entity->roam_radius;
}

static bool u2_try_move_hostile_entity(U2Entity* entity, int dx, int dy) {
    int target_x = 0;
    int target_y = 0;
    const U2TileDef* target_tile = NULL;
    U2Entity* blocking_entity = NULL;

    if (!u2_is_hostile_entity(entity) || game_state.session.current_map == NULL || (dx == 0 && dy == 0)) {
        return false;
    }

    target_x = entity->tile_x + dx;
    target_y = entity->tile_y + dy;

    if (u2_scene_is_looped()) {
        target_x = u2_entity_wrap_coord(target_x, game_state.session.current_map->width);
        target_y = u2_entity_wrap_coord(target_y, game_state.session.current_map->height);
    } else if (target_x < 0 || target_y < 0 || target_x >= game_state.session.current_map->width || target_y >= game_state.session.current_map->height) {
        return false;
    }

    target_tile = u2_get_tile_at(game_state.session.current_map, target_x, target_y);
    if (!u2_is_monster_walkable_tile(target_tile) || !u2_is_within_entity_roam_radius(entity, target_x, target_y)) {
        return false;
    }

    if (game_state.session.player.tile_x == target_x && game_state.session.player.tile_y == target_y) {
        if (dx < 0) entity->facing = U2_FACING_WEST;
        if (dx > 0) entity->facing = U2_FACING_EAST;
        if (dy < 0) entity->facing = U2_FACING_NORTH;
        if (dy > 0) entity->facing = U2_FACING_SOUTH;
        u2_resolve_hostile_bump_attack(entity);
        return true;
    }

    blocking_entity = u2_find_scene_entity_at_excluding(target_x, target_y, entity);
    if (blocking_entity != NULL && blocking_entity->solid) {
        return false;
    }

    u2_entity_move_by(entity, game_state.session.current_map, dx, dy);
    u2_sync_persistent_entity_state_from_entity(entity);
    return true;
}

static void u2_update_hostile_entity(U2Entity* entity) {
    static const int directions[4][2] = {
        { -1, 0 },
        { 1, 0 },
        { 0, -1 },
        { 0, 1 },
    };
    int delta_x = 0;
    int delta_y = 0;
    int primary_dx = 0;
    int primary_dy = 0;
    int secondary_dx = 0;
    int secondary_dy = 0;
    const bool looped = u2_scene_is_looped();
    int random_start = 0;

    if (!u2_is_hostile_entity(entity) || !entity->roaming || game_state.session.current_map == NULL) {
        return;
    }

    delta_x = u2_get_axis_delta_toward_target(
        entity->tile_x,
        game_state.session.player.tile_x,
        game_state.session.current_map->width,
        looped
    );
    delta_y = u2_get_axis_delta_toward_target(
        entity->tile_y,
        game_state.session.player.tile_y,
        game_state.session.current_map->height,
        looped
    );

    if (abs(delta_x) + abs(delta_y) <= U2_MONSTER_CHASE_RADIUS) {
        if (abs(delta_x) >= abs(delta_y)) {
            primary_dx = sign(int, delta_x);
            secondary_dy = sign(int, delta_y);
        } else {
            primary_dy = sign(int, delta_y);
            secondary_dx = sign(int, delta_x);
        }

        if (u2_try_move_hostile_entity(entity, primary_dx, primary_dy)) {
            return;
        }

        if (u2_try_move_hostile_entity(entity, secondary_dx, secondary_dy)) {
            return;
        }
    } else if (!coin_flip(60)) {
        return;
    }

    random_start = randi_range(0, 3);
    for (int i = 0; i < 4; ++i) {
        const int index = (random_start + i) % 4;
        if (u2_try_move_hostile_entity(entity, directions[index][0], directions[index][1])) {
            return;
        }
    }
}

static void u2_take_hostile_turn(void) {
    const U2MapDef* starting_map = game_state.session.current_map;

    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        if (starting_map == NULL || game_state.session.current_map != starting_map) {
            return;
        }
        u2_update_hostile_entity(&game_state.session.scene_entities[i]);
    }
}

static int u2_get_service_option_count(const U2ServiceDef* service) {
    if (service == NULL) {
        return 0;
    }

    switch (service->kind) {
        case U2_SERVICE_SHOP:
            return u2_get_shop_stock_count(service->entity_id) + 1;
        case U2_SERVICE_INN:
            return 2;
        case U2_SERVICE_HEALER:
            return 3;
        case U2_SERVICE_SANCTUARY:
            return 4;
        case U2_SERVICE_NONE:
        default:
            return 1;
    }
}

static void u2_try_buy_shop_stock(const U2ShopStockDef* stock) {
    Item* item = NULL;
    const char* label = NULL;

    if (stock == NULL) {
        return;
    }

    label = stock->label;
    if (label == NULL && stock->item_id != NULL) {
        item = items_get(stock->item_id);
        if (item != NULL) {
            label = item->name;
        }
    }

    if (label == NULL) {
        label = "goods";
    }

    if (game_state.session.gold < stock->price) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You need %d gold for %s.", stock->price, label);
        return;
    }

    if (stock->item_id != NULL) {
        if (item == NULL) {
            item = items_get(stock->item_id);
        }
        if (item == NULL) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "That stock entry is missing.");
            return;
        }
        if (!u2_inventory_add(&game_state.session.inventory, item, 1)) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your pack is too full for %s.", label);
            return;
        }
    }

    game_state.session.gold -= stock->price;
    if (stock->food_value > 0) {
        game_state.session.food += stock->food_value;
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Purchased %s. Food +%d.", label, stock->food_value);
    } else {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Purchased %s.", label);
    }
}

static void u2_execute_service_action(void) {
    const U2ServiceDef* service = u2_get_active_service_def();
    Character* player = &game_state.session.player.sheet;

    if (service == NULL) {
        u2_close_service_panel();
        return;
    }

    switch (service->kind) {
        case U2_SERVICE_SHOP:
        {
            const int stock_count = u2_get_shop_stock_count(service->entity_id);
            if (game_state.session.service_selection_index >= stock_count) {
                u2_close_service_panel();
                return;
            }
            u2_try_buy_shop_stock(u2_get_shop_stock_by_index(service->entity_id, game_state.session.service_selection_index));
            return;
        }
        case U2_SERVICE_INN:
            if (game_state.session.service_selection_index == 1) {
                u2_close_service_panel();
                return;
            }
            if (game_state.session.gold < service->primary_cost) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The innkeeper asks for %d gold.", service->primary_cost);
                return;
            }
            if (player->hp == d20_get_max_hp(player) && player->mp == d20_get_max_mp(player)) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You already feel well rested.");
                return;
            }
            game_state.session.gold -= service->primary_cost;
            u2_restore_player_fully();
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You rest soundly and recover fully.");
            return;
        case U2_SERVICE_HEALER:
            if (game_state.session.service_selection_index == 2) {
                u2_close_service_panel();
                return;
            }
            if (game_state.session.service_selection_index == 0) {
                if (game_state.session.gold < service->primary_cost) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The healer asks for %d gold.", service->primary_cost);
                    return;
                }
                if (player->hp == d20_get_max_hp(player)) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your wounds are already mended.");
                    return;
                }
                game_state.session.gold -= service->primary_cost;
                u2_restore_player_hp();
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your wounds are treated.");
                return;
            }
            if (game_state.session.gold < service->secondary_cost) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The healer asks for %d gold.", service->secondary_cost);
                return;
            }
            if (player->mp == d20_get_max_mp(player)) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your spirit is already steady.");
                return;
            }
            game_state.session.gold -= service->secondary_cost;
            u2_restore_player_mp();
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your mind feels clear again.");
            return;
        case U2_SERVICE_SANCTUARY:
            if (game_state.session.service_selection_index == 3) {
                u2_close_service_panel();
                return;
            }
            if (game_state.session.service_selection_index == 0) {
                if (game_state.session.gold < service->primary_cost) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Royal hospitality costs %d gold today.", service->primary_cost);
                    return;
                }
                if (player->hp == d20_get_max_hp(player) && player->mp == d20_get_max_mp(player)) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You are already at full strength.");
                    return;
                }
                game_state.session.gold -= service->primary_cost;
                u2_restore_player_fully();
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You leave the sanctuary restored.");
                return;
            }
            if (game_state.session.service_selection_index == 1) {
                if (game_state.session.gold < service->secondary_cost) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "A royal physician asks for %d gold.", service->secondary_cost);
                    return;
                }
                if (player->hp == d20_get_max_hp(player)) {
                    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You do not need healing.");
                    return;
                }
                game_state.session.gold -= service->secondary_cost;
                u2_restore_player_hp();
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your wounds are seen to.");
                return;
            }
            if (game_state.session.gold < service->tertiary_cost) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The sanctuary tithe is %d gold.", service->tertiary_cost);
                return;
            }
            if (player->mp == d20_get_max_mp(player)) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your spirit is already renewed.");
                return;
            }
            game_state.session.gold -= service->tertiary_cost;
            u2_restore_player_mp();
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your spirit is renewed.");
            return;
        case U2_SERVICE_NONE:
        default:
            u2_close_service_panel();
            return;
    }
}

static void u2_interact_with_entity(const U2Entity* entity) {
    const U2ServiceDef* service = u2_find_service_def(entity->id);
    const U2DialogueDef* dialogue = u2_find_dialogue(entity->dialogue_id);

    if (u2_is_hostile_entity(entity)) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Move into %s to attack.", entity->name);
        return;
    }

    if (service != NULL) {
        u2_open_service_panel(entity);
        return;
    }

    if (dialogue != NULL) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s", dialogue->text);
        return;
    }

    if (entity->interaction_kind == U2_INTERACTION_SIGN) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The sign offers nothing useful.");
    } else {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s has nothing to say.", entity->name);
    }
}

static bool u2_try_interact_forward(void) {
    int dx = 0;
    int dy = 0;
    int target_x = 0;
    int target_y = 0;
    U2Entity* entity = NULL;
    const U2TileDef* front_tile = NULL;

    u2_get_facing_delta(game_state.session.player.facing, &dx, &dy);
    if (dx == 0 && dy == 0 || game_state.session.current_map == NULL) {
        return false;
    }

    if (!u2_get_offset_tile(game_state.session.player.tile_x, game_state.session.player.tile_y, dx, dy, 1, &target_x, &target_y)) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Nothing there responds.");
        return false;
    }

    entity = u2_find_scene_entity_at(target_x, target_y);
    if (entity != NULL && (entity->interaction_kind != U2_INTERACTION_NONE || u2_is_hostile_entity(entity))) {
        u2_interact_with_entity(entity);
        return true;
    }

    front_tile = u2_get_tile_at(game_state.session.current_map, target_x, target_y);
    if (u2_is_counter_tile(front_tile)) {
        int beyond_x = 0;
        int beyond_y = 0;

        if (u2_get_offset_tile(game_state.session.player.tile_x, game_state.session.player.tile_y, dx, dy, 2, &beyond_x, &beyond_y)) {
            entity = u2_find_scene_entity_at(beyond_x, beyond_y);
            if (entity != NULL && (entity->interaction_kind != U2_INTERACTION_NONE || u2_is_hostile_entity(entity))) {
                u2_interact_with_entity(entity);
                return true;
            }
        }
    }

    if (front_tile != NULL) {
        const char* placeholder_message = u2_get_unimplemented_message(front_tile);
        if (placeholder_message != NULL) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s", placeholder_message);
            return true;
        }
    }

    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Nothing there responds.");
    return false;
}

static bool u2_get_entity_screen_position(const U2Entity* entity, int* out_x, int* out_y) {
    if (!u2_entity_is_valid(entity) || game_state.session.current_map == NULL) {
        return false;
    }

    const int camera_px_x = (int)floorf(game_state.camera_x);
    const int camera_px_y = (int)floorf(game_state.camera_y);
    int screen_x = (entity->tile_x * U2_TILE_SIZE) - camera_px_x;
    int screen_y = (entity->tile_y * U2_TILE_SIZE) - camera_px_y;

    if (u2_scene_is_looped()) {
        while (screen_x < -U2_TILE_SIZE) {
            screen_x += u2_current_map_pixel_width();
        }
        while (screen_x >= u2_world_view_width()) {
            screen_x -= u2_current_map_pixel_width();
        }
        while (screen_y < -U2_TILE_SIZE) {
            screen_y += u2_current_map_pixel_height();
        }
        while (screen_y >= u2_world_view_height()) {
            screen_y -= u2_current_map_pixel_height();
        }
    }

    if (screen_x <= -U2_TILE_SIZE || screen_x >= u2_world_view_width() || screen_y <= -U2_TILE_SIZE || screen_y >= u2_world_view_height()) {
        return false;
    }

    *out_x = screen_x;
    *out_y = screen_y;
    return true;
}

static bool u2_get_tile_center_screen_position(float tile_x, float tile_y, int* out_x, int* out_y) {
    float screen_x = tile_x * U2_TILE_SIZE + (U2_TILE_SIZE * 0.5f) - game_state.camera_x;
    float screen_y = tile_y * U2_TILE_SIZE + (U2_TILE_SIZE * 0.5f) - game_state.camera_y;

    if (game_state.session.current_map == NULL) {
        return false;
    }

    if (u2_scene_is_looped()) {
        while (screen_x < -U2_TILE_SIZE) {
            screen_x += u2_current_map_pixel_width();
        }
        while (screen_x >= u2_world_view_width() + U2_TILE_SIZE) {
            screen_x -= u2_current_map_pixel_width();
        }
        while (screen_y < -U2_TILE_SIZE) {
            screen_y += u2_current_map_pixel_height();
        }
        while (screen_y >= u2_world_view_height() + U2_TILE_SIZE) {
            screen_y -= u2_current_map_pixel_height();
        }
    }

    if (screen_x < -U2_TILE_SIZE || screen_x >= u2_world_view_width() + U2_TILE_SIZE ||
        screen_y < -U2_TILE_SIZE || screen_y >= u2_world_view_height() + U2_TILE_SIZE) {
        return false;
    }

    *out_x = (int)roundf(screen_x);
    *out_y = (int)roundf(screen_y);
    return true;
}

static bool u2_is_flash_currently_visible(const U2HitFlashState* flash) {
    const int phase = (int)floorf(flash->timer / U2_HIT_BLINK_INTERVAL);
    return (phase % 2) == 0;
}

static bool u2_should_draw_entity_sprite(const U2Entity* entity) {
    if (!u2_entity_is_valid(entity) || entity->id == NULL) {
        return false;
    }

    for (int i = 0; i < U2_MAX_HIT_FLASHES; ++i) {
        const U2HitFlashState* flash = &game_state.hit_flashes[i];
        if (!flash->active || flash->target_player || flash->entity_id[0] == '\0') {
            continue;
        }
        if (strcmp(flash->entity_id, entity->id) == 0) {
            return u2_is_flash_currently_visible(flash);
        }
    }

    return true;
}

static bool u2_should_draw_player_sprite(void) {
    for (int i = 0; i < U2_MAX_HIT_FLASHES; ++i) {
        const U2HitFlashState* flash = &game_state.hit_flashes[i];
        if (flash->active && flash->target_player) {
            return u2_is_flash_currently_visible(flash);
        }
    }

    return true;
}

static uint32_t u2_get_damage_popup_color(const U2DamagePopupState* popup) {
    return COLOR_DARKGRAY;
}

static void u2_draw_damage_popups(void) {
    for (int i = 0; i < U2_MAX_DAMAGE_POPUPS; ++i) {
        const U2DamagePopupState* popup = &game_state.damage_popups[i];
        const float progress = popup->duration > 0.0f ? popup->timer / popup->duration : 1.0f;
        const float rise_pixels = progress * U2_DAMAGE_POPUP_RISE_PIXELS;
        int screen_x = 0;
        int screen_y = 0;

        if (!popup->active) {
            continue;
        }

        if (!u2_get_tile_center_screen_position(popup->tile_x, popup->tile_y, &screen_x, &screen_y)) {
            continue;
        }

        ascii_font_draw_ex(
            chao_canvas,
            font,
            screen_x,
            screen_y - (int)roundf(rise_pixels) - 10,
            u2_get_damage_popup_color(popup),
            TEXT_ALIGN_CENTER,
            -1,
            "%s",
            popup->text
        );
    }
}

static void u2_draw_world(void) {
    if (game_state.session.current_map == NULL) {
        return;
    }

    const int camera_px_x = (int)floorf(game_state.camera_x);
    const int camera_px_y = (int)floorf(game_state.camera_y);
    const int start_tile_x = camera_px_x / U2_TILE_SIZE;
    const int start_tile_y = camera_px_y / U2_TILE_SIZE;
    const int tile_offset_x = -(camera_px_x % U2_TILE_SIZE);
    const int tile_offset_y = -(camera_px_y % U2_TILE_SIZE);
    const int visible_tiles_x = (u2_world_view_width() / U2_TILE_SIZE) + 2;
    const int visible_tiles_y = (u2_world_view_height() / U2_TILE_SIZE) + 2;

    for (int y = 0; y < visible_tiles_y; ++y) {
        int map_y = start_tile_y + y;
        if (u2_scene_is_looped()) {
            map_y = u2_entity_wrap_coord(map_y, game_state.session.current_map->height);
        } else if (map_y < 0 || map_y >= game_state.session.current_map->height) {
            continue;
        }

        const char* row = game_state.session.current_map->rows[map_y];
        const int draw_y = tile_offset_y + (y * U2_TILE_SIZE);

        for (int x = 0; x < visible_tiles_x; ++x) {
            int map_x = start_tile_x + x;
            if (u2_scene_is_looped()) {
                map_x = u2_entity_wrap_coord(map_x, game_state.session.current_map->width);
            } else if (map_x < 0 || map_x >= game_state.session.current_map->width) {
                continue;
            }

            const unsigned char symbol = (unsigned char)row[map_x];
            u2_draw_tile(game_state.tile_lookup[symbol], tile_offset_x + (x * U2_TILE_SIZE), draw_y);
        }
    }
}

static void u2_draw_scene_entities(void) {
    for (int i = 0; i < U2_MAX_SCENE_ENTITIES; ++i) {
        U2Entity* entity = &game_state.session.scene_entities[i];
        int draw_x = 0;
        int draw_y = 0;

        if (u2_get_entity_screen_position(entity, &draw_x, &draw_y) && u2_should_draw_entity_sprite(entity)) {
            u2_draw_tile(entity->tile, draw_x, draw_y);
        }
    }

    {
        int player_x = 0;
        int player_y = 0;
        if (u2_get_entity_screen_position(&game_state.session.player, &player_x, &player_y) && u2_should_draw_player_sprite()) {
            u2_draw_tile(game_state.session.player.tile, player_x, player_y);
        }
    }
}

static void u2_draw_spell_projectile(void) {
    const U2SpellProjectileState* projectile = &game_state.session.spell_projectile;
    const float traveled_steps = projectile->travel_steps * projectile->progress;
    const float tile_x = projectile->start_tile_x + projectile->dir_x * traveled_steps;
    const float tile_y = projectile->start_tile_y + projectile->dir_y * traveled_steps;
    int screen_x = 0;
    int screen_y = 0;

    if (!projectile->active) {
        return;
    }

    if (!u2_get_tile_center_screen_position(tile_x, tile_y, &screen_x, &screen_y)) {
        return;
    }

    fill_circle(chao_canvas, screen_x, screen_y, 4, COLOR_WHITE);
}

static void u2_draw_hud(void) {
    const int x = u2_world_view_width() + 4;
    const int y = 4;
    const int w = U2_HUD_WIDTH - 8;
    const int h = screen_size.y - 8;
    const U2ClassTemplate* class_template = u2_find_class_template(game_state.session.chosen_class);

    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 10, y + 10, COLOR_WHITE, "%s", game_state.session.player_name);
    ascii_font_draw(chao_canvas, font, x + 10, y + 22, COLOR_LIGHTGRAY, "%s", class_template != NULL ? class_template->name : "None");
    ascii_font_draw(chao_canvas, font, x + 10, y + 34, COLOR_WHITE, "LV %d", game_state.session.player.sheet.level);
    ascii_font_draw(chao_canvas, font, x + 10, y + 46, COLOR_WHITE, "HP %d/%d", game_state.session.player.sheet.hp, d20_get_max_hp(&game_state.session.player.sheet));
    ascii_font_draw(chao_canvas, font, x + 10, y + 58, COLOR_WHITE, "MP %d/%d", game_state.session.player.sheet.mp, d20_get_max_mp(&game_state.session.player.sheet));
    ascii_font_draw(chao_canvas, font, x + 10, y + 74, COLOR_WHITE, "G %d", game_state.session.gold);
    ascii_font_draw(chao_canvas, font, x + 10, y + 86, u2_get_food_color(), "F %d", game_state.session.food);
    ascii_font_draw(chao_canvas, font, x + 10, y + 102, COLOR_LIGHTGRAY, "STR %d", game_state.session.player.sheet.stats[STAT_STR]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 114, COLOR_LIGHTGRAY, "DEX %d", game_state.session.player.sheet.stats[STAT_DEX]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 126, COLOR_LIGHTGRAY, "MND %d", game_state.session.player.sheet.stats[STAT_MIND]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 142, COLOR_WHITE, "Area:");
    if (game_state.session.current_map != NULL) {
        u2_draw_wrapped_text(x + 10, y + 154, w - 20, COLOR_LIGHTGRAY, game_state.session.current_map->name);
    }
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 52, COLOR_YELLOW, "F5 Save");
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 40, COLOR_YELLOW, "F9 Load");
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 28, COLOR_YELLOW, "C/I Info");
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 16, COLOR_YELLOW, "M Magic");
}

static void u2_draw_message_log(void) {
    if (!game_state.session.message.visible || game_state.session.message.text[0] == '\0') {
        return;
    }

    if (game_state.session.message.modal) {
        const int w = 220;
        const int text_width = w - 20;
        const Vector2Int text_size = u2_get_wrapped_text_size(text_width, game_state.session.message.text);
        const int h = max(80, text_size.y + 34);
        const int x = (screen_size.x - w) / 2;
        const int y = (screen_size.y - h) / 2;
        char visible_text[U2_MESSAGE_TEXT_MAX];
        u2_get_revealed_wrapped_message_text(text_width, visible_text, sizeof(visible_text));
        u2_draw_panel(x, y, w, h);
        ascii_font_draw(chao_canvas, font, x + 10, y + 12, COLOR_WHITE, "%s", visible_text);
        ascii_font_draw(chao_canvas, font, x + 10, y + h - 18, COLOR_YELLOW, "Enter closes");
        return;
    }

    {
        const int x = 4;
        const int w = u2_world_view_width() - 8;
        const int text_width = w - 20;
        const Vector2Int text_size = u2_get_wrapped_text_size(text_width, game_state.session.message.text);
        const int h = max(U2_LOG_HEIGHT, text_size.y + 20);
        int player_screen_x = 0;
        int player_screen_y = 0;
        int y = screen_size.y - h - 4;
        char visible_text[U2_MESSAGE_TEXT_MAX];

        if (u2_get_entity_screen_position(&game_state.session.player, &player_screen_x, &player_screen_y)) {
            const int player_center_y = player_screen_y + (U2_TILE_SIZE / 2);
            if (player_center_y >= (u2_world_view_height() * 2) / 3) {
                y = 4;
            }
        }

        u2_get_revealed_wrapped_message_text(text_width, visible_text, sizeof(visible_text));
        u2_draw_panel(x, y, w, h);
        ascii_font_draw(chao_canvas, font, x + 10, y + 10, COLOR_WHITE, "%s", visible_text);
    }
}

static const char* u2_get_equipped_item_name(Slot slot) {
    Item* item = game_state.session.player.sheet.equipment[slot];
    return item != NULL ? item->name : "-";
}

static void u2_get_inventory_item_label(const ItemSlot* slot, char* dst, size_t dst_size) {
    const Item* item = NULL;

    if (dst == NULL || dst_size == 0) {
        return;
    }

    dst[0] = '\0';
    if (slot == NULL || slot->item == NULL) {
        return;
    }

    item = slot->item;
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            snprintf(dst, dst_size, "%s [%s]", item->name, item->damage != NULL ? item->damage : "-");
            break;
        case ITEM_TYPE_SHIELD:
        case ITEM_TYPE_ARMOR:
            snprintf(dst, dst_size, "%s [AC +%d]", item->name, item->armor_class);
            break;
        default:
            snprintf(dst, dst_size, "%s", item->name);
            break;
    }
}

static int u2_get_inventory_first_visible_index(int visible_rows) {
    int first = game_state.session.inventory_selection_index - visible_rows + 1;
    if (game_state.session.inventory_selection_index < visible_rows) {
        first = 0;
    }
    return clamp(first, 0, max(0, game_state.session.inventory.count - visible_rows));
}

static void u2_clamp_inventory_selection(void) {
    game_state.session.inventory_selection_index = clamp(
        game_state.session.inventory_selection_index,
        0,
        max(0, game_state.session.inventory.count - 1)
    );
}

static const char* u2_get_inventory_action_label(const Item* item) {
    Slot equip_slot = SLOT_END;

    if (u2_get_item_equip_slot(item, &equip_slot)) {
        return "Equip";
    }
    if (item != NULL && strcmp(item->id, "spellbook") == 0) {
        return "Read";
    }
    if (u2_item_is_consumable(item)) {
        return "Use";
    }
    return "Inspect";
}

static bool u2_try_enter_transition_at_player_position(void) {
    const U2TransitionDef* transition = NULL;

    if (!u2_scene_is_looped() || game_state.session.current_map == NULL) {
        return false;
    }

    transition = u2_find_transition(
        game_state.session.current_map->id,
        game_state.session.player.tile_x,
        game_state.session.player.tile_y
    );
    if (transition == NULL) {
        return false;
    }

    u2_load_scene(transition->destination_map_id, transition->destination_x, transition->destination_y);
    u2_session_show_message(
        &game_state.session,
        false,
        U2_LOG_MESSAGE_DURATION,
        "Entered %s.",
        game_state.session.current_map->name
    );
    return true;
}

static bool u2_get_spell_travel_result(
    int start_x,
    int start_y,
    int dx,
    int dy,
    int range,
    int* out_end_x,
    int* out_end_y,
    int* out_travel_steps,
    char* out_target_entity_id,
    size_t out_target_entity_id_size
) {
    bool found_any_tile = false;

    if (out_end_x == NULL || out_end_y == NULL || out_travel_steps == NULL || out_target_entity_id == NULL || out_target_entity_id_size == 0) {
        return false;
    }

    out_target_entity_id[0] = '\0';

    for (int distance = 1; distance <= range; ++distance) {
        int tile_x = 0;
        int tile_y = 0;
        U2Entity* entity = NULL;
        const U2TileDef* tile = NULL;

        if (!u2_get_offset_tile(start_x, start_y, dx, dy, distance, &tile_x, &tile_y)) {
            break;
        }

        found_any_tile = true;
        *out_end_x = tile_x;
        *out_end_y = tile_y;
        *out_travel_steps = distance;

        entity = u2_find_scene_entity_at(tile_x, tile_y);
        if (u2_is_hostile_entity(entity)) {
            u2_copy_string(out_target_entity_id, out_target_entity_id_size, entity->id);
            return true;
        }

        tile = u2_get_tile_at(game_state.session.current_map, tile_x, tile_y);
        if (tile == NULL || !tile->passable || (entity != NULL && entity->solid)) {
            return true;
        }
    }

    return found_any_tile;
}

static void u2_begin_spell_projectile(
    const U2SpellDef* spell,
    int dx,
    int dy,
    int travel_steps,
    int damage,
    const char* target_entity_id
) {
    U2SpellProjectileState* projectile = &game_state.session.spell_projectile;

    u2_session_clear_spell_targeting(&game_state.session);
    u2_session_clear_message(&game_state.session);
    u2_session_clear_spell_projectile(&game_state.session);

    projectile->active = true;
    projectile->spell_id = spell->id;
    projectile->start_tile_x = game_state.session.player.tile_x;
    projectile->start_tile_y = game_state.session.player.tile_y;
    projectile->dir_x = dx;
    projectile->dir_y = dy;
    projectile->travel_steps = max(1, travel_steps);
    projectile->damage = damage;
    projectile->progress = 0.0f;
    projectile->duration = max(0.12f, 0.05f * (float)projectile->travel_steps);
    u2_copy_string(projectile->target_entity_id, sizeof(projectile->target_entity_id), target_entity_id);
}

static void u2_finish_spell_projectile(void) {
    U2SpellProjectileState projectile = game_state.session.spell_projectile;
    const U2SpellDef* spell = u2_get_active_projectile_spell();

    u2_session_clear_spell_projectile(&game_state.session);

    if (spell == NULL) {
        return;
    }

    if (projectile.target_entity_id[0] != '\0') {
        U2Entity* target = u2_find_scene_entity_by_id(projectile.target_entity_id);
        char damage_text[U2_DAMAGE_POPUP_TEXT_MAX];

        if (u2_is_hostile_entity(target)) {
            target->sheet.hp = max(0, target->sheet.hp - projectile.damage);
            u2_spawn_hit_flash(target, false);
            snprintf(damage_text, sizeof(damage_text), "%d", projectile.damage);
            u2_spawn_entity_damage_popup(target, COLOR_WHITE, damage_text);
            if (target->sheet.hp <= 0) {
                u2_handle_hostile_defeat(target);
            } else {
                u2_sync_persistent_entity_state_from_entity(target);
            }
            u2_take_hostile_turn();
            return;
        }
    }

    u2_take_hostile_turn();
}

static void u2_update_spell_projectile(float dt) {
    U2SpellProjectileState* projectile = &game_state.session.spell_projectile;

    if (!projectile->active) {
        return;
    }

    projectile->progress += dt / max(0.01f, projectile->duration);
    if (projectile->progress >= 1.0f) {
        projectile->progress = 1.0f;
        u2_finish_spell_projectile();
    }
}

static bool u2_try_launch_directional_spell(const U2SpellDef* spell, int dx, int dy) {
    int end_x = 0;
    int end_y = 0;
    int travel_steps = 0;
    char target_entity_id[32];
    const int damage = max(1, roll_dice(spell->power_dice) + d20_get_stat_bonus(&game_state.session.player.sheet, STAT_MIND) + max(0, game_state.session.player.sheet.level - 1) / 2);

    if (!u2_get_spell_travel_result(
        game_state.session.player.tile_x,
        game_state.session.player.tile_y,
        dx,
        dy,
        spell->range,
        &end_x,
        &end_y,
        &travel_steps,
        target_entity_id,
        sizeof(target_entity_id)
    )) {
        u2_session_show_message(&game_state.session, false, 0.0f, "There is no room to cast %s that way. Choose another direction.", spell->name);
        return false;
    }

    (void)end_x;
    (void)end_y;
    game_state.session.player.sheet.mp -= spell->mp_cost;
    u2_clamp_player_resources();
    u2_begin_spell_projectile(spell, dx, dy, travel_steps, damage, target_entity_id);
    return true;
}

static bool u2_try_cast_targeting_spell_in_direction(int dx, int dy) {
    const U2SpellDef* spell = u2_get_targeting_spell();

    if (spell == NULL) {
        u2_session_clear_spell_targeting(&game_state.session);
        return false;
    }

    switch (spell->effect) {
        case U2_SPELL_DAMAGE_BOLT:
            return u2_try_launch_directional_spell(spell, dx, dy);
        default:
            u2_session_clear_spell_targeting(&game_state.session);
            return false;
    }
}

static bool u2_try_cast_spell(const U2SpellDef* spell) {
    Character* player = &game_state.session.player.sheet;

    if (spell == NULL) {
        return false;
    }

    if (player->mp < spell->mp_cost) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You need %d MP to cast %s.", spell->mp_cost, spell->name);
        return false;
    }

    switch (spell->effect) {
        case U2_SPELL_DAMAGE_BOLT:
            u2_begin_spell_targeting(spell);
            return true;
        case U2_SPELL_HEAL_SELF:
        {
            const int max_hp = d20_get_max_hp(player);
            const int heal_amount = max(1, roll_dice(spell->power_dice) + d20_get_stat_bonus(player, STAT_MIND));
            const int previous_hp = player->hp;

            if (player->hp >= max_hp) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You do not need %s right now.", spell->name);
                return false;
            }

            player->mp -= spell->mp_cost;
            player->hp = min(max_hp, player->hp + heal_amount);
            game_state.session.panel = U2_PANEL_NONE;
            u2_session_show_message(
                &game_state.session,
                false,
                U2_LOG_MESSAGE_DURATION,
                "%s restores %d HP.",
                spell->name,
                player->hp - previous_hp
            );
            u2_clamp_player_resources();
            u2_take_hostile_turn();
            return true;
        }
        case U2_SPELL_BLINK_FORWARD:
        {
            int dx = 0;
            int dy = 0;
            int best_x = game_state.session.player.tile_x;
            int best_y = game_state.session.player.tile_y;
            bool found_destination = false;

            u2_get_facing_delta(game_state.session.player.facing, &dx, &dy);
            for (int distance = 1; distance <= spell->range; ++distance) {
                int tile_x = 0;
                int tile_y = 0;
                const U2TileDef* tile = NULL;
                U2Entity* blocking_entity = NULL;

                if (!u2_get_offset_tile(game_state.session.player.tile_x, game_state.session.player.tile_y, dx, dy, distance, &tile_x, &tile_y)) {
                    break;
                }

                tile = u2_get_tile_at(game_state.session.current_map, tile_x, tile_y);
                blocking_entity = u2_find_scene_entity_at(tile_x, tile_y);
                if (tile == NULL || !tile->passable || (blocking_entity != NULL && blocking_entity->solid)) {
                    break;
                }

                best_x = tile_x;
                best_y = tile_y;
                found_destination = true;
            }

            if (!found_destination) {
                u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "There is nowhere safe to land.");
                return false;
            }

            player->mp -= spell->mp_cost;
            game_state.session.player.tile_x = best_x;
            game_state.session.player.tile_y = best_y;
            game_state.session.panel = U2_PANEL_NONE;
            if (u2_try_enter_transition_at_player_position()) {
                u2_clamp_player_resources();
                return true;
            }
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s carries you onward.", spell->name);
            u2_clamp_player_resources();
            u2_take_hostile_turn();
            return true;
        }
        default:
            return false;
    }
}

static bool u2_try_equip_inventory_item(int index) {
    ItemSlot* inventory_slot = NULL;
    Item* new_item = NULL;
    Item* previous_item = NULL;
    Slot equip_slot = SLOT_END;

    if (index < 0 || index >= game_state.session.inventory.count) {
        return false;
    }

    inventory_slot = &game_state.session.inventory.slots[index];
    if (inventory_slot->item == NULL || !u2_get_item_equip_slot(inventory_slot->item, &equip_slot)) {
        return false;
    }

    new_item = inventory_slot->item;
    previous_item = game_state.session.player.sheet.equipment[equip_slot];
    if (previous_item == new_item) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s is already equipped.", new_item->name);
        return false;
    }

    if (!u2_inventory_remove_at(&game_state.session.inventory, index, 1)) {
        return false;
    }

    game_state.session.player.sheet.equipment[equip_slot] = new_item;
    if (previous_item != NULL) {
        u2_inventory_add(&game_state.session.inventory, previous_item, 1);
    }

    u2_clamp_inventory_selection();
    game_state.session.panel = U2_PANEL_NONE;
    u2_clamp_player_resources();
    u2_session_show_message(
        &game_state.session,
        false,
        U2_LOG_MESSAGE_DURATION,
        "Equipped %s in %s.",
        new_item->name,
        u2_slot_name(equip_slot)
    );
    u2_take_hostile_turn();
    return true;
}

static bool u2_try_use_inventory_item(int index) {
    ItemSlot* inventory_slot = NULL;
    Item* item = NULL;

    if (index < 0 || index >= game_state.session.inventory.count) {
        return false;
    }

    inventory_slot = &game_state.session.inventory.slots[index];
    item = inventory_slot->item;
    if (item == NULL) {
        return false;
    }

    if (strcmp(item->id, "trail_ration") == 0) {
        if (!u2_inventory_remove_at(&game_state.session.inventory, index, 1)) {
            return false;
        }
        game_state.session.food += 20;
        u2_clamp_inventory_selection();
        game_state.session.panel = U2_PANEL_NONE;
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You eat a trail ration. Food +20.");
        u2_take_hostile_turn();
        return true;
    }

    if (strcmp(item->id, "lesser_healing_potion") == 0) {
        const int max_hp = d20_get_max_hp(&game_state.session.player.sheet);
        const int previous_hp = game_state.session.player.sheet.hp;
        const int heal_amount = roll_dice("2d6");

        if (previous_hp >= max_hp) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You are already at full health.");
            return false;
        }

        if (!u2_inventory_remove_at(&game_state.session.inventory, index, 1)) {
            return false;
        }
        game_state.session.player.sheet.hp = min(max_hp, previous_hp + heal_amount);
        u2_clamp_inventory_selection();
        game_state.session.panel = U2_PANEL_NONE;
        u2_session_show_message(
            &game_state.session,
            false,
            U2_LOG_MESSAGE_DURATION,
            "You drink a healing potion and recover %d HP.",
            game_state.session.player.sheet.hp - previous_hp
        );
        u2_take_hostile_turn();
        return true;
    }

    if (strcmp(item->id, "focus_tonic") == 0) {
        const int max_mp = d20_get_max_mp(&game_state.session.player.sheet);
        const int previous_mp = game_state.session.player.sheet.mp;
        const int restore_amount = roll_dice("2d4");

        if (max_mp <= 0 || previous_mp >= max_mp) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Your spirit is already steady.");
            return false;
        }

        if (!u2_inventory_remove_at(&game_state.session.inventory, index, 1)) {
            return false;
        }
        game_state.session.player.sheet.mp = min(max_mp, previous_mp + restore_amount);
        u2_clamp_inventory_selection();
        game_state.session.panel = U2_PANEL_NONE;
        u2_session_show_message(
            &game_state.session,
            false,
            U2_LOG_MESSAGE_DURATION,
            "You drink a focus tonic and recover %d MP.",
            game_state.session.player.sheet.mp - previous_mp
        );
        u2_take_hostile_turn();
        return true;
    }

    if (strcmp(item->id, "spellbook") == 0) {
        if (u2_get_player_spell_count() <= 0) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "The spellbook's pages remain stubbornly quiet.");
            return false;
        }
        u2_open_spell_panel();
        return true;
    }

    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You cannot figure out how to use %s.", item->name);
    return false;
}

static bool u2_try_activate_inventory_selection(void) {
    const int index = game_state.session.inventory_selection_index;
    const ItemSlot* slot = index >= 0 && index < game_state.session.inventory.count ? &game_state.session.inventory.slots[index] : NULL;
    Slot equip_slot = SLOT_END;

    if (slot == NULL || slot->item == NULL) {
        return false;
    }

    if (u2_get_item_equip_slot(slot->item, &equip_slot)) {
        return u2_try_equip_inventory_item(index);
    }

    return u2_try_use_inventory_item(index);
}

static void u2_draw_character_panel(void) {
    char damage_text[64];
    const int w = 232;
    const int h = 148;
    const int x = (screen_size.x - w) / 2;
    const int y = (screen_size.y - h) / 2;
    Character* player = &game_state.session.player.sheet;

    d20_get_damage_string(player, SLOT_HAND_RIGHT, damage_text, sizeof(damage_text));

    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 10, y + 10, COLOR_YELLOW, "CHARACTER");
    ascii_font_draw(chao_canvas, font, x + 10, y + 24, COLOR_WHITE, "%s", game_state.session.player_name);
    ascii_font_draw(chao_canvas, font, x + 10, y + 36, COLOR_LIGHTGRAY, "%s", u2_class_name(game_state.session.chosen_class));
    ascii_font_draw(chao_canvas, font, x + 10, y + 48, COLOR_WHITE, "LV %d  XP %d/%d", player->level, player->exp, d20_get_exp_for_level(player->level + 1));
    ascii_font_draw(chao_canvas, font, x + 10, y + 60, COLOR_WHITE, "HP %d/%d", player->hp, d20_get_max_hp(player));
    ascii_font_draw(chao_canvas, font, x + 114, y + 60, COLOR_WHITE, "MP %d/%d", player->mp, d20_get_max_mp(player));
    ascii_font_draw(chao_canvas, font, x + 10, y + 74, COLOR_LIGHTGRAY, "STR %d  DEX %d  MND %d", player->stats[STAT_STR], player->stats[STAT_DEX], player->stats[STAT_MIND]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 86, COLOR_LIGHTGRAY, "PHY %d  SUB %d", player->skills[SKILL_PHYSICAL], player->skills[SKILL_SUBTERFUGE]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 98, COLOR_LIGHTGRAY, "KNO %d  COM %d", player->skills[SKILL_KNOWLEDGE], player->skills[SKILL_COMMUNICATION]);
    ascii_font_draw(chao_canvas, font, x + 10, y + 110, COLOR_WHITE, "AC %d  DMG %s", d20_get_armor_class(player), damage_text);
    ascii_font_draw(chao_canvas, font, x + 10, y + 126, COLOR_YELLOW, "Stat points banked: %d", player->stat_points);
}

static void u2_draw_inventory_panel(void) {
    const int w = 254;
    const int h = 188;
    const int x = (screen_size.x - w) / 2;
    const int y = (screen_size.y - h) / 2;
    const int visible_rows = 8;
    const int first_visible = u2_get_inventory_first_visible_index(visible_rows);

    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 10, y + 10, COLOR_YELLOW, "INVENTORY");
    ascii_font_draw(chao_canvas, font, x + 10, y + 24, COLOR_WHITE, "Gold %d  Food %d", game_state.session.gold, game_state.session.food);
    ascii_font_draw(chao_canvas, font, x + 10, y + 40, COLOR_LIGHTGRAY, "Weapon: %s", u2_get_equipped_item_name(SLOT_HAND_RIGHT));
    ascii_font_draw(chao_canvas, font, x + 10, y + 52, COLOR_LIGHTGRAY, "Offhand: %s", u2_get_equipped_item_name(SLOT_HAND_LEFT));
    ascii_font_draw(chao_canvas, font, x + 10, y + 64, COLOR_LIGHTGRAY, "Body: %s", u2_get_equipped_item_name(SLOT_BODY));

    if (game_state.session.inventory.count == 0) {
        ascii_font_draw(chao_canvas, font, x + 10, y + 88, COLOR_LIGHTGRAY, "(empty)");
        ascii_font_draw(chao_canvas, font, x + 10, y + h - 18, COLOR_YELLOW, "Esc closes");
        return;
    }

    for (int row = 0; row < visible_rows; ++row) {
        const int i = first_visible + row;
        const ItemSlot* slot = NULL;
        const bool selected = i == game_state.session.inventory_selection_index;
        char item_label[96];

        if (i >= game_state.session.inventory.count) {
            break;
        }

        slot = &game_state.session.inventory.slots[i];
        if (slot->item == NULL) {
            continue;
        }

        u2_get_inventory_item_label(slot, item_label, sizeof(item_label));

        if (slot->amount > 1) {
            ascii_font_draw(
                chao_canvas,
                font,
                x + 10,
                y + 88 + row * 12,
                selected ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s %dx %s",
                selected ? ">" : " ",
                (int)slot->amount,
                item_label
            );
        } else {
            ascii_font_draw(
                chao_canvas,
                font,
                x + 10,
                y + 88 + row * 12,
                selected ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s %s",
                selected ? ">" : " ",
                item_label
            );
        }
    }

    if (game_state.session.inventory_selection_index >= 0 &&
        game_state.session.inventory_selection_index < game_state.session.inventory.count &&
        game_state.session.inventory.slots[game_state.session.inventory_selection_index].item != NULL) {
        ascii_font_draw(
            chao_canvas,
            font,
            x + 10,
            y + h - 30,
            COLOR_LIGHTGRAY,
            "%s: %s",
            u2_get_inventory_action_label(game_state.session.inventory.slots[game_state.session.inventory_selection_index].item),
            game_state.session.inventory.slots[game_state.session.inventory_selection_index].item->name
        );
    }
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 18, COLOR_YELLOW, "Up/Down select  Enter acts  Esc closes");
}

static void u2_draw_spell_panel(void) {
    const int w = 250;
    const int h = 148;
    const int x = (screen_size.x - w) / 2;
    const int y = (screen_size.y - h) / 2;
    const int spell_count = u2_get_player_spell_count();
    const U2SpellDef* selected_spell = u2_get_player_spell_by_index(game_state.session.spell_selection_index);

    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 10, y + 10, COLOR_YELLOW, "SPELLS");
    ascii_font_draw(chao_canvas, font, x + 10, y + 24, COLOR_WHITE, "MP %d/%d", game_state.session.player.sheet.mp, d20_get_max_mp(&game_state.session.player.sheet));

    if (spell_count <= 0) {
        ascii_font_draw(chao_canvas, font, x + 10, y + 44, COLOR_LIGHTGRAY, "No spells known.");
        ascii_font_draw(chao_canvas, font, x + 10, y + h - 18, COLOR_YELLOW, "Esc closes");
        return;
    }

    for (int i = 0; i < spell_count; ++i) {
        const U2SpellDef* spell = u2_get_player_spell_by_index(i);
        if (spell == NULL) {
            continue;
        }

        ascii_font_draw(
            chao_canvas,
            font,
            x + 10,
            y + 44 + i * 12,
            i == game_state.session.spell_selection_index ? COLOR_WHITE : COLOR_LIGHTGRAY,
            "%s %s - %d MP",
            i == game_state.session.spell_selection_index ? ">" : " ",
            spell->name,
            spell->mp_cost
        );
    }

    if (selected_spell != NULL) {
        u2_draw_wrapped_text(x + 10, y + 86, w - 20, COLOR_LIGHTGRAY, selected_spell->description);
    }
    ascii_font_draw(chao_canvas, font, x + 10, y + h - 18, COLOR_YELLOW, "Up/Down select  Enter casts  Esc closes");
}

static void u2_draw_service_panel(void) {
    const U2ServiceDef* service = u2_get_active_service_def();
    const int w = 288;
    const int h = 176;
    const int x = (screen_size.x - w) / 2;
    const int y = (screen_size.y - h) / 2;
    const int list_x = x + 12;
    const int list_y = y + 72;

    if (service == NULL) {
        return;
    }

    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 12, y + 10, COLOR_YELLOW, "%s", service->title);
    u2_draw_wrapped_text(x + 12, y + 24, w - 24, COLOR_LIGHTGRAY, service->description);
    ascii_font_draw(chao_canvas, font, x + 12, y + 52, COLOR_WHITE, "Gold %d  Food %d", game_state.session.gold, game_state.session.food);
    ascii_font_draw(chao_canvas, font, x + 148, y + 52, COLOR_WHITE, "HP %d/%d", game_state.session.player.sheet.hp, d20_get_max_hp(&game_state.session.player.sheet));
    ascii_font_draw(chao_canvas, font, x + 148, y + 64, COLOR_WHITE, "MP %d/%d", game_state.session.player.sheet.mp, d20_get_max_mp(&game_state.session.player.sheet));

    switch (service->kind) {
        case U2_SERVICE_SHOP:
        {
            const int stock_count = u2_get_shop_stock_count(service->entity_id);
            for (int i = 0; i < stock_count; ++i) {
                const U2ShopStockDef* stock = u2_get_shop_stock_by_index(service->entity_id, i);
                Item* item = stock != NULL && stock->item_id != NULL ? items_get(stock->item_id) : NULL;
                const char* label = stock != NULL ? stock->label : NULL;
                char line[96];

                if (label == NULL && item != NULL) {
                    label = item->name;
                }
                if (label == NULL) {
                    label = "Stock";
                }

                snprintf(line, sizeof(line), "%s %s - %dg",
                    i == game_state.session.service_selection_index ? ">" : " ",
                    label,
                    stock != NULL ? stock->price : 0);
                ascii_font_draw(chao_canvas, font, list_x, list_y + i * 12,
                    i == game_state.session.service_selection_index ? COLOR_WHITE : COLOR_LIGHTGRAY,
                    "%s", line);
            }

            ascii_font_draw(chao_canvas, font, list_x, list_y + stock_count * 12,
                game_state.session.service_selection_index == stock_count ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Leave", game_state.session.service_selection_index == stock_count ? ">" : " ");
            ascii_font_draw(chao_canvas, font, x + 12, y + h - 18, COLOR_YELLOW, "Up/Down selects  Enter buys  Esc leaves");
            return;
        }
        case U2_SERVICE_INN:
            ascii_font_draw(chao_canvas, font, list_x, list_y + 0,
                game_state.session.service_selection_index == 0 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Rest till dawn - %dg",
                game_state.session.service_selection_index == 0 ? ">" : " ",
                service->primary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 12,
                game_state.session.service_selection_index == 1 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Leave",
                game_state.session.service_selection_index == 1 ? ">" : " ");
            ascii_font_draw(chao_canvas, font, x + 12, y + h - 18, COLOR_YELLOW, "Enter confirms  Esc leaves");
            return;
        case U2_SERVICE_HEALER:
            ascii_font_draw(chao_canvas, font, list_x, list_y + 0,
                game_state.session.service_selection_index == 0 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Heal wounds - %dg",
                game_state.session.service_selection_index == 0 ? ">" : " ",
                service->primary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 12,
                game_state.session.service_selection_index == 1 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Recover spirit - %dg",
                game_state.session.service_selection_index == 1 ? ">" : " ",
                service->secondary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 24,
                game_state.session.service_selection_index == 2 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Leave",
                game_state.session.service_selection_index == 2 ? ">" : " ");
            ascii_font_draw(chao_canvas, font, x + 12, y + h - 18, COLOR_YELLOW, "Enter confirms  Esc leaves");
            return;
        case U2_SERVICE_SANCTUARY:
            ascii_font_draw(chao_canvas, font, list_x, list_y + 0,
                game_state.session.service_selection_index == 0 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Rest fully - %dg",
                game_state.session.service_selection_index == 0 ? ">" : " ",
                service->primary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 12,
                game_state.session.service_selection_index == 1 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Heal wounds - %dg",
                game_state.session.service_selection_index == 1 ? ">" : " ",
                service->secondary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 24,
                game_state.session.service_selection_index == 2 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Recover spirit - %dg",
                game_state.session.service_selection_index == 2 ? ">" : " ",
                service->tertiary_cost);
            ascii_font_draw(chao_canvas, font, list_x, list_y + 36,
                game_state.session.service_selection_index == 3 ? COLOR_WHITE : COLOR_LIGHTGRAY,
                "%s Leave",
                game_state.session.service_selection_index == 3 ? ">" : " ");
            ascii_font_draw(chao_canvas, font, x + 12, y + h - 18, COLOR_YELLOW, "Enter confirms  Esc leaves");
            return;
        case U2_SERVICE_NONE:
        default:
            ascii_font_draw(chao_canvas, font, list_x, list_y, COLOR_LIGHTGRAY, "No services available.");
            ascii_font_draw(chao_canvas, font, x + 12, y + h - 18, COLOR_YELLOW, "Esc leaves");
            return;
    }
}

static bool u2_save_exists(void) {
    char* data = NULL;

    if (u2_cached_save_exists >= 0) {
        return u2_cached_save_exists != 0;
    }

    data = platform_load(U2_SAVE_FILE_PATH);
    u2_cached_save_exists = data != NULL ? 1 : 0;
    free(data);
    return u2_cached_save_exists != 0;
}

static void u2_save_character(U2SavedCharacter* saved_character, const Character* character) {
    if (saved_character == NULL || character == NULL) {
        return;
    }

    memset(saved_character, 0, sizeof(*saved_character));
    saved_character->class_id = character->class;
    saved_character->base_hp = character->base_hp;
    saved_character->base_mp = character->base_mp;
    u2_copy_string(saved_character->base_damage, sizeof(saved_character->base_damage), character->base_damage);
    saved_character->armor_class = character->armor_class;
    saved_character->hp = character->hp;
    saved_character->mp = character->mp;
    memcpy(saved_character->stats, character->stats, sizeof(saved_character->stats));
    memcpy(saved_character->skills, character->skills, sizeof(saved_character->skills));
    saved_character->level = character->level;
    saved_character->exp = character->exp;
    saved_character->stat_points = character->stat_points;

    for (int i = 0; i < SLOT_END; ++i) {
        u2_copy_string(
            saved_character->equipment_ids[i],
            sizeof(saved_character->equipment_ids[i]),
            character->equipment[i] != NULL ? character->equipment[i]->id : NULL
        );
    }
}

static void u2_restore_character(Character* character, const U2SavedCharacter* saved_character) {
    if (character == NULL || saved_character == NULL) {
        return;
    }

    character->class = (CharacterClass)saved_character->class_id;
    character->base_hp = saved_character->base_hp;
    character->base_mp = saved_character->base_mp;
    character->armor_class = saved_character->armor_class;
    character->hp = max(0, saved_character->hp);
    character->mp = max(0, saved_character->mp);
    memcpy(character->stats, saved_character->stats, sizeof(character->stats));
    memcpy(character->skills, saved_character->skills, sizeof(character->skills));
    character->level = max(1, saved_character->level);
    character->exp = max(0, saved_character->exp);
    character->stat_points = max(0, saved_character->stat_points);

    memset(character->equipment, 0, sizeof(character->equipment));
    for (int i = 0; i < SLOT_END; ++i) {
        if (saved_character->equipment_ids[i][0] != '\0') {
            character->equipment[i] = items_get(saved_character->equipment_ids[i]);
        }
    }
}

static void u2_fill_save_data(U2SaveData* save_data) {
    if (save_data == NULL) {
        return;
    }

    memset(save_data, 0, sizeof(*save_data));
    memcpy(save_data->magic, "U2SV", U2_SAVE_MAGIC_SIZE);
    save_data->version = U2_SAVE_VERSION;
    u2_copy_string(save_data->player_name, sizeof(save_data->player_name), game_state.session.player_name);
    save_data->chosen_class = game_state.session.chosen_class;
    u2_copy_string(
        save_data->current_map_id,
        sizeof(save_data->current_map_id),
        game_state.session.current_map != NULL ? game_state.session.current_map->id : NULL
    );
    save_data->player_tile_x = game_state.session.player.tile_x;
    save_data->player_tile_y = game_state.session.player.tile_y;
    save_data->player_facing = game_state.session.player.facing;
    save_data->gold = game_state.session.gold;
    save_data->food = game_state.session.food;
    save_data->food_depletion_step_counter = game_state.session.food_depletion_step_counter;
    save_data->food_regen_step_counter = game_state.session.food_regen_step_counter;
    u2_save_character(&save_data->player, &game_state.session.player.sheet);

    save_data->inventory_count = min(game_state.session.inventory.count, U2_MAX_INVENTORY_SLOTS);
    for (int i = 0; i < save_data->inventory_count; ++i) {
        const ItemSlot* slot = &game_state.session.inventory.slots[i];
        if (slot->item == NULL) {
            continue;
        }

        u2_copy_string(save_data->inventory[i].item_id, sizeof(save_data->inventory[i].item_id), slot->item->id);
        save_data->inventory[i].amount = (int)slot->amount;
    }

    for (int i = 0; i < U2_MAX_ENTITIES; ++i) {
        const U2PersistentEntityState* persistent_state = &game_state.session.persistent_entities[i];
        U2SavedSceneEntity* saved_entity = &save_data->scene_entities[save_data->scene_entity_count];

        if (!persistent_state->in_use || persistent_state->id == NULL || save_data->scene_entity_count >= U2_MAX_SCENE_ENTITIES) {
            continue;
        }

        saved_entity->active = persistent_state->active ? 1 : 0;
        u2_copy_string(saved_entity->id, sizeof(saved_entity->id), persistent_state->id);
        saved_entity->tile_x = persistent_state->tile_x;
        saved_entity->tile_y = persistent_state->tile_y;
        saved_entity->facing = persistent_state->facing;
        saved_entity->hp = persistent_state->hp;
        saved_entity->mp = persistent_state->mp;
        save_data->scene_entity_count++;
    }
}

static bool u2_write_save_data(const U2SaveData* save_data) {
    char* encoded = NULL;
    bool success = false;

    if (save_data == NULL) {
        return false;
    }

    encoded = u2_encode_save_data_hex(save_data);
    if (encoded == NULL) {
        return false;
    }

    success = platform_save(U2_SAVE_FILE_PATH, encoded);
    free(encoded);
    if (success) {
        u2_cached_save_exists = 1;
    }
    return success;
}

static bool u2_read_save_data(U2SaveData* save_data) {
    char* encoded = NULL;
    bool success = false;

    if (save_data == NULL) {
        return false;
    }

    encoded = platform_load(U2_SAVE_FILE_PATH);
    if (encoded == NULL) {
        u2_cached_save_exists = 0;
        return false;
    }

    success = u2_decode_save_data_hex(save_data, encoded);
    free(encoded);

    if (!success ||
        memcmp(save_data->magic, "U2SV", U2_SAVE_MAGIC_SIZE) != 0 ||
        save_data->version != U2_SAVE_VERSION) {
        return false;
    }

    u2_cached_save_exists = 1;
    save_data->inventory_count = clamp(save_data->inventory_count, 0, U2_MAX_INVENTORY_SLOTS);
    save_data->scene_entity_count = clamp(save_data->scene_entity_count, 0, U2_MAX_SCENE_ENTITIES);
    return true;
}

static void u2_restore_inventory_from_save(const U2SaveData* save_data) {
    if (save_data == NULL) {
        return;
    }

    u2_inventory_clear(&game_state.session.inventory);
    for (int i = 0; i < save_data->inventory_count; ++i) {
        if (save_data->inventory[i].item_id[0] == '\0' || save_data->inventory[i].amount <= 0) {
            continue;
        }

        u2_inventory_add_by_id(
            &game_state.session.inventory,
            save_data->inventory[i].item_id,
            (size_t)save_data->inventory[i].amount
        );
    }
}

static void u2_restore_persistent_entities_from_save(const U2SaveData* save_data) {
    if (save_data == NULL) {
        return;
    }

    for (int i = 0; i < save_data->scene_entity_count; ++i) {
        const U2SavedSceneEntity* saved_entity = &save_data->scene_entities[i];
        const U2SpawnDef* spawn_def = NULL;
        U2PersistentEntityState* persistent_state = NULL;

        if (saved_entity->id[0] == '\0') {
            continue;
        }

        spawn_def = u2_find_spawn_def(saved_entity->id);
        if (spawn_def == NULL) {
            continue;
        }

        persistent_state = u2_get_persistent_entity_state(spawn_def->id, true);
        if (persistent_state == NULL) {
            continue;
        }

        persistent_state->seeded = true;
        persistent_state->active = saved_entity->active != 0;
        persistent_state->tile_x = saved_entity->tile_x;
        persistent_state->tile_y = saved_entity->tile_y;
        persistent_state->facing = (U2Facing)clamp(saved_entity->facing, U2_FACING_SOUTH, U2_FACING_EAST);
        persistent_state->hp = max(0, saved_entity->hp);
        persistent_state->mp = max(0, saved_entity->mp);
    }
}

static bool u2_try_save_game(void) {
    U2SaveData save_data;

    if (!game_state.session.started || game_state.session.current_map == NULL) {
        return false;
    }

    u2_fill_save_data(&save_data);
    return u2_write_save_data(&save_data);
}

static bool u2_try_load_game(void) {
    U2SaveData save_data;
    const U2ClassTemplate* class_template = NULL;

    if (!u2_read_save_data(&save_data)) {
        return false;
    }

    class_template = u2_find_class_template((CharacterClass)save_data.chosen_class);
    if (class_template == NULL || u2_find_map(save_data.current_map_id) == NULL) {
        return false;
    }

    u2_session_init(&game_state.session);
    game_state.session.started = true;
    game_state.session.flow_state = U2_FLOW_PLAYING;
    u2_copy_string(game_state.session.player_name, sizeof(game_state.session.player_name), save_data.player_name);
    u2_configure_player_from_template(class_template);
    u2_restore_character(&game_state.session.player.sheet, &save_data.player);
    game_state.session.gold = save_data.gold;
    game_state.session.food = save_data.food;
    game_state.session.food_depletion_step_counter = max(0, save_data.food_depletion_step_counter);
    game_state.session.food_regen_step_counter = max(0, save_data.food_regen_step_counter);
    u2_restore_inventory_from_save(&save_data);
    u2_restore_persistent_entities_from_save(&save_data);
    u2_load_scene(save_data.current_map_id, save_data.player_tile_x, save_data.player_tile_y);
    game_state.session.player.facing = (U2Facing)clamp(save_data.player_facing, U2_FACING_SOUTH, U2_FACING_EAST);
    u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Loaded saved journey.");
    return true;
}

static void u2_draw_title_screen(void) {
    const int w = 236;
    const int h = 136;
    const int x = (screen_size.x - w) / 2;
    const int y = 34;
    const bool has_save = u2_save_exists();
    const bool continue_selected = has_save && game_state.session.title_selection_index == 1;
    const bool new_game_selected = !continue_selected;

    fill(chao_canvas, COLOR_BLACK);
    u2_draw_panel(x, y, w, h);
    ascii_font_draw_ex(chao_canvas, font, screen_size.x / 2, y + 16, COLOR_YELLOW, TEXT_ALIGN_CENTER, -1, "ULTIMATUM II");
    ascii_font_draw_ex(chao_canvas, font, screen_size.x / 2, y + 32, COLOR_LIGHTGRAY, TEXT_ALIGN_CENTER, -1, "Revenge of the Enchantress");
    ascii_font_draw_ex(
        chao_canvas,
        font,
        screen_size.x / 2,
        y + 58,
        new_game_selected ? COLOR_WHITE : COLOR_LIGHTGRAY,
        TEXT_ALIGN_CENTER,
        -1,
        "%s New Game",
        new_game_selected ? ">" : " "
    );
    ascii_font_draw_ex(
        chao_canvas,
        font,
        screen_size.x / 2,
        y + 74,
        has_save ? (continue_selected ? COLOR_WHITE : COLOR_LIGHTGRAY) : COLOR_DARKGRAY,
        TEXT_ALIGN_CENTER,
        -1,
        "%s Continue",
        continue_selected ? ">" : " "
    );
    ascii_font_draw_ex(chao_canvas, font, screen_size.x / 2, y + 94, COLOR_LIGHTGRAY, TEXT_ALIGN_CENTER, -1, "Up/Down choose  Enter selects");
    ascii_font_draw_ex(chao_canvas, font, screen_size.x / 2, y + 110, COLOR_LIGHTGRAY, TEXT_ALIGN_CENTER, -1, "Microlite20 town slice");
}

static void u2_draw_class_select(void) {
    const int w = 300;
    const int h = 176;
    const int x = (screen_size.x - w) / 2;
    const int y = 22;
    const U2ClassTemplate* selected_class = u2_get_class_template_by_index(game_state.session.class_selection_index);

    fill(chao_canvas, COLOR_BLACK);
    u2_draw_panel(x, y, w, h);
    ascii_font_draw(chao_canvas, font, x + 12, y + 10, COLOR_YELLOW, "Choose Your Class");

    for (int i = 0; i < (int)u2_class_templates_count; ++i) {
        const U2ClassTemplate* class_template = &u2_class_templates[i];
        ascii_font_draw(
            chao_canvas,
            font,
            x + 14,
            y + 28 + i * 12,
            i == game_state.session.class_selection_index ? COLOR_WHITE : COLOR_LIGHTGRAY,
            "%s %s",
            i == game_state.session.class_selection_index ? ">" : " ",
            class_template->name
        );
    }

    if (selected_class != NULL) {
        ascii_font_draw(chao_canvas, font, x + 122, y + 28, COLOR_WHITE, "%s", selected_class->name);
        u2_draw_wrapped_text(x + 122, y + 40, 162, COLOR_LIGHTGRAY, selected_class->description);
        ascii_font_draw(chao_canvas, font, x + 122, y + 78, COLOR_WHITE, "HP %d  MP %d", selected_class->base_hp, selected_class->base_mp);
        ascii_font_draw(chao_canvas, font, x + 122, y + 90, COLOR_LIGHTGRAY, "STR %d  DEX %d", selected_class->strength, selected_class->dexterity);
        ascii_font_draw(chao_canvas, font, x + 122, y + 102, COLOR_LIGHTGRAY, "MND %d", selected_class->mind);
        ascii_font_draw(chao_canvas, font, x + 122, y + 114, COLOR_LIGHTGRAY, "Start %s", items_get(selected_class->starter_weapon_id)->name);
        ascii_font_draw(chao_canvas, font, x + 122, y + 126, COLOR_LIGHTGRAY, "Gold %d  Food %d", selected_class->starter_gold, selected_class->starter_food);
    }

    ascii_font_draw(chao_canvas, font, x + 12, y + h - 20, COLOR_YELLOW, "Enter begins  Esc backs out");
}

static void u2_draw_gameplay(void) {
    fill(chao_canvas, COLOR_BLACK);
    chao_blend_mode = BLEND_NONE;

    u2_draw_world();
    u2_draw_scene_entities();
    u2_draw_spell_projectile();
    u2_draw_damage_popups();
    u2_draw_hud();
    u2_draw_message_log();

    if (game_state.session.panel == U2_PANEL_CHARACTER) {
        u2_draw_character_panel();
    } else if (game_state.session.panel == U2_PANEL_INVENTORY) {
        u2_draw_inventory_panel();
    } else if (game_state.session.panel == U2_PANEL_SPELLS) {
        u2_draw_spell_panel();
    } else if (game_state.session.panel == U2_PANEL_SERVICE) {
        u2_draw_service_panel();
    }
}

static void u2_draw_combat(void) {
    u2_draw_gameplay();
}

static void game_draw(void) {
    switch (game_state.session.flow_state) {
        case U2_FLOW_TITLE:
            u2_draw_title_screen();
            break;
        case U2_FLOW_CLASS_SELECT:
            u2_draw_class_select();
            break;
        case U2_FLOW_COMBAT:
            u2_draw_combat();
            break;
        case U2_FLOW_PLAYING:
        default:
            u2_draw_gameplay();
            break;
    }
}

static U2MoveDirection u2_get_requested_direction(void) {
    if (input.just_pressed[KEY_LEFT]) return U2_MOVE_WEST;
    if (input.just_pressed[KEY_RIGHT]) return U2_MOVE_EAST;
    if (input.just_pressed[KEY_UP]) return U2_MOVE_NORTH;
    if (input.just_pressed[KEY_DOWN]) return U2_MOVE_SOUTH;

    if (input.pressed[KEY_LEFT]) return U2_MOVE_WEST;
    if (input.pressed[KEY_RIGHT]) return U2_MOVE_EAST;
    if (input.pressed[KEY_UP]) return U2_MOVE_NORTH;
    if (input.pressed[KEY_DOWN]) return U2_MOVE_SOUTH;

    return U2_MOVE_NONE;
}

static bool u2_is_sprint_pressed(void) {
    return input.pressed[KEY_LSHIFT] || input.pressed[KEY_RSHIFT];
}

static void u2_try_move_player(U2MoveDirection direction) {
    int dx = 0;
    int dy = 0;
    int target_x = game_state.session.player.tile_x;
    int target_y = game_state.session.player.tile_y;
    const U2TileDef* target_tile = NULL;
    U2Entity* blocking_entity = NULL;

    switch (direction) {
        case U2_MOVE_WEST: dx = -1; game_state.session.player.facing = U2_FACING_WEST; break;
        case U2_MOVE_EAST: dx = 1; game_state.session.player.facing = U2_FACING_EAST; break;
        case U2_MOVE_NORTH: dy = -1; game_state.session.player.facing = U2_FACING_NORTH; break;
        case U2_MOVE_SOUTH: dy = 1; game_state.session.player.facing = U2_FACING_SOUTH; break;
        case U2_MOVE_NONE:
        default:
            return;
    }

    target_x += dx;
    target_y += dy;

    if (u2_scene_is_looped()) {
        target_x = u2_entity_wrap_coord(target_x, game_state.session.current_map->width);
        target_y = u2_entity_wrap_coord(target_y, game_state.session.current_map->height);
    } else if (target_x < 0 || target_y < 0 || target_x >= game_state.session.current_map->width || target_y >= game_state.session.current_map->height) {
        const U2TransitionDef* exit_transition = u2_find_first_transition_from_map(game_state.session.current_map->id);
        if (exit_transition != NULL) {
            u2_load_scene(exit_transition->destination_map_id, exit_transition->destination_x, exit_transition->destination_y);
            u2_session_show_message(
                &game_state.session,
                false,
                U2_LOG_MESSAGE_DURATION,
                "Returned to %s.",
                game_state.session.current_map->name
            );
            return;
        }
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "You cannot leave this location that way.");
        return;
    }

    blocking_entity = u2_find_scene_entity_at(target_x, target_y);
    if (blocking_entity != NULL && blocking_entity->solid) {
        if (u2_is_hostile_entity(blocking_entity)) {
            u2_resolve_player_bump_attack(blocking_entity);
            u2_take_hostile_turn();
            return;
        }
        u2_interact_with_entity(blocking_entity);
        return;
    }

    target_tile = u2_get_tile_at(game_state.session.current_map, target_x, target_y);
    if (target_tile == NULL || !target_tile->passable) {
        u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s", u2_get_blocked_message(target_tile));
        return;
    }

    {
        const char* placeholder_message = u2_get_unimplemented_message(target_tile);
        if (placeholder_message != NULL) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "%s", placeholder_message);
            return;
        }
    }

    game_state.session.player.tile_x = target_x;
    game_state.session.player.tile_y = target_y;
    u2_update_food_for_step();

    if (u2_scene_is_looped()) {
        const U2TransitionDef* transition = u2_find_transition(game_state.session.current_map->id, target_x, target_y);
        if (transition != NULL) {
            u2_load_scene(transition->destination_map_id, transition->destination_x, transition->destination_y);
            u2_session_show_message(
                &game_state.session,
                false,
                U2_LOG_MESSAGE_DURATION,
                "Entered %s.",
                game_state.session.current_map->name
            );
            return;
        }
    }

    u2_take_hostile_turn();
}

static void u2_update_message_timer(float dt) {
    if (!game_state.session.message.visible) {
        return;
    }

    {
        const int total_chars = (int)strlen(game_state.session.message.text);
        game_state.session.message.reveal_progress += dt * U2_MESSAGE_REVEAL_CHARS_PER_SECOND;
        game_state.session.message.visible_chars = min(total_chars, (int)game_state.session.message.reveal_progress);
    }

    if (!game_state.session.message.modal && game_state.session.message.visible_chars >= (int)strlen(game_state.session.message.text) && game_state.session.message.timer > 0.0f) {
        game_state.session.message.timer -= dt;
        if (game_state.session.message.timer <= 0.0f) {
            u2_session_clear_message(&game_state.session);
        }
    }
}

static void u2_update_title(void) {
    const bool has_save = u2_save_exists();

    if (!has_save) {
        game_state.session.title_selection_index = 0;
    }

    if (input.just_pressed[KEY_UP] || input.just_pressed[KEY_DOWN]) {
        if (has_save) {
            game_state.session.title_selection_index = 1 - game_state.session.title_selection_index;
        } else {
            game_state.session.title_selection_index = 0;
        }
    }

    if (input.just_pressed[KEY_N]) {
        game_state.session.flow_state = U2_FLOW_CLASS_SELECT;
        game_state.session.class_selection_index = 0;
        game_state.session.title_selection_index = 0;
        return;
    }

    if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
        if (game_state.session.title_selection_index == 1 && has_save) {
            u2_try_load_game();
            return;
        }

        game_state.session.flow_state = U2_FLOW_CLASS_SELECT;
        game_state.session.class_selection_index = 0;
        game_state.session.title_selection_index = 0;
    }
}

static void u2_update_class_select(void) {
    if (input.just_pressed[KEY_UP]) {
        game_state.session.class_selection_index--;
        if (game_state.session.class_selection_index < 0) {
            game_state.session.class_selection_index = (int)u2_class_templates_count - 1;
        }
    }
    if (input.just_pressed[KEY_DOWN]) {
        game_state.session.class_selection_index++;
        if (game_state.session.class_selection_index >= (int)u2_class_templates_count) {
            game_state.session.class_selection_index = 0;
        }
    }
    if (input.just_pressed[KEY_ESC]) {
        game_state.session.flow_state = U2_FLOW_TITLE;
    }
    if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
        const U2ClassTemplate* class_template = u2_get_class_template_by_index(game_state.session.class_selection_index);
        if (class_template != NULL) {
            u2_start_new_game(class_template);
        }
    }
}

static void u2_update_combat(void) {
    u2_session_clear_combat(&game_state.session);
    game_state.session.flow_state = U2_FLOW_PLAYING;
}

static void u2_update_gameplay_ui(void) {
    if (input.just_pressed[KEY_F5]) {
        if (u2_try_save_game()) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Journey saved.");
        } else {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Unable to save right now.");
        }
        return;
    }

    if (input.just_pressed[KEY_F9]) {
        if (!u2_try_load_game()) {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "No valid save was found.");
        }
        return;
    }

    if (game_state.session.message.visible && game_state.session.message.modal) {
        if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE] || input.just_pressed[KEY_ESC]) {
            u2_session_clear_message(&game_state.session);
        }
        return;
    }

    if (game_state.session.spell_targeting.active) {
        if (input.just_pressed[KEY_ESC] || input.just_pressed[KEY_M]) {
            u2_session_clear_spell_targeting(&game_state.session);
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "Spell canceled.");
            return;
        }
        if (input.just_pressed[KEY_LEFT]) {
            u2_try_cast_targeting_spell_in_direction(-1, 0);
            return;
        }
        if (input.just_pressed[KEY_RIGHT]) {
            u2_try_cast_targeting_spell_in_direction(1, 0);
            return;
        }
        if (input.just_pressed[KEY_UP]) {
            u2_try_cast_targeting_spell_in_direction(0, -1);
            return;
        }
        if (input.just_pressed[KEY_DOWN]) {
            u2_try_cast_targeting_spell_in_direction(0, 1);
            return;
        }
        return;
    }

    if (game_state.session.panel == U2_PANEL_SERVICE) {
        const U2ServiceDef* service = u2_get_active_service_def();
        const int option_count = u2_get_service_option_count(service);

        if (service == NULL || option_count <= 0) {
            u2_close_service_panel();
            return;
        }

        if (input.just_pressed[KEY_ESC]) {
            u2_close_service_panel();
            return;
        }
        if (input.just_pressed[KEY_UP]) {
            game_state.session.service_selection_index--;
            if (game_state.session.service_selection_index < 0) {
                game_state.session.service_selection_index = option_count - 1;
            }
        }
        if (input.just_pressed[KEY_DOWN]) {
            game_state.session.service_selection_index++;
            if (game_state.session.service_selection_index >= option_count) {
                game_state.session.service_selection_index = 0;
            }
        }
        if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
            u2_execute_service_action();
        }
        return;
    }

    if (game_state.session.panel == U2_PANEL_INVENTORY) {
        u2_clamp_inventory_selection();

        if (input.just_pressed[KEY_ESC] || input.just_pressed[KEY_I]) {
            game_state.session.panel = U2_PANEL_NONE;
            return;
        }
        if (game_state.session.inventory.count <= 0) {
            return;
        }
        if (input.just_pressed[KEY_UP]) {
            game_state.session.inventory_selection_index--;
            if (game_state.session.inventory_selection_index < 0) {
                game_state.session.inventory_selection_index = game_state.session.inventory.count - 1;
            }
            return;
        }
        if (input.just_pressed[KEY_DOWN]) {
            game_state.session.inventory_selection_index++;
            if (game_state.session.inventory_selection_index >= game_state.session.inventory.count) {
                game_state.session.inventory_selection_index = 0;
            }
            return;
        }
        if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
            u2_try_activate_inventory_selection();
        }
        return;
    }

    if (game_state.session.panel == U2_PANEL_SPELLS) {
        const int spell_count = u2_get_player_spell_count();

        if (input.just_pressed[KEY_ESC] || input.just_pressed[KEY_M]) {
            game_state.session.panel = U2_PANEL_NONE;
            return;
        }
        if (spell_count <= 0) {
            return;
        }
        if (input.just_pressed[KEY_UP]) {
            game_state.session.spell_selection_index--;
            if (game_state.session.spell_selection_index < 0) {
                game_state.session.spell_selection_index = spell_count - 1;
            }
            return;
        }
        if (input.just_pressed[KEY_DOWN]) {
            game_state.session.spell_selection_index++;
            if (game_state.session.spell_selection_index >= spell_count) {
                game_state.session.spell_selection_index = 0;
            }
            return;
        }
        if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
            u2_try_cast_spell(u2_get_player_spell_by_index(game_state.session.spell_selection_index));
        }
        return;
    }

    if (game_state.session.panel != U2_PANEL_NONE) {
        if (input.just_pressed[KEY_ESC] ||
            (game_state.session.panel == U2_PANEL_CHARACTER && input.just_pressed[KEY_C])) {
            game_state.session.panel = U2_PANEL_NONE;
        }
        return;
    }

    if (input.just_pressed[KEY_ENTER] || input.just_pressed[KEY_SPACE]) {
        u2_try_interact_forward();
        return;
    }

    if (input.just_pressed[KEY_C]) {
        game_state.session.panel = U2_PANEL_CHARACTER;
        return;
    }
    if (input.just_pressed[KEY_I]) {
        u2_open_inventory_panel();
        return;
    }
    if (input.just_pressed[KEY_M]) {
        if (u2_get_player_spell_count() > 0) {
            u2_open_spell_panel();
        } else {
            u2_session_show_message(&game_state.session, false, U2_LOG_MESSAGE_DURATION, "This class knows no spells yet.");
        }
        return;
    }
}

static void u2_update_player_movement(float dt) {
    const U2MoveDirection direction = u2_get_requested_direction();
    const float step_delay = u2_is_sprint_pressed() ? U2_SPRINT_STEP_DELAY : U2_STEP_DELAY;

    if (direction == U2_MOVE_NONE) {
        game_state.session.held_move_direction = U2_MOVE_NONE;
        game_state.session.move_repeat_timer = 0.0f;
        return;
    }

    if (direction == game_state.session.held_move_direction) {
        game_state.session.move_repeat_timer = min(game_state.session.move_repeat_timer, step_delay);
    }

    game_state.session.move_repeat_timer -= dt;
    if (direction != game_state.session.held_move_direction || game_state.session.move_repeat_timer <= 0.0f) {
        u2_try_move_player(direction);
        game_state.session.move_repeat_timer = step_delay;
    }

    game_state.session.held_move_direction = direction;
}

static void u2_update_gameplay(float dt) {
    const bool had_active_projectile = game_state.session.spell_projectile.active;

    u2_update_message_timer(dt);
    u2_update_hit_feedback(dt);
    if (had_active_projectile) {
        u2_update_spell_projectile(dt);
        u2_update_camera(dt);
        return;
    }

    u2_update_gameplay_ui();

    if (u2_session_input_locked(&game_state.session)) {
        return;
    }

    u2_update_player_movement(dt);
    u2_update_camera(dt);
}

void game_init(void) {
    platform_set_window_size(800, 600);

    memset(&game_state, 0, sizeof(game_state));

    items_init();
    assets_load_bitmap("tiles", "assets/tiles.png");
    assets_load_bitmap("frame", "assets/frame.png");
    font_bitmap = load_bitmap("assets/cp437_8x8.png");
    font = ascii_font_create(font_bitmap, true);

    game_state.frame_slice = (NineSlice){
        .bitmap = AGB("frame"),
        .top = 4,
        .bottom = 4,
        .left = 4,
        .right = 4,
        .stretch_mode = SLICE_MODE_TILE,
    };

    u2_build_tile_lookup();
    u2_session_init(&game_state.session);
}

void game_update(float dt) {
    switch (game_state.session.flow_state) {
        case U2_FLOW_TITLE:
            u2_update_title();
            break;
        case U2_FLOW_CLASS_SELECT:
            u2_update_class_select();
            break;
        case U2_FLOW_COMBAT:
            u2_update_combat();
            break;
        case U2_FLOW_PLAYING:
        default:
            u2_update_gameplay(dt);
            break;
    }

    game_draw();
}

void game_final(void) {
    ascii_font_free(font);
    free_bitmap(font_bitmap);
}

#endif // GAME_H
