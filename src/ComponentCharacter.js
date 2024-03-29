function ComponentCharacter(game, tileName) {
    this.name = "Character";
    this.entity = null;

    this.looped = chao.makeSignal(); // (loopedFrom - "n"/"s"/"w"/"e")

    this.tileName = tileName;
    this.sprite = null;

    this.x = 0;
    this.y = 0;

    this.create = function() {
        this.sprite = this.entity.addComponent(new ComponentSprite());
        this.sprite.setImage(chao.createImage(undefined, Tilesets.getCurrent().width, Tilesets.getCurrent().height));
        this.sprite.entity.setPivot(0, 0);

        var tileRect = {
            x: Tiles[this.tileName].x * Tilesets.getCurrent().width,
            y: Tiles[this.tileName].y * Tilesets.getCurrent().height,
            width: Tilesets.getCurrent().width,
            height: Tilesets.getCurrent().height
        }

        chao.drawImagePart(this.sprite.image, chao.getImage("tiles"), 0, 0, tileRect);
    }

    this.destroy = function() {
        //
    }

    this.draw = function() {
        //
    }

    this.update = function() {
        //
    }

    this.updatePosition = function() {
        this.entity.x = this.x * Tilesets.getCurrent().width;
        this.entity.y = this.y * Tilesets.getCurrent().height;
    }

    this.setPosition = function(x, y) {
        this.x = x;
        this.y = y;
        this.updatePosition();
    }

    this.move = function(dir) {
        var newPos = { x: this.x, y: this.y };
        switch (dir) {
            case "n":
                newPos.y--;
                break;
            case "s":
                newPos.y++;
                break;
            case "w":
                newPos.x--;
                break;
            case "e":
                newPos.x++;
                break;
        }

        if (game.location.isPassable(newPos.x, newPos.y)) {
            this.x = newPos.x;
            this.y = newPos.y;

            var loopBorder = "";
            if (this.x < 0) {
                this.x = 63;
                loopBorder = "w";
            } else if (this.x > 63) {
                this.x = 0;
                loopBorder = "e";
            }
            if (this.y < 0) {
                this.y = 63;
                loopBorder = "n";
            } else if (this.y > 63) {
                this.y = 0;
                loopBorder = "s";
            }

            if (loopBorder.length > 0) {
                this.looped.fire(loopBorder);
            }

            this.updatePosition();
            return true;
        }

        return game.location.getTileName(newPos.x, newPos.y);
    }

}