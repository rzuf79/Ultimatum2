function ComponentLocation(game){
	this.name 				= "Location";
	this.entity 			= null;

	this.tiles 				= [];
	this.tileNames			= [];
	this.tileSymbols		= [];

	this.imageLocation 		= null;
	this.currentLocation	= null;
	this.specialTiles		= [];

	this.waterTileY			= 0;
	this.imageWater			= null;
	this.imageForceField	= null;

	this.create = function(){
		this.imageLocation = this.entity.addComponent(new ComponentSprite());
		this.imageLocation.entity.setPivot(0, 0);

		this.tileNames 		= Object.keys(Maps.symbols);
		this.tileSymbols 	= Object.values(Maps.symbols);

		var tilesAtlas = chao.getImage("tiles");

		for(var i = 0; i < this.tileNames.length; ++i){
			var newTile 	= chao.createImage(this.tileNames[i], Reg.TILE_W, Reg.TILE_H);
			var tileSprite	= chao.createImage(undefined, Reg.TILE_W, Reg.TILE_H);

			var tileRect	= {
				x: Tiles[this.tileNames[i]].x * Reg.TILE_W,
				y: Tiles[this.tileNames[i]].y * Reg.TILE_H,
				width: Reg.TILE_W,
				height: Reg.TILE_H
			}

			chao.drawImagePart(tileSprite, tilesAtlas, 0, 0, tileRect);
			chao.tintImage(tileSprite, Tiles[this.tileNames[i]].color);

			chao.clearToColor(newTile, chao.makeColor(0, 0, 0));
			chao.drawImage(newTile, tileSprite, 0, 0);

			this.tiles.push(newTile);
		}

		this.imageWater 		= chao.createImage("water_tile", Reg.TILE_W, Reg.TILE_H);
		this.imageForceField 	= chao.createImage("force_field_tile", Reg.TILE_W, Reg.TILE_H);
	}

	this.destroy = function(){
		//
	}

	this.draw = function(x, y, alpha){
		//
	}

	this.update = function(){
		// update animated tiles
		this.waterTileY += chao.getTimeDelta()*2;
		if(this.waterTileY >= Reg.TILE_H){
			this.waterTileY -= Reg.TILE_H;
		}

		var waterImage = chao.getImage("water");
		chao.drawImage(this.imageWater, waterImage, 0, this.waterTileY);
		chao.drawImage(this.imageWater, waterImage, 0, this.waterTileY-Reg.TILE_H);

		var forceFieldImage = chao.getImage("forceField");
		chao.drawImage(this.imageForceField, forceFieldImage, 0, this.waterTileY);
		chao.drawImage(this.imageForceField, forceFieldImage, 0, this.waterTileY-Reg.TILE_H);

	}

	this.loadLocation = function(locationName){

		for(var i = 0; i < this.specialTiles.length; ++i){
			this.entity.remove(this.specialTiles[i]);
		}
		this.specialTiles = [];

		var newLocation 	= Maps[locationName];
		var width 			= newLocation.map[0].length;
		var height 			= newLocation.map.length;
		var locationCanvas 	= chao.createImage(undefined, width*Reg.TILE_W, height*Reg.TILE_H);

		this.imageLocation.setImage(locationCanvas);

		for(var i = 0; i < width; ++i){
			for(var j = 0; j < height; ++j){
				var symbol  	= newLocation.map[j][i];
				var symbolIdx 	= this.tileSymbols.indexOf(symbol);

				if(symbolIdx == -1){
					chao.log("Unrecognized map symbol: \"" + symbol + "\"");
				}

				var image = this.tiles[symbolIdx];
				chao.drawImage(locationCanvas, image, i*Reg.TILE_W, j*Reg.TILE_H);
				
				// if(symbol === "~"){
				// 	this.addWater(i, j, this.imageWater);
				// } else if(symbol === "="){
				// 	this.addWater(i, j, this.imageForceField);
				// }
			}
		}

		this.imageLocation.setImage(locationCanvas);

		this.currentLocation = newLocation;
	}

	this.addWater = function(x, y, image){
		var waterTile = chao.helpers.createSprite(undefined, "Water", image, x*Reg.TILE_W, y*Reg.TILE_H);
		this.entity.add(waterTile.entity);
		this.specialTiles.push(waterTile.entity);
	}

	this.getTileSymbol = function(x, y){
		return this.currentLocation.map[y][x];
	}

	this.getTile = function(x, y){
		var idx = this.tileSymbols.indexOf(this.getTileSymbol(x, y));
		return Tiles[this.tileNames[idx]];
	}

	this.getTileName = function(x, y){
		var idx = this.tileSymbols.indexOf(this.getTileSymbol(x, y));
		return this.tileNames[idx];
	}

	this.isPassable = function(x, y){
		return this.getTile(x, y).passable;
	}

}