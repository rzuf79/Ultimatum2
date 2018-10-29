function ComponentPlayer(game, x, y){
	this.name 				= "Player";
	this.entity 			= null;

	this.game 				= game;

	this.character 			= null;
	this.sprite 			= null;

	this.startX				= x;
	this.startY 			= y;

	this.create = function(){
		this.character = this.entity.addComponent(new ComponentCharacter(this.game, "thief"));
		this.sprite = this.character.sprite;

		this.character.setPosition(this.startX, this.startY);
	}

	this.destroy = function(){
		//
	}

	this.draw = function(x, y, alpha){
		//
	}

	this.update = function(){
		var moveDir = "";
		if(chao.justPressed[chao.KEY_UP]){
			moveDir = "n";
		}else if(chao.justPressed[chao.KEY_DOWN]){
			moveDir = "s";
		}else if(chao.justPressed[chao.KEY_LEFT]){
			moveDir = "w";
		}else if(chao.justPressed[chao.KEY_RIGHT]){
			moveDir = "e";
		}

		if(moveDir.length > 0){
			var collider = this.character.move(moveDir);
			if(typeof collider === "string" || collider instanceof String){
				chao.log("Ouch!");
			}
		}
	}

}