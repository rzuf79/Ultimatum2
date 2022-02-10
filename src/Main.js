chao.init(320, 240, chao.SCALING_MODE_EXTEND, "canvas");
chao.backgroundColor = "#000000";
chao.setImagesSmoothing(false);

// ----- Them assets
chao.loadImage("tiles", Tilesets.getCurrent().path);

// -----

chao.switchState(new StateGame());