#ifndef D20_H
#define D20_H

#include "die.h"
#include "items.h"
#include <math.h>
#include <stdint.h>

#define MAX_CHARACTER_ITEMS  12

typedef enum {
	STAT_STR = 0,
	STAT_DEX,
	STAT_MIND
} Stat;

typedef enum {
	CLASS_FIGHTER = 0,
	CLASS_ROGUE,
	CLASS_MAGI,
	CLASS_CLERIC,
	CLASS_NONE
} CharacterClass;

typedef enum {
	SKILL_PHYSICAL = 0,
	SKILL_SUBTERFUGE,
	SKILL_KNOWLEDGE,
	SKILL_COMMUNICATION
} Skill;

typedef enum {
	BONUS_MELEE = 0,
	BONUS_RANGED,
	BONUS_MAGIC
} Bonus;

typedef enum {
	SLOT_HAND_RIGHT = 0,
	SLOT_HAND_LEFT,
	SLOT_BODY,
	SLOT_HEAD,
	SLOT_ACCESSORY,
	SLOT_END,
} Slot;

typedef struct {
    CharacterClass class;

    int base_hp;
    int base_mp;
    char* base_damage;
    int armor_class;

	int hp;
	int mp;
	int stats[3];
	int skills[4];

	int level;
	int exp;
	int stat_points;

	Item* equipment[SLOT_END];
} Character;

int d20_get_damage_bonus(Character* chara);
void d20_get_damage_string(Character* chara, Slot slot, char* out, size_t out_size);

int d20_get_exp_for_level(int level) {
    if (level <= 1) return 0;
    if (level <= 7) {
        return 1000 * (1 << (level - 1));
    }
    return 128000 + (long long)(level - 7) * 120000;
}

void d20_advance_level(Character* chara) {
    chara->level ++;
    if (chara->level % 3 == 0) {
        chara->stat_points += 3;
    }
}

int d20_get_stat_bonus(Character* chara, Stat stat) {
    float base = (float)chara->stats[stat];
    float value = max(0.0f, floor((base - 10.0f) / 2.0f));
    return (int)value;
}

int d20_get_skill(Character* chara, Skill skill, bool with_bonuses) {
    int value = chara->skills[skill];
    if (with_bonuses) {
        if (skill == SKILL_PHYSICAL && chara->class == CLASS_FIGHTER) value += 3;
        if (skill == SKILL_SUBTERFUGE && chara->class == CLASS_ROGUE) value += 3;
        if (skill == SKILL_KNOWLEDGE && chara->class == CLASS_MAGI) value += 3;
        if (skill == SKILL_COMMUNICATION && chara->class == CLASS_CLERIC) value += 3;
    }
    return value;
}

int d20_get_max_hp(Character* chara) {
    return chara->base_hp + chara->stats[STAT_STR] + (int)floorf((float)chara->level * 3.5f);
}

int d20_get_max_mp(Character* chara) {
    return chara->base_mp + chara->stats[STAT_MIND] + (int)floorf((float)chara->level * 3.5f);
}

float d20_get_hp_percentage(Character* chara) {
    return (float)chara->hp / d20_get_max_hp(chara);
}

float d20_get_mp_percentage(Character* chara) {
    return (float)chara->mp / d20_get_max_mp(chara);
}

ItemType d20_get_item_type(Character* chara, Slot slot) {
	return chara->equipment[slot] ? chara->equipment[slot]->type : ITEM_TYPE_NONE;
}

float d20_get_weapon_damage(Character* chara, Slot slot, bool is_critic) {
    char damage[64];

    d20_get_damage_string(chara, slot, damage, sizeof(damage));
    if (is_critic) {
        return roll_dice_max(damage);
    }
    return roll_dice(damage);
}

bool d20_is_dual_wielding(Character* chara) {
    return chara->equipment[SLOT_HAND_RIGHT] && chara->equipment[SLOT_HAND_RIGHT]->type == ITEM_TYPE_WEAPON &&
        chara->equipment[SLOT_HAND_LEFT] && chara->equipment[SLOT_HAND_LEFT]->type == ITEM_TYPE_WEAPON;
}

int d20_get_attack_bonus(Character* chara, Bonus bonus) {
    switch (bonus) {
        case BONUS_MELEE:
        {
            int bonus = d20_get_stat_bonus(chara, STAT_STR) + chara->level;
            if (d20_is_dual_wielding(chara)) {
                bonus -= 2;
            }
            return bonus;
        }
        case BONUS_RANGED:
            return d20_get_stat_bonus(chara, STAT_DEX) + chara->level;
        case BONUS_MAGIC:
            return d20_get_stat_bonus(chara, STAT_MIND) + chara->level;
    }
    return 0;
}

void d20_get_damage_string(Character* chara, Slot slot, char* out, size_t out_size) {
    char* damage = chara->base_damage;
    if (chara->equipment[slot] != NULL) {
        damage = chara->equipment[slot]->damage;
    }
    int total_bonus = d20_get_stat_bonus(chara, STAT_STR) + d20_get_damage_bonus(chara);
    if (out == NULL || out_size == 0) {
        return;
    }
    if (damage == NULL || damage[0] == '\0') {
        snprintf(out, out_size, "-");
        return;
    }
    if (total_bonus != 0) {
        const char* plus = strchr(damage, '+');
        const char* minus = strchr(damage, '-');
        const char* op = plus != NULL ? plus : minus;
        if (op != NULL) {
            int mod = atoi(op);
            size_t len = (size_t)(op - damage);
            mod += total_bonus;
            if (mod != 0) {
                if (mod > 0) {
                    snprintf(out, out_size, "%.*s+%d", (int)len, damage, mod);
                } else {
                    snprintf(out, out_size, "%.*s%d", (int)len, damage, mod);
                }
                return;
            } else {
                snprintf(out, out_size, "%.*s", (int)len, damage);
                return;
            }
        } else {
            char sign = total_bonus > 0 ? '+' : '-';
            snprintf(out, out_size, "%s%c%d", damage, sign, total_bonus);
            return;
        }

    }
    snprintf(out, out_size, "%s", damage);
}

int d20_get_damage_bonus(Character* chara) {
    if (chara->class == CLASS_FIGHTER) {
        return 1 + (int)floor((float)chara->level / 5.0f);
    }
    return 0;
}

int d20_get_number_of_attacks(Character* chara) {
    return 1 + floor((float)d20_get_attack_bonus(chara, BONUS_MELEE) / 5.0f);
}

int d20_get_armor_class(Character* chara) {
    int ac = chara->armor_class + d20_get_stat_bonus(chara, STAT_DEX);
    for (int i = 0; i < SLOT_END; ++i) {
    	if (chara->equipment[i]) {
			ac += chara->equipment[i]->armor_class;
		}
    }
    return ac;
}

void d20_deal_damage(Character* chara, int amount) {
    chara->hp -= amount;
}

int d20_perform_single_attack(Character* attacker, Character* target, Slot slot, int attack_bonus) {
    int roll = roll_die(1, 20);
    bool is_crit = roll == 20;
    if (is_crit || roll + attack_bonus > d20_get_armor_class(target)) {
        int damage = d20_get_weapon_damage(attacker, slot, is_crit);
        if (damage > 0) {
            d20_deal_damage(target, damage);
            return damage;
        } else {
            return 0; // no damage
        }
    }
    return -1; // missed!
}

void d20_attack(Character* attacker, Character* target, void (*attack_cb)(int)) {
    int attack_bonus = d20_get_attack_bonus(attacker, BONUS_MELEE);
    for (int i = 0; i < d20_get_number_of_attacks(attacker); ++i) {
        if (target->hp <= 0) {
            break;
        }
        int right_handed = false;
        if (d20_get_item_type(attacker, SLOT_HAND_RIGHT) != ITEM_TYPE_SHIELD) {
            int dmg = d20_perform_single_attack(attacker, target, SLOT_HAND_RIGHT, attack_bonus - (i * 5));
            attack_cb(dmg);
            right_handed = true;
        }
        bool double_attack = d20_get_item_type(attacker, SLOT_HAND_LEFT) != ITEM_TYPE_SHIELD && (d20_is_dual_wielding(attacker) || !right_handed);
        if (double_attack && target->hp > 0) {
            int dmg = d20_perform_single_attack(attacker, target, SLOT_HAND_LEFT, attack_bonus - (i * 5));
            attack_cb(dmg);
        }
    }
}

void d20_character_init(Character* chara) {
	chara->level = 0;
	chara->exp = 0;
	chara->stat_points = 0;
	chara->hp = d20_get_max_hp(chara);
	chara->mp = d20_get_max_mp(chara);
}

#endif // D20_H
