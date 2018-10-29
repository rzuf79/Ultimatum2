function ComponentWater(image){
	this.name 				= "Water";
	this.entity 			= null;

	this.sprite 			= null;
	this.timer				= 0;
	this.currentY			= 0;
	this.image 				= image;

	this.create = function(){
		this.sprite = this.entity.addComponent(new ComponentImage());
		this.sprite.setImage(chao.createImage(undefined, Reg.TILE_W, Reg.TILE_H));
	}

	this.update = function(){
		this.currentY += chao.getTimeDelta()*2;
		if(this.currentY >= Reg.TILE_H){
			this.currentY -= Reg.TILE_H;
		}

		chao.drawImage(this.sprite.image, this.image, 0, this.currentY);
		chao.drawImage(this.sprite.image, this.image, 0, this.currentY-Reg.TILE_H);
	}

}