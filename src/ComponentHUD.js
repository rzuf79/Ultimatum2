function ComponentHUD(){
	this.FRAME_BORDER_SIZE = 4;
	this.FRAME_WIDTH = (8 * 12) + 8;

	this.name = "HUD";
	this.entity = null;

	this.frame = null; // ComponentFrame

	this.create = function(){
		this.entity.makeFullscreen();
		var entity = this.entity;
		this.frame = entity.addWithComponent("Frame", new ComponentFrame("frame", 4));
		this.frame.entity.width = this.FRAME_WIDTH;
		this.frame.entity.height = chao.screenHeight + 8;
		this.frame.entity.alignToParent(1.0, 0.0, 1.0, 0.0, 4, -4, true);

		// var stats = Utils.addText(entity, "HP 16/16  MP0/0  EXP 0/100", 1, 1, 1);
	}

	this.destroy = function(){
		//
	}

	this.resize = function() {
		this.frame.entity.height = chao.screenHeight + 8;
	}

	this.draw = function(){
		//
	}

	this.update = function(){
		//
	}

	this.getVisibileFrameWidth = function() {
		return this.FRAME_WIDTH - this.FRAME_BORDER_SIZE;
	}


}