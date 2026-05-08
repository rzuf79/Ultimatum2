#ifndef REG_H
#define REG_H

#define LOCATION_CANVAS_SIZE 140
#define ENTITY_MAX_ANIMS 4

typedef enum {
	STATE_MENU = 0,
	STATE_GAME,
	STATE_EDITOR,
} GameState;

typedef enum {
	INGAME_NONE = 0,
	INGAME_ENTITY_SELECT,
	INGAME_STATS,
} IngameState;

typedef enum {
	STATS_CHARACTER = 0,
	STATS_INVENTORY,
	STATS_SPELLS,
	STATS_JOURNAL,
	STATS_END
} StatsState;

typedef enum {
	N = 0,
	S,
	W,
	E,
	U,
	D
} Direction;

GameState game_state = STATE_GAME;
IngameState ingame_state = INGAME_NONE;

#endif // REG_H
