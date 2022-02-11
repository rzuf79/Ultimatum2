function ComponentBitmapText(fontImage, text){
	this.name = "BitmapText";
	this.entity = null;

	this.image = null;
	this.glyphSize = {x:0, y:0};

	this._text = text;
	this._color = 0xff000000;

	this.create = function(){
		fontImage = chao.getImage(fontImage);
		this.glyphSize.x = fontImage.width / 16;
		this.glyphSize.y = fontImage.height / 16;
		this.changeText(text);
	}

	this.draw = function(){
		if (this.image) {
			var entity = this.entity;
			var drawAlpha = entity.screenAlpha;
			var drawX = entity.screenX - (entity.screenWidth * entity.pivotX);
			var drawY = entity.screenY - (entity.screenHeight * entity.pivotY);

			chao.drawImage(chao.canvas, this.image,
				drawX, drawY,
				drawAlpha,
				1.0, 1.0,
				entity.screenRotation,
				entity.pivotX, entity.pivotY);
		}
	}

	this.changeText = function() {
		var i, j;
		var lines = this.text.split("\n");
		var imageSize = { x : 0, y : lines.length * this.glyphSize.y };
		var longestLineLength = -1;
		for (i = 0; i < lines.length; ++i) {
			if (lines[i].length > longestLineLength) {
				longestLineLength = lines[i].length;
				imageSize.x = longestLineLength * this.glyphSize.x;
			}
		}
		this.image = chao.createImage(undefined, imageSize.x, imageSize.y);
		for (i = 0; i < lines.length; ++i) {
			for (j = 0; j < lines[i].length; ++j) {
				var idx = lines[i].charCodeAt(j);
				var glyphX = idx % 16;
				var glyphY = Math.floor(idx / 16);
				var rect = chao.makeRect(
					glyphX * this.glyphSize.x, glyphY * this.glyphSize.y,
					this.glyphSize.x, this.glyphSize.y);
				chao.drawImagePart(this.image, fontImage,
					j * this.glyphSize.x, i * this.glyphSize.y,
					rect);
			}
		}

	}

}
ComponentBitmapText.prototype = {
	// This is a serious mess, even by the messiness of all the other things here. Is there a better way of doing this?
	get text() {
		return this._text;
	},
	set text(newText) {
		if (newText !== this.text) {
			this._text = newText;
			this.changeText();
		}
	}
};