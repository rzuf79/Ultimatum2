chao.init(320, 240, chao.SCALING_MODE_EXTEND, "canvas");
chao.backgroundColor = "#4f4f4f";
chao.setImagesSmoothing(false);

// ----- Them assets
chao.loadImage("font8", "assets/images/cp437_8x8.png")
chao.loadImage("tiles", Tilesets.getCurrent().path);

// -----

chao.switchState(new StateGame());