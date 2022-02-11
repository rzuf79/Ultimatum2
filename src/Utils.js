var Utils = {
	addText: function(parent, text, x, y) {
		x = x || 0;
		y = y || 0;

		var textEntity = new Entity("Bitmap Text", x, y);
		var textComponent = textEntity.addComponent(new ComponentBitmapText("font8", "IO CHAOS (i tak dalej)"));
		parent.add(textEntity);
		return textComponent;
	}
};