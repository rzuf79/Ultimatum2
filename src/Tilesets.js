var Tilesets = {

	getCurrent: function() {
		return Tilesets.classic;
	},

	getColor: function(tileName) {
		var tileset = Tilesets.getCurrent();
		var colorId = tileset.colors[tileName] || 0;
		return tileset.palette[colorId];
	},

	classic: {
		path: "assets/images/tiles.png",
		
		width: 16,
		height: 16,

		palette: [
			Reg.colorPico8White,
			Reg.colorPico8DarkGreen,
			Reg.colorPico8Blue,
			Reg.colorPico8DarkPurple,
		],

		colors: {
			grass: 1,
			forest: 1,
			water: 2,
			portal: 2,
			swamp: 3,
			road: 3,
			forceField: 3,
		}
	},
}