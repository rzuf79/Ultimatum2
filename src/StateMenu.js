function StateMenu() {

    this.create = function() {
        //
    }

    this.resize = function() {
        //
    }

    this.update = function() {
        if (chao.justPressed[chao.KEY_L]) {
            chao.logHierarchy(this.rootEntity);
        }
    }

    this.draw = function() {
        //
    }

}