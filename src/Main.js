chao.init(320, 240, chao.SCALING_MODE_KEEP_RATIO, "canvas");
chao.backgroundColor = "#000000";
chao.setImagesSmoothing(false);

// ----- Them assets
chao.loadImage("tiles", "assets/images/tiles.png");

// -----

chao.switchState(new StateGame());