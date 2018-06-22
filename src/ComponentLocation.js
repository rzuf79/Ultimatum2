function ComponentLocation(game){
	this.name 				= "Location";
	this.entity 			= null;

	this.imageLocation 		= null;

	this.tiles 				= [];

	this.tileNames			= [];
	this.tileSymbols		= [];

	this.create = function(){
		this.imageLocation = this.entity.addComponent(new ComponentImage());

		this.tileNames 		= Object.keys(Maps.symbols);
		this.tileSymbols 	= Object.values(Maps.symbols);

		var tileset = chao.getImage("tiles");

		for(var i = 0; i < this.tileNames.length; ++i){
			var newTile 	= chao.createImage(this.tileNames[i], Reg.TILE_W, Reg.TILE_H);
			var tileRect	= {
				x: Tiles[this.tileNames[i]].x * Reg.TILE_W,
				y: Tiles[this.tileNames[i]].y * Reg.TILE_H,
				width: Reg.TILE_W,
				height: Reg.TILE_H
			}

			chao.drawImagePart(newTile, tileset, 0, 0, tileRect);
			chao.tintImage(newTile, Tiles[this.tileNames[i]].color);

			this.tiles.push(newTile);
		}

	}

	this.destroy = function(){
		//
	}

	this.draw = function(x, y, alpha){
		//
	}

	this.update = function(){
		//
	}

	this.loadLocation = function(locationName){
		var newLocation 	= Maps[locationName];
		var width 			= newLocation.map[0].length;
		var height 			= newLocation.map.length;
		var locationCanvas 	= chao.createImage(undefined, width*Reg.TILE_W, height*Reg.TILE_H);

		this.imageLocation.setImage(locationCanvas);

		for(var i = 0; i < width; ++i){
			for(var j = 0; j < height; ++j){
				var symbol  = newLocation.map[j][i];
				var image 	= this.tiles[this.tileSymbols.indexOf(symbol)];

				chao.drawImage(locationCanvas, image, i*Reg.TILE_W, j*Reg.TILE_H);
			}
		}

		this.imageLocation.setImage(locationCanvas);
	}

}