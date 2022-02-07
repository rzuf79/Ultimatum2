/**
 * Tiles data.
 *
 * x - X position on the tileset.
 * y - Y position on the tileset.
 * color - Color of the tile.
 */
const Tiles = {

	grass: {
		x: 0,
		y: 0,
		color: Reg.colorPico8DarkGreen,
		passable: true
	},

	forest: {
		x: 1,
		y: 0,
		color: Reg.colorPico8DarkGreen,
		passable: true
	},

	water: {
		x: 2,
		y: 0,
		color: Reg.colorPico8Blue,
		passable: false
	},

	mountains: {
		x: 3,
		y: 0,
		color: Reg.colorPico8White,
		passable: false
	},

	town: {
		x: 4,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	castle: {
		x: 5,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	portal: {
		x: 6,
		y: 0,
		color: Reg.colorPico8Blue,
		passable: true
	},

	village: {
		x: 7,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	signpost: {
		x: 8,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	dungeon: {
		x: 9,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	tower: {
		x: 10,
		y: 0,
		color: Reg.colorPico8White,
		passable: true
	},

	swamp: {
		x: 11,
		y: 0,
		color: Reg.colorPico8DarkPurple,
		passable: true
	},

	wall: {
		x: 12,
		y: 0,
		color: Reg.colorPico8White,
		passable: false
	},

	road: {
		x: 13,
		y: 0,
		color: Reg.colorPico8DarkPurple,
		passable: true
	},

	forceField: {
		x: 14,
		y: 0,
		color: Reg.colorPico8DarkPurple,
		passable: true
	},

	door: {
		x: 11,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	deskLeft: {
		x: 0,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	desk: {
		x: 1,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	deskRight: {
		x: 2,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	A: {
		x: 3,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	B: {
		x: 4,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	C: {
		x: 5,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	D: {
		x: 6,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	E: {
		x: 7,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	F: {
		x: 8,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	G: {
		x: 9,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	H: {
		x: 10,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	I: {
		x: 11,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	J: {
		x: 12,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	K: {
		x: 13,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	L: {
		x: 14,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	M: {
		x: 15,
		y: 1,
		color: Reg.colorPico8White,
		passable: false
	},

	N: {
		x: 0,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	O: {
		x: 1,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	P: {
		x: 2,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	Q: {
		x: 3,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	R: {
		x: 4,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	S: {
		x: 5,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	T: {
		x: 6,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	U: {
		x: 7,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	V: {
		x: 8,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	W: {
		x: 9,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	X: {
		x: 10,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	Y: {
		x: 11,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	Z: {
		x: 12,
		y: 2,
		color: Reg.colorPico8White,
		passable: false
	},

	ladderUp: {
		x: 13,
		y: 2,
		color: Reg.colorPico8White,
		passable: true
	},

	ladderDown: {
		x: 14,
		y: 2,
		color: Reg.colorPico8White,
		passable: true
	},

	horse: {
		x: 0,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	ship: {
		x: 1,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	plane: {
		x: 2,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	shuttle: {
		x: 3,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	sword: {
		x: 4,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	shield: {
		x: 5,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	chest: {
		x: 6,
		y: 3,
		color: Reg.colorPico8White,
		passable: true
	},

	thief: {
		x: 7,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	wizard: {
		x: 8,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	cleric: {
		x: 9,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	warrior: {
		x: 10,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	jester: {
		x: 11,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	merchant: {
		x: 12,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	guard: {
		x: 13,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	guard2: {
		x: 14,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	british: {
		x: 15,
		y: 3,
		color: Reg.colorWhite,
		passable: false
	},

	orc: {
		x: 0,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	daemon: {
		x: 1,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	devil: {
		x: 2,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	balron: {
		x: 4,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	serpent: {
		x: 4,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	beholder: {
		x: 5,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	ghost: {
		x: 6,
		y: 4,
		color: Reg.colorPico8White,
		passable: false
	},

	viper: {
		x: 7,
		y: 4,
		color: Reg.colorWhite,
		passable: false
	},

	gremlin: {
		x: 8,
		y: 4,
		color: Reg.colorWhite,
		passable: false
	},

	daemonBig: {
		x: 9,
		y: 4,
		color: Reg.colorWhite,
		passable: false
	},

	balrog: {
		x: 10,
		y: 4,
		color: Reg.colorWhite,
		passable: false
	},



}