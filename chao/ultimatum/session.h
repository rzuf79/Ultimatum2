#ifndef ULTIMATUM_SESSION_H
#define ULTIMATUM_SESSION_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "entity.h"

enum {
    U2_MAX_SCENE_ENTITIES = 32,
    U2_MAX_INVENTORY_SLOTS = 16,
    U2_MESSAGE_TEXT_MAX = 512,
    U2_PLAYER_NAME_MAX = 32,
};

typedef enum {
    U2_FLOW_TITLE = 0,
    U2_FLOW_CLASS_SELECT,
    U2_FLOW_PLAYING,
    U2_FLOW_COMBAT,
} U2FlowState;

typedef enum {
    U2_SCENE_OVERWORLD = 0,
    U2_SCENE_INTERIOR,
} U2SceneType;

typedef enum {
    U2_PANEL_NONE = 0,
    U2_PANEL_CHARACTER,
    U2_PANEL_INVENTORY,
    U2_PANEL_SERVICE,
    U2_PANEL_SPELLS,
} U2PanelType;

typedef enum {
    U2_MOVE_NONE = 0,
    U2_MOVE_WEST,
    U2_MOVE_EAST,
    U2_MOVE_NORTH,
    U2_MOVE_SOUTH,
} U2MoveDirection;

typedef struct {
    ItemSlot slots[U2_MAX_INVENTORY_SLOTS];
    int count;
} U2Inventory;

typedef struct {
    bool visible;
    bool modal;
    float timer;
    float reveal_progress;
    int visible_chars;
    char text[U2_MESSAGE_TEXT_MAX];
} U2MessageState;

typedef struct {
    bool in_use;
    bool seeded;
    bool active;
    const char* id;
    int tile_x;
    int tile_y;
    U2Facing facing;
    int hp;
    int mp;
} U2PersistentEntityState;

typedef struct {
    bool active;
    U2Entity enemy;
    char source_entity_id[32];
    int selection_index;
    char log[U2_MESSAGE_TEXT_MAX];
} U2CombatState;

typedef struct {
    bool active;
    const char* spell_id;
} U2SpellTargetingState;

typedef struct {
    bool active;
    const char* spell_id;
    int start_tile_x;
    int start_tile_y;
    int dir_x;
    int dir_y;
    int travel_steps;
    int damage;
    char target_entity_id[32];
    float progress;
    float duration;
} U2SpellProjectileState;

typedef struct {
    bool started;
    char player_name[U2_PLAYER_NAME_MAX];
    CharacterClass chosen_class;
    U2FlowState flow_state;
    U2SceneType scene_type;
    U2PanelType panel;
    const U2MapDef* current_map;
    U2Entity player;
    U2Entity scene_entities[U2_MAX_SCENE_ENTITIES];
    int gold;
    int food;
    int food_depletion_step_counter;
    int food_regen_step_counter;
    int title_selection_index;
    int class_selection_index;
    float move_repeat_timer;
    U2MoveDirection held_move_direction;
    U2Inventory inventory;
    int inventory_selection_index;
    int spell_selection_index;
    const char* active_service_entity_id;
    int service_selection_index;
    U2PersistentEntityState persistent_entities[U2_MAX_ENTITIES];
    U2CombatState combat;
    U2SpellTargetingState spell_targeting;
    U2SpellProjectileState spell_projectile;
    U2MessageState message;
} U2GameSession;

static void u2_inventory_clear(U2Inventory* inventory) {
    memset(inventory, 0, sizeof(*inventory));
}

static bool u2_inventory_add(U2Inventory* inventory, Item* item, size_t amount) {
    if (inventory == NULL || item == NULL || amount == 0) {
        return false;
    }

    if (item->stackable) {
        for (int i = 0; i < inventory->count; ++i) {
            if (inventory->slots[i].item == item) {
                inventory->slots[i].amount += amount;
                return true;
            }
        }
    }

    if (inventory->count >= U2_MAX_INVENTORY_SLOTS) {
        return false;
    }

    inventory->slots[inventory->count].item = item;
    inventory->slots[inventory->count].amount = amount;
    inventory->count++;
    return true;
}

static bool u2_inventory_add_by_id(U2Inventory* inventory, const char* item_id, size_t amount) {
    return u2_inventory_add(inventory, items_get(item_id), amount);
}

static bool u2_inventory_remove_at(U2Inventory* inventory, int index, size_t amount) {
    if (inventory == NULL || index < 0 || index >= inventory->count || amount == 0) {
        return false;
    }

    if (inventory->slots[index].item == NULL || inventory->slots[index].amount < amount) {
        return false;
    }

    inventory->slots[index].amount -= amount;
    if (inventory->slots[index].amount > 0) {
        return true;
    }

    for (int i = index; i < inventory->count - 1; ++i) {
        inventory->slots[i] = inventory->slots[i + 1];
    }

    memset(&inventory->slots[inventory->count - 1], 0, sizeof(inventory->slots[inventory->count - 1]));
    inventory->count--;
    return true;
}

static void u2_session_clear_scene_entities(U2GameSession* session) {
    memset(session->scene_entities, 0, sizeof(session->scene_entities));
}

static void u2_session_clear_combat(U2GameSession* session) {
    memset(&session->combat, 0, sizeof(session->combat));
}

static void u2_session_clear_spell_targeting(U2GameSession* session) {
    memset(&session->spell_targeting, 0, sizeof(session->spell_targeting));
}

static void u2_session_clear_spell_projectile(U2GameSession* session) {
    memset(&session->spell_projectile, 0, sizeof(session->spell_projectile));
}

static void u2_session_clear_message(U2GameSession* session) {
    session->message.visible = false;
    session->message.modal = false;
    session->message.timer = 0.0f;
    session->message.reveal_progress = 0.0f;
    session->message.visible_chars = 0;
    session->message.text[0] = '\0';
}

static void u2_session_show_message(U2GameSession* session, bool modal, float timer, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(session->message.text, sizeof(session->message.text), fmt, args);
    va_end(args);

    session->message.visible = true;
    session->message.modal = modal;
    session->message.timer = timer;
    session->message.reveal_progress = 0.0f;
    session->message.visible_chars = 0;
}

static bool u2_session_input_locked(const U2GameSession* session) {
    return session->panel != U2_PANEL_NONE ||
        session->spell_targeting.active ||
        session->spell_projectile.active ||
        (session->message.visible && session->message.modal);
}

static void u2_session_init(U2GameSession* session) {
    memset(session, 0, sizeof(*session));
    snprintf(session->player_name, sizeof(session->player_name), "Traveler");
    session->chosen_class = CLASS_NONE;
    session->flow_state = U2_FLOW_TITLE;
    session->scene_type = U2_SCENE_OVERWORLD;
    session->panel = U2_PANEL_NONE;
    session->active_service_entity_id = NULL;
    session->service_selection_index = 0;
    session->title_selection_index = 0;
    session->class_selection_index = 0;
    session->inventory_selection_index = 0;
    session->spell_selection_index = 0;
    u2_inventory_clear(&session->inventory);
    u2_session_clear_scene_entities(session);
    memset(session->persistent_entities, 0, sizeof(session->persistent_entities));
    u2_session_clear_combat(session);
    u2_session_clear_spell_targeting(session);
    u2_session_clear_spell_projectile(session);
    u2_session_clear_message(session);
}

#endif // ULTIMATUM_SESSION_H
