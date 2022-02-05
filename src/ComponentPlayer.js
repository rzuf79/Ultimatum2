function ComponentPlayer(game, camera, x, y) {
    this.name = "Player";
    this.entity = null;

    this.character = null;
    this.sprite = null;

    this.startX = x;
    this.startY = y;

    this.movementTimer = 0.0;
    this.lastMoveDir = "";

    this.create = function() {
        this.character = this.entity.addComponent(new ComponentCharacter(game, "thief"));
        this.sprite = this.character.sprite;

        this.character.setPosition(this.startX, this.startY);
        this.character.looped.subscribe(this.onLooped, this);
    }

    this.destroy = function() {
        //
    }

    this.draw = function() {
        //
    }

    this.update = function() {
        var moveDir = "";
        if (chao.pressed[chao.KEY_UP]) {
            moveDir = "n";
        }
        if (chao.pressed[chao.KEY_DOWN]) {
            moveDir = "s";
        }
        if (chao.pressed[chao.KEY_LEFT]) {
            moveDir = "w";
        }
        if (chao.pressed[chao.KEY_RIGHT]) {
            moveDir = "e";
        }

        if (moveDir != this.lastMoveDir) {
            this.movementTimer = 0.0;
        }
        this.lastMoveDir = moveDir;

        var repeatRate = Reg.MOVEMENT_REPEAT_RATE;
        if (chao.pressed[chao.KEY_LSHIFT] || chao.pressed[chao.KEY_RSHIFT]) {
            repeatRate *= 0.5;
        }

        if (moveDir.length > 0) {
            if (this.movementTimer > 0.0) {
                this.movementTimer -= chao.getTimeDelta();
            } else {
                var collider = this.character.move(moveDir);
                if (typeof collider === "string" || collider instanceof String) {
                    chao.log("Ouch!");
                }

                this.movementTimer = repeatRate;
            }
        }
    }

    this.onLooped = function(loopBorder) {
        switch (loopBorder) {
            case "n": camera.entity.y -= game.location.locationSize.y; break;
            case "s": camera.entity.y += game.location.locationSize.y; break;
            case "w": camera.entity.x -= game.location.locationSize.x; break;
            case "e": camera.entity.x += game.location.locationSize.x; break;
        }
    }

}