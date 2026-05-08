#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 128

typedef enum {
	ITEM_TYPE_KEY = 0,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_SHIELD,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_HAT,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_CONSUMABLE,
	ITEM_TYPE_NONE,
} ItemType;

typedef struct {
	char* id;
	char* name;
	ItemType type;
	bool stackable;
	char* damage;
	int armor_class;
} Item;

typedef struct {
	Item* item;
	size_t amount;
} ItemSlot;

typedef struct {
	ItemSlot* items;
	size_t length;
	size_t capacity;
} ItemsArray;

int items_count = 0;
Item items[MAX_ITEMS];

Item* items_get(const char* id) {
	if (id == NULL) {
		printf("Passing a NULL to items_get huh?\n");
		return NULL;
	}
	for (int i = 0; i < items_count; ++i) {
		if (strcmp(id, items[i].id) == 0) {
			return &items[i];
		}
	}
	return NULL;
}

void items_init() {
	int i = 0;

	items[i++] = (Item) {
		.id = "short_sword",
		.name = "Short Sword",
		.type = ITEM_TYPE_WEAPON,
		.stackable = false,
		.damage = "1d8",
		.armor_class = 0,
	};
	items[i++] = (Item) {
		.id = "dagger",
		.name = "Dagger",
		.type = ITEM_TYPE_WEAPON,
		.stackable = false,
		.damage = "1d6",
		.armor_class = 0,
	};
	items[i++] = (Item) {
		.id = "oak_staff",
		.name = "Oak Staff",
		.type = ITEM_TYPE_WEAPON,
		.stackable = false,
		.damage = "1d4",
		.armor_class = 0,
	};
	items[i++] = (Item) {
		.id = "mace",
		.name = "Mace",
		.type = ITEM_TYPE_WEAPON,
		.stackable = false,
		.damage = "1d6",
		.armor_class = 0,
	};
	items[i++] = (Item) {
		.id = "leather_armor",
		.name = "Leather Armor",
		.type = ITEM_TYPE_ARMOR,
		.stackable = false,
		.damage = NULL,
		.armor_class = 1,
	};
	items[i++] = (Item) {
		.id = "traveler_robe",
		.name = "Traveler Robe",
		.type = ITEM_TYPE_ARMOR,
		.stackable = false,
		.damage = NULL,
		.armor_class = 0,
	};
	items[i++] = (Item) {
		.id = "wooden_shield",
		.name = "Wooden Shield",
		.type = ITEM_TYPE_SHIELD,
		.stackable = false,
		.damage = NULL,
		.armor_class = 1,
	};
		items[i++] = (Item) {
			.id = "trail_ration",
			.name = "Trail Ration",
			.type = ITEM_TYPE_FOOD,
			.stackable = true,
			.damage = NULL,
			.armor_class = 0,
		};
		items[i++] = (Item) {
			.id = "lesser_healing_potion",
			.name = "Lesser Healing Potion",
			.type = ITEM_TYPE_CONSUMABLE,
			.stackable = true,
			.damage = NULL,
			.armor_class = 0,
		};
		items[i++] = (Item) {
			.id = "focus_tonic",
			.name = "Focus Tonic",
			.type = ITEM_TYPE_CONSUMABLE,
			.stackable = true,
			.damage = NULL,
			.armor_class = 0,
		};
		items[i++] = (Item) {
			.id = "spellbook",
			.name = "Spellbook",
		.type = ITEM_TYPE_KEY,
		.stackable = false,
		.damage = NULL,
		.armor_class = 0,
	};

	items_count = i;
}

#endif // ITEMS_H
