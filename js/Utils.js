var Utils = {
	addText: function(parent, text, x, y, outlineSize) {
		x = x || 0;
		y = y || 0;
		outlineSize = outlineSize || 0;

		var textEntity = new Entity("Bitmap Text", x, y);
		var textComponent = textEntity.addComponent(new ComponentBitmapText("font8", text, outlineSize));
		textComponent.outlineSize = outlineSize;
		parent.add(textEntity);
		return textComponent;
	}
};