function ComponentLocation(game, camera) {
    this.name = "Location";
    this.entity = null;

    this.tiles = [];
    this.tileNames = [];
    this.tileSymbols = [];

    this.imageLocation = null;
    this.locationSize = { x: 64, y: 64};
    this.locationPixelSize = { x: 0, y: 0};
    this.currentLocation = null;

    this.animatedTileY = 0;
    this.imageWater = null;
    this.imageForceField = null;

    this.create = function() {
        this.tileNames = Object.keys(Maps.symbols);
        this.tileSymbols = Object.values(Maps.symbols);

        var tilesAtlas = chao.getImage("tiles");
        var tileset = Tilesets.getCurrent();

        for (var i = 0; i < this.tileNames.length; ++i) {
            var tileName = this.tileNames[i];
            var newTile = chao.createImage(tileName, tileset.width, tileset.height);
            var tileSprite = chao.createImage(undefined, tileset.width, tileset.height);

            var tileRect = {
                x: Tiles[tileName].x * tileset.width,
                y: Tiles[tileName].y * tileset.height,
                width: tileset.width,
                height: tileset.height
            }

            chao.drawImagePart(tileSprite, tilesAtlas, 0, 0, tileRect);
            chao.tintImage(tileSprite, Tilesets.getColor(tileName));

            chao.clearToColor(newTile, chao.makeColor(0, 0, 0));
            chao.drawImage(newTile, tileSprite, 0, 0);

            this.tiles.push(newTile);
        }

        this.imageWater = chao.createImage("water_tile", tileset.width, tileset.height);
        this.imageForceField = chao.createImage("force_field_tile", tileset.width, tileset.height);
    }

    this.destroy = function() {
        //
    }

    this.draw = function() {
        var entity = this.entity;
        var screenPos = { x : entity.screenX, y : entity.screenY };

        // main location image
        if (this.imageLocation) {
            chao.drawImage(chao.canvas, this.imageLocation, screenPos.x, screenPos.y);
        }

        if (this.currentLocation.looped) {
            var size = this.locationPixelSize;
            
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
            visibleRect.x = Math.ceil(visibleRect.x / Tilesets.getCurrent().width);
            visibleRect.y = Math.ceil(visibleRect.y / Tilesets.getCurrent().height);
            visibleRect.width = Math.ceil(visibleRect.width / Tilesets.getCurrent().width);
            visibleRect.height = Math.ceil(visibleRect.height / Tilesets.getCurrent().height);
            for(x = visibleRect.x - 1; x < visibleRect.x + visibleRect.width; ++x) {
                var tx = x >= 0 ? x : x + this.locationSize.x;
                tx %= this.locationSize.x;
                var drawX = tx * Tilesets.getCurrent().width;
                for(y = visibleRect.y - 1; y < visibleRect.y + visibleRect.height; ++y) {
                    var ty = y >= 0 ? y : y + this.locationSize.y;
                    ty %= this.locationSize.y;
                    var drawY = ty * Tilesets.getCurrent().height;
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
        if (this.animatedTileY >= Tilesets.getCurrent().height) {
            this.animatedTileY -= Tilesets.getCurrent().height;
        }

        var waterImage = chao.getImage("water");
        chao.drawImage(this.imageWater, waterImage, 0, this.animatedTileY);
        chao.drawImage(this.imageWater, waterImage, 0, this.animatedTileY - Tilesets.getCurrent().height);

        var forceFieldImage = chao.getImage("forceField");
        chao.drawImage(this.imageForceField, forceFieldImage, 0, this.animatedTileY);
        chao.drawImage(this.imageForceField, forceFieldImage, 0, this.animatedTileY - Tilesets.getCurrent().height);

    }

    this.loadLocation = function(locationName) {
        var newLocation = Maps[locationName];

        this.locationSize.x = newLocation.map[0].length;
        this.locationSize.y = newLocation.map.length;
        this.locationPixelSize.x = this.locationSize.x * Tilesets.getCurrent().width;
        this.locationPixelSize.y = this.locationSize.y * Tilesets.getCurrent().height;

        this.imageLocation = chao.createImage(undefined, this.locationPixelSize.x, this.locationPixelSize.y);

        for (var x = 0; x < this.locationSize.x; ++x) {
            for (var y = 0; y < this.locationSize.y; ++y) {
                var symbol = newLocation.map[y][x];
                var symbolIdx = this.tileSymbols.indexOf(symbol);

                if (symbolIdx == -1) {
                    chao.log("Unrecognized map symbol: \"" + symbol + "\"");
                }

                var image = this.tiles[symbolIdx];
                chao.drawImage(this.imageLocation, image, x * Tilesets.getCurrent().width, y * Tilesets.getCurrent().height);
            }
        }

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

        if (x < 0) x = this.locationSize.x - 1;
        if (x > this.locationSize.x - 1) x = 0;
        if (y < 0) y = this.locationSize.y - 1;
        if (y > this.locationSize.y - 1) y = 0;
        
        return this.getTile(x, y).passable;
    }

}
