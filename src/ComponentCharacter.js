function ComponentCharacter(game, tileName){
	this.name 				= "Character";
	this.entity 			= null;

	this.game				= game;

	this.tileName 			= tileName;
	this.sprite				= null;

	this.x 					= 0;
	this.y 					= 0;

	this.create = function(){
		this.sprite = this.entity.addComponent(new ComponentImage());
		this.sprite.setImage(chao.createImage(undefined, Reg.TILE_W, Reg.TILE_H));

		var tileRect	= {
			x: Tiles[this.tileName].x * Reg.TILE_W,
			y: Tiles[this.tileName].y * Reg.TILE_H,
			width: Reg.TILE_W,
			height: Reg.TILE_H
		}

		chao.drawImagePart(this.sprite.image, chao.getImage("tiles"), 0, 0, tileRect);
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

	this.updatePosition = function(){
		this.entity.x = this.x * Reg.TILE_W;
		this.entity.y = this.y * Reg.TILE_H;
	}

	this.setPosition = function(x, y){
		this.x = x;
		this.y = y;
		this.updatePosition();
	}

	this.move = function(dir){
		var newPos = { x: this.x, y: this.y };
		switch(dir){
			case "n": newPos.y --; break;
			case "s": newPos.y ++; break;
			case "w": newPos.x --; break;
			case "e": newPos.x ++; break;
		}
		
		if(this.game.location.isPassable(newPos.x, newPos.y)){
			this.x = newPos.x;
			this.y = newPos.y;
			this.updatePosition();
			return true;
		}

		return this.game.location.getTileName(newPos.x, newPos.y);
	}

}