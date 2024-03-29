function StateGame() {

    this.camera = null;

    this.groupWorld = null;
    this.groupGUI = null;

    this.ludzik = null;
    this.location = null;

    this.hud = null;

    this.create = function() {
        this.groupWorld = new Entity("Group World");
        this.groupGUI = new Entity("Group GUI");

        this.add(this.groupWorld);
        this.add(this.groupGUI);

        this.groupGUI.makeFullscreen();

        this.camera = (new Entity("Camera")).addComponent(new ComponentCamera());
        this.groupWorld.add(this.camera.entity);

        this.location = (new Entity("Location")).addComponent(new ComponentLocation(this, this.camera));

        this.hud = this.groupGUI.addWithComponent(new Entity("HUD"), new ComponentHUD());

        this.camera.entity.add(this.location.entity);
        this.camera.offsetX = -(this.hud.getVisibileFrameWidth()) / 2;

        var loc = 3;
        if (loc == 0) {
            this.location.loadLocation("piratesHarbor");
            this.ludzik = (new Entity("Ludzik")).addComponent(new ComponentPlayer(this, this.camera, 32, 62));
        } else if (loc == 1) {
            this.location.loadLocation("bc1423");
            this.ludzik = (new Entity("Ludzik")).addComponent(new ComponentPlayer(this, this.camera, 16, 17));
        } else if (loc == 2) {
            this.location.loadLocation("mars");
            this.ludzik = (new Entity("Ludzik")).addComponent(new ComponentPlayer(this, this.camera, 30, 30));
        } else if (loc == 3) {
            this.location.loadLocation("planetX");
            this.ludzik = (new Entity("Ludzik")).addComponent(new ComponentPlayer(this, this.camera, 14, 20));
        }


        this.camera.entity.add(this.ludzik.entity);
        this.camera.follow(this.ludzik.entity, 4.0);
        this.camera.snapToFollowed();


        chao.helpers.createFpsCounter(this.groupGUI, 16);

        // Utils.addText(this.groupGUI, "IO CHAOS (i tak dalej)", 100, 100, 1);

    }

    this.resize = function() {
        //
    }

    this.update = function() {
        if (chao.justPressed[chao.KEY_L]) {
            chao.logHierarchy(this.rootEntity);
        }

        // debug camera controls
        if (false) {
            if (chao.pressed[chao.KEY_UP]) {
                this.camera.entity.y += chao.getTimeDelta() * 50;
            }
            if (chao.pressed[chao.KEY_DOWN]) {
                this.camera.entity.y -= chao.getTimeDelta() * 50;
            }
            if (chao.pressed[chao.KEY_LEFT]) {
                this.camera.entity.x += chao.getTimeDelta() * 50;
            }
            if (chao.pressed[chao.KEY_RIGHT]) {
                this.camera.entity.x -= chao.getTimeDelta() * 50;
            }
        }
    }

    this.draw = function() {
        //
    }

    this.loadLocation = function(locationName) {
        //
    }

}