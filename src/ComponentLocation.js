function ComponentLocation(game, camera) {
    this.name = "Location";
    this.entity = null;

    this.tiles = [];
    this.tileNames = [];
    this.tileSymbols = [];

    this.imageLocation = null;
    this.locationSize = { x: 0, y: 0};
    this.currentLocation = null;

    this.animatedTileY = 0;
    this.imageWater = null;
    this.imageForceField = null;

    this.create = function() {
        this.locationSize.x = 64 * Reg.TILE_W;
        this.locationSize.y = 64 * Reg.TILE_H;
        this.imageLocation = chao.createImage(undefined, this.locationSize.x, this.locationSize.y);

        this.tileNames = Object.keys(Maps.symbols);
        this.tileSymbols = Object.values(Maps.symbols);

        var tilesAtlas = chao.getImage("tiles");

        for (var i = 0; i < this.tileNames.length; ++i) {
            var newTile = chao.createImage(this.tileNames[i], Reg.TILE_W, Reg.TILE_H);
            var tileSprite = chao.createImage(undefined, Reg.TILE_W, Reg.TILE_H);

            var tileRect = {
                x: Tiles[this.tileNames[i]].x * Reg.TILE_W,
                y: Tiles[this.tileNames[i]].y * Reg.TILE_H,
                width: Reg.TILE_W,
                height: Reg.TILE_H
            }

            chao.drawImagePart(tileSprite, tilesAtlas, 0, 0, tileRect);
            chao.tintImage(tileSprite, Tiles[this.tileNames[i]].color);

            chao.clearToColor(newTile, chao.makeColor(0, 0, 0));
            chao.drawImage(newTile, tileSprite, 0, 0);

            this.tiles.push(newTile);
        }

        this.imageWater = chao.createImage("water_tile", Reg.TILE_W, Reg.TILE_H);
        this.imageForceField = chao.createImage("force_field_tile", Reg.TILE_W, Reg.TILE_H);
    }

    this.destroy = function() {
        //
    }

    this.draw = function() {
        var entity = this.entity;
        var screenPos = { x : entity.screenX, y : entity.screenY };

        // main location image
        chao.drawImage(chao.canvas, this.imageLocation, screenPos.x, screenPos.y);

        if (this.currentLocation.looped) {
            var size = this.locationSize;
            
            var rects = [ 
                chao.makeRect(0, -size.y, size.x, size.y),
                chao.makeRect(0, size.y, size.x, size.y),
                chao.makeRect(-size.x, 0, size.x, size.y),
                chao.makeRect(size.x, 0, size.x, size.y),
            ];
            
            for (var i = 0; i < rects.length; ++i) {
                if (camera.isRectVisible(rects[i])) {
                    chao.drawImage(chao.canvas, this.imageLocation, screenPos.x + rects[i].x, screenPos.y + rects[i].y );
                }
            }
        }

        // draw animated tiles
        {
            var visibleRect = camera.getVisibleRect();
            visibleRect.x = Math.ceil(visibleRect.x / Reg.TILE_W);
            visibleRect.y = Math.ceil(visibleRect.y / Reg.TILE_H);
            visibleRect.width = Math.ceil(visibleRect.width / Reg.TILE_W);
            visibleRect.height = Math.ceil(visibleRect.height / Reg.TILE_H);
            for(x = visibleRect.x - 1; x < visibleRect.x + visibleRect.width; ++x) {
                var tx = x >= 0 ? x : x + 64;
                tx %= 64;
                var drawX = tx * Reg.TILE_W;
                for(y = visibleRect.y - 1; y < visibleRect.y + visibleRect.height; ++y) {
                    var ty = y >= 0 ? y : y + 64;
                    ty %= 64;
                    var drawY = ty * Reg.TILE_H;
                    switch (this.currentLocation.map[ty][tx]) {
                        case '~': 
                            chao.drawImage(this.imageLocation, this.imageWater, drawX, drawY);
                            break;
                    }
                }
            }
        }

    }

    this.update = function() {
        // update animated tiles
        this.animatedTileY += chao.getTimeDelta() * 2;
        if (this.animatedTileY >= Reg.TILE_H) {
            this.animatedTileY -= Reg.TILE_H;
        }

        var waterImage = chao.getImage("water");
        chao.drawImage(this.imageWater, waterImage, 0, this.animatedTileY);
        chao.drawImage(this.imageWater, waterImage, 0, this.animatedTileY - Reg.TILE_H);

        var forceFieldImage = chao.getImage("forceField");
        chao.drawImage(this.imageForceField, forceFieldImage, 0, this.animatedTileY);
        chao.drawImage(this.imageForceField, forceFieldImage, 0, this.animatedTileY - Reg.TILE_H);

    }

    this.loadLocation = function(locationName) {
        var newLocation = Maps[locationName];
        var width = newLocation.map[0].length;
        var height = newLocation.map.length;
        
        this.imageLocation = chao.createImage(undefined, width * Reg.TILE_W, height * Reg.TILE_H);

        // this.imageLocation.setImage(locationCanvas);

        for (var x = 0; x < width; ++x) {
            for (var y = 0; y < height; ++y) {
                var symbol = newLocation.map[y][x];
                var symbolIdx = this.tileSymbols.indexOf(symbol);

                if (symbolIdx == -1) {
                    chao.log("Unrecognized map symbol: \"" + symbol + "\"");
                }

                var image = this.tiles[symbolIdx];
                chao.drawImage(this.imageLocation, image, x * Reg.TILE_W, y * Reg.TILE_H);

                // if(symbol === "~"){
                //     animatedTileData.image = this.imageWater;
                //     this.animatedTiles.push(animatedTileData);
                // } else if(symbol === "="){
                // 	animatedTileData.image = this.imageForceField;
                //     this.animatedTiles.push(animatedTileData);
                // }
            }
        }

        // this.imageLocation.setImage(locationCanvas);

        if (newLocation.looped) {
            camera.resetBounds();
        } else {
            camera.setBounds(0, 0, this.imageLocation.width, this.imageLocation.height);
        }

        this.currentLocation = newLocation;
    }

    this.getTileSymbol = function(x, y) {
        return this.currentLocation.map[y][x];
    }

    this.getTile = function(x, y) {
        var idx = this.tileSymbols.indexOf(this.getTileSymbol(x, y));
        return Tiles[this.tileNames[idx]];
    }

    this.getTileName = function(x, y) {
        var idx = this.tileSymbols.indexOf(this.getTileSymbol(x, y));
        return this.tileNames[idx];
    }

    this.isPassable = function(x, y) {
        var looped = this.currentLocation.looped;

        if (x < 0) x = 63;
        if (x > 63) x = 0;
        if (y < 0) y = 63;
        if (y > 63) y = 0;
        
        return this.getTile(x, y).passable;
    }

}
