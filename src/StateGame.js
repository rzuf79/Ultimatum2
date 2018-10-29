function StateGame(){

	this.camera 		= null;
	
	this.groupWorld 	= null;
	this.groupGUI 		= null;

	this.ludzik 		= null;

	this.location		= null;
	
	this.create = function(){
		
		this.groupWorld = new Entity("Group World");
		this.groupGUI 	= new Entity("Group GUI");

		this.add(this.groupWorld);
		this.add(this.groupGUI);

		this.camera = (new Entity("Camera")).addComponent(new ComponentCamera());
		this.groupWorld.add(this.camera.entity);

		this.location = (new Entity("Location")).addComponent(new ComponentLocation(this));
		this.camera.entity.add(this.location.entity);

		this.location.loadLocation("newSanAntonio");

		this.ludzik = (new Entity("Ludzik")).addComponent(new ComponentPlayer(this, 32, 62));
		this.camera.entity.add(this.ludzik.entity);

		this.camera.follow(this.ludzik.entity, chao.framerate/2);

	}

	this.resize = function(){
		//
	}

	this.update = function(){
		if(chao.justPressed[chao.KEY_L]){
			chao.logHierarchy(this.rootEntity);
		}

		// debug camera controls
		if(false){
			if(chao.keys[chao.KEY_UP]){
				this.camera.entity.y += chao.getTimeDelta() * 50;
			}
			if(chao.keys[chao.KEY_DOWN]){
				this.camera.entity.y -= chao.getTimeDelta() * 50;
			}
			if(chao.keys[chao.KEY_LEFT]){
				this.camera.entity.x += chao.getTimeDelta() * 50;
			}
			if(chao.keys[chao.KEY_RIGHT]){
				this.camera.entity.x -= chao.getTimeDelta() * 50;
			}
		}
	}

	this.draw = function(){
		//
	}

	this.loadLocation = function(locationName){
		//
	}

}