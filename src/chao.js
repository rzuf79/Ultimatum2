/**
 * chao.js
 */

"use strict";

/**
 * Global chao namespace.
 */
var chao = {

	/** Consts. */
	VERSION					: "0.21",

	SCALING_MODE_NONE		: 0,	// Game canvas will not be scaled at all.
	SCALING_MODE_STRETCH	: 1,	// Scales the canvas to fill the whole viewport.
	SCALING_MODE_KEEP_RATIO	: 2,	// Scales the canvas to fit the viewport but keeps the aspect ratio.
	SCALING_MODE_EXTEND		: 3,	// Scale to fit and lenghten the shorter dimension to fill the viewport.
	SCALING_MODE_END		: 4,

	INTERPOLATE_LINEAR		: 0,	// Basing interpolation, without any smoothing.
	INTERPOLATE_SMOOTH		: 1,	// Smooooooth!
	INTERPOLATE_EASE_TO		: 2,	// Smoothes the end of the interpolation.
	INTERPOLATE_EASE_FROM	: 3,	// Smoothes the beginning of the interpolation.
	INTERPOLATE_BOUNCE		: 4,	// Bounces the interpolated value a bit before reaching the end.

	REPEAT_MODE_ONCE		: 0,	// Just plays the thing once.
	REPEAT_MODE_LOOP		: 1,	// When finished, start again from the beginning.
	REPEAT_MODE_BOUNCE		: 2,	// Goes back and forth.

	/** Key codes. */
	KEY_A: 0x41, KEY_B: 0x42, KEY_C: 0x43, KEY_D: 0x44, KEY_E: 0x45, KEY_F: 0x46, KEY_G: 0x47, KEY_H: 0x48, KEY_I: 0x49, KEY_J: 0x4A, KEY_K: 0x4B, KEY_L: 0x4C, KEY_M: 0x4D, KEY_N: 0x4E, KEY_O: 0x4F, KEY_P: 0x50, KEY_Q: 0x51, KEY_R: 0x52, KEY_S: 0x53, KEY_T: 0x54, KEY_U: 0x55, KEY_V: 0x56, KEY_W: 0x57, KEY_X: 0x58, KEY_Y: 0x59, KEY_Z: 0x5A, KEY_0: 0x30, KEY_1: 0x31, KEY_2: 0x32, KEY_3: 0x33, KEY_4: 0x34, KEY_5: 0x35, KEY_6: 0x36, KEY_7: 0x37, KEY_8: 0x38, KEY_9: 0x39, KEY_0_PAD: 0x60, KEY_1_PAD: 0x61, KEY_2_PAD: 0x62, KEY_3_PAD: 0x63, KEY_4_PAD: 0x64, KEY_5_PAD: 0x65, KEY_6_PAD: 0x66, KEY_7_PAD: 0x67, KEY_8_PAD: 0x68, KEY_9_PAD: 0x69, KEY_F1: 0x70, KEY_F2: 0x71, KEY_F3: 0x72, KEY_F4: 0x73, KEY_F5: 0x74, KEY_F6: 0x75, KEY_F7: 0x76, KEY_F8: 0x77, KEY_F9: 0x78, KEY_F10: 0x79, KEY_F11: 0x7a, KEY_F12: 0x7b, KEY_ESC: 0x1B, KEY_TILDE: 0xc0, KEY_MINUS: 0xbd, KEY_EQUALS: 0xbb, KEY_BACKSPACE: 0x08, KEY_TAB: 0x09, KEY_OPENBRACE: 0xdb, KEY_CLOSEBRACE: 0xdd, KEY_ENTER: 0x0D, KEY_COLON: 0xba, KEY_QUOTE: 0xde, KEY_BACKSLASH: 0xdc, KEY_COMMA: 0xbc, KEY_STOP: 0xbe, KEY_SLASH: 0xBF, KEY_SPACE: 0x20, KEY_INSERT: 0x2D, KEY_DEL: 0x2E, KEY_HOME: 0x24, KEY_END: 0x23, KEY_PGUP: 0x21, KEY_PGDN: 0x22, KEY_LEFT: 0x25, KEY_RIGHT: 0x27, KEY_UP: 0x26, KEY_DOWN: 0x28, KEY_SLASH_PAD: 0x6F, KEY_ASTERISK: 0x6A, KEY_MINUS_PAD: 0x6D, KEY_PLUS_PAD: 0x6B, KEY_ENTER_PAD: 0x0D, KEY_PRTSCR: 0x2C, KEY_PAUSE: 0x13, KEY_EQUALS_PAD: 0x0C, KEY_LSHIFT: 0x10, KEY_RSHIFT: 0x10, KEY_LCONTROL: 0x11, KEY_RCONTROL: 0x11, KEY_ALT: 0x12, KEY_ALTGR: 0x12, KEY_LWIN: 0x5b, KEY_RWIN: 0x5c, KEY_MENU: 0x5d, KEY_SCRLOCK: 0x9d, KEY_NUMLOCK: 0x90, KEY_CAPSLOCK: 0x14,
	
	/** Basic 16 colors.
	 *
	 * 0 = Black 			0xff000000
	 * 1 = Blue 			0xff0000aa
	 * 2 = Green 			0xff00aa00
	 * 3 = Cyan 			0xff00aaaa
	 * 4 = Red 				0xffaa0000
	 * 5 = Magenta 			0xff800080
	 * 6 = Brown 			0xff995500
	 * 7 = White 			0xffaaaaaa
	 * 8 = Gray				0xff555555
	 * 9 = LightBlue 		0xff5555ff
	 * 10 = LightGreen		0xff54ff3f
	 * 11 = LightCyan		0xff55ffff
	 * 12 = LightRed		0xffff5555
	 * 13 = LightMagenta	0xffff55ff
	 * 14 = Yellow 			0xffffff55
	 * 15 = BrightWhite		0xffffffff
	 */
	colorCodes: [0xff000000, 0xff0000aa, 0xff00ff00, 0xff00aaaa, 0xffaa0000, 0xff800080, 0xff995500, 0xffaaaaaa, 0xff555555, 0xff5555ff, 0xff54ff3f, 0xff55ffff, 0xffff5555, 0xffff55ff, 0xffffff55, 0xffffffff],

	/** Base64-encoded default font */
	defaultFontData: "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AuP91hUHyjWWdL04hQHN/AVhBoFnuhUZ/W0wAqYAAAAAAwAq/q0FBAWVAA4AJAAoAAAJATYXFTc1MzcjNwcXIxMBByUHFzYeAQYuATcHHgI+AiYHNwMXAScDuf8AQ9Rtf0jGAXEBqvH8hlABXdAOUKohVqWLGmwHeLx/ZwiFaNunaAGeYgLL/dYVCMk1lXW+OIYBzgIudAHzBylUt20JtUdgNaMiR4nSegb7+e5hBoFnAAAAAAEAG//8A6cFMwAiAAASJyYnITcFByY3NiQWFzcuASQGCgESHgE2NycGJyYnITcFB9gEDwYCHLH9t4UMNk0BJm0NrQaE/tzJyzZhrvnrqhHA5K9LAc+x/bdTAi0PQTqtAZybeK8GiDByQ6QHcv6//ov+8MZUXvML029VoK0BYQAAAAABAAABagABADoAwAAFAJwAJgBR/54AKQAzAEUAKQA3AIcAKQA5AD0AKQA6AEkAKQA8AEIAKQBD/2IAKQBH/z4AKQBR/20AKQBX/54ALQBH//MALQBR//IAMABH/+cAMABR/+oAMwA3AB4AMwA8/7sAMwBD/54AMwBH/5oAMwBR/8IANwAzADwANwA3AF4ANwA5ADMANwA6AGkANwA8ADkANwBD/t4ANwBH/y8ANwBKAAwANwBR/wAANwBX/ukANwBb/xIAOQApADMAOQAzAFoAOQA3AJwAOQA5AFIAOQA6AIcAOQA8AFcAOQBD/0UAOQBH/0UAOQBL/9QAOQBR/20AOQBX/4MAOQBb/7MAOgA3ADsAOgBD/0UAOgBH/28AOgBL/+cAOgBR/1sAOgBX/7AAPAA6ADMAPABD/woAPABH/0wAPABL/8QAPABR/0EAPABX/zsASABH/8QASABI/44ASABLAAAASABR/9AAAAAAAAwAlgADAAEECQAAADYAAAADAAEECQABACIARAADAAEECQACAA4ANgADAAEECQADACwARAADAAEECQAEACIARAADAAEECQAFAAYAcAADAAEECQAGACAAdgADAAEECQAHACoAlgADAAEECQAIADgBIAADAAEECQAJAB4AwAADAAEECQAKAEIA3gADAAEECQAMADgBIABLAGUAdgBpAG4AIABLAGkAbgBnADoAIABLAGkAbgBnAHQAaABpAG4AZwBzACAAMgAwADAANwBSAGUAZwB1AGwAYQByAEsAaQBuAGcAdABoAGkAbgBnAHMAIABFAHgAZQB0AGUAcgAgADIAMAAwADcAMQAuADAASwBpAG4AZwB0AGgAaQBuAGcAcwBFAHgAZQB0AGUAcgBLAGkAbgBnAHQAaABpAG4AZwBzACAAQQBwAHIAaQBsACAAMgAwADAANwBLAGUAdgBpAG4AIABLAGkAbgBnACAAMgAwADAANwBDAGEAbABsAGkAZwByAGEAcABoAGkAYwAgAFIAbwB1AG4AZABoAGEAbgBkACAAcwBhAG4AcwAgAHMAZQByAGkAZgBrAGUAdgBpAG4AawBpAG4AZwBAAG0AeQBwAG8AcwB0AG8AZgBmAGkAYwBlAC4AYwBvAC4AdQBrAAAAAgAAAAAAAP9cADAAAAAAAAAAAAAAAAAAAAAAAAAAAABtAAABAgEDAAMABAAFAAYABwAIAAkACgALAAwADQAOAA8AEAARABIAEwAUABUAFgAXABgAGQAaABsAHAAdAB4AHwAgACEAIgAjACQAJQAmACcAKAApACoAKwAsAC0ALgAvADAAMQAyADMANAA1ADYANwA4ADkAOgA7ADwAPQA+AD8AQABCAEMARABFAEYARwBIAEkASgBLAEwATQBOAE8AUABRAFIAUwBUAFUAVgBXAFgAWQBaAFsAXABdAF4AYACFAIcAqwC0ALUAtgC3ALgA7wDwAPQA9QD2AQQFLm51bGwQbm9ubWFya2luZ3JldHVybgRFdXJvAAAAAf//AAI=",

	/** String for testing if a font was loaded. */
	fontTestString: "giItT1WQy@!-/#", // this only looks like it's random, but it's not!

	/**
	 * Inits the whole engine.
	 *
	 * @param width - Target width of the viewport in pixels.
	 * @param height - Target height of the viewport in pixels.
	 * @param scalingMode - Defines how the canvas will scale in the window it's in. Use chao.SCALING_MODE_SOMETHING. Defaults to SCALING_MODE_NONE.
	 * @param canvasId - Optional - id of the canvas element to put the game into.
	 */
	init: function(width, height, scalingMode, canvasId) {

		// Some cross-browser compatibility stuff below
		if (!Date.now) {
			Date.now = function now() { return new Date().getTime(); };
		}

		// Engine inits:
		var canvas;
		if(canvasId){
			canvas = document.getElementById(canvasId);
		}
		if(!canvas){
			canvas = document.createElement("canvas");
			if(canvasId){
				canvas.setAttribute("id", canvasId);
			}
			canvas.setAttribute("width", width);
			canvas.setAttribute("height", height);
			canvas.style.backgroundColor = "black";
			document.body.appendChild(canvas);
		}

		var context		= canvas.getContext("2d");

		// Canvas object is organized like a regular chao image, for cohesion.
		chao.canvas = {
			canvas: 	canvas,
			context: 	context,
			width: 		width,
			height: 	height,
		}

		chao.loggingEnabled 	= true;										// Enable debug logging to browser's console.

		chao.screenWidth 		= width;									// Viewport width in pixels. Can change dynamically in some scaling modes.
		chao.screenHeight 		= height;									// Viewport height in pixels. Can change dynamically in some scaling modes.
		chao.scalingMode		= scalingMode || chao.SCALING_MODE_NONE;	// Currently set scaling mode.

		chao.screenScaleX		= 1.0;										// Current x scale of the viewport.
		chao.screenScaleY		= 1.0;										// Current y scale of the viewport.
		chao.baseScreenWidth	= width;									// Original width of the viewport, the one passed in the chao.init()
		chao.baseScreenHeight	= height;									// Original height of the viewport, the one passed in the chao.init()

		chao.installVisibilityHandler();
		chao.hasFocus 			= true;										// Does the game window have focus at the moment.

		chao.images				= [];										// Array containing all the loaded images.
		chao.smoothing			= true;										// Is images smoothing enabled.
		chao.setImagesSmoothing(chao.smoothing);

		chao.pauseOnFadeEnabled	= true;										// Should the game pause when the window focus is lost.
		chao.imagePauseFade 	= null;										// Image displayed when the game is paused because of the lost focus.
		chao.updatePauseFadeImage();

		chao.onAssetsLoaded		= undefined;								// Internal - called when all the assets are finished loading.

		chao.updateInterval 	= null;										// Holds the engine's main loop.
		chao.framerate 			= 60;										// How many times the game is supposed to be updated each second.
		chao.setFPS(60);
		chao.lastTime 			= Date.now();								// Internal - remembers the epoch time from the last update cycle.
		chao.timeDelta 			= 0.0;										// milliseconds that passed since the last frame.
		chao.timeScale 			= 1.0;										// Scale time returned by getTimeDelta function. Useful for slowing down or speeding things up.

		chao.countFPS 			= false;									// Set to true and the engine will count how many frames it managed to process each second.
		chao.currentFPS 		= 0;										// Number of frames processed during the last second.
		chao.FPSCounter 		= 0;										// Internal - for FPS counting.
		chao.FPSTimer 			= 0;										// Internal - for FPS counting.

		chao.sounds 			= [];										// Array containing all the loaded sounds.
		chao.currentMusic 		= null;										// Sound that is currently playing as a music.
		chao.musicWasSupressed	= false;									// If music was supressed for some reason. Will try to play it again on the first user input.
		chao.muted 				= false;									// Are the sounds muted right now. Use chao.setMute to change this.
		chao.muteOnFocusLost	= true;										// Is the game supposed to be silent when focus i lost.
		chao.wasMutedOnFocusLost= false;									// Internal - helps to figuer out what to do with the sound when we get the focus back.

		chao.keys 				= [];										// Boolean values of all the keyboard keys presses. Eg. if(chao.keys[chao.KEY_SPACE]) checks if the space key is currently pressed.
		chao.justPressed 		= [];										// Boolean values of all the keyboard keys presses. Eg. if(chao.keys[chao.KEY_SPACE]) checks if the space key was pressed in current frame.
		chao.justReleased 		= [];										// Boolean values of all the keyboard keys releases. Eg. if(chao.keys[chao.KEY_SPACE]) checks if the space key was released in current frame.

		chao.touches			= [];										// Array containing all the active touches. Each touch is an object with following values: "id" (unique id of the touch), "x", "y", "justPressed" (was this touch activated in the current frame). The first active touch is interpreted just as the left mouse button.

		chao.mouse 						= {};								// All the mousey data.
		chao.mouse.x 					= -1;								// X position of the mouse, from the left edge of the viewport.
		chao.mouse.y 					= -1;								// Y position of the mouse, from the top edge of the viewport.
		chao.mouse.wheelDelta 			= 0;								// How mouch the mouse wheel was scrolled since the last frame.
		chao.mouse.pressed 				= false;							// Is the left mouse button pressed right now.
		chao.mouse.justPressed			= false;							// Was the left mouse button just pressed in the current frame.
		chao.mouse.justReleased			= false;							// Was the left mouse button just released in the current frame.
		chao.mouse.pressedRight 		= false;							// Is the right mouse button pressed right now.
		chao.mouse.justPressedRight		= false;							// Was the right mouse button just pressed in the current frame.
		chao.mouse.justReleasedRight	= false;							// Was the right mouse button just released in the current frame.
		chao.mouse.pressedMiddle 		= false;							// Is the middle mouse button pressed right now.
		chao.mouse.justPressedMiddle	= false;							// Was the middle mouse button just pressed in the current frame.
		chao.mouse.justReleasedMiddle	= false;							// Was the middle mouse button just released in the current frame.
		chao.mouse.suppressUntilUp		= false;							// Internal - mouse input will be ignored until its button is released. To avoid accidental clicks when returning from focus pause.

		chao.resetInput();

		chao.fonts 						= [];								// All loaded font objects.
		chao.loadedFontsNum 			= 0;								// Total number of fonts loaded.
		chao.enableFontsLoadCheck		= true;								// Enable wonky fonts loading state checks.
		chao.font 						= chao.loadBase64Font(undefined, chao.defaultFontData); // Default chao font.

		chao.tweens						= [];								// Array containing all the active tweens.

		chao.entitiesToDestroy			= [];								// Internal - list of entities to be safely destroyed and removed from the hierarchy.
		chao.focusedEntity				= null;								// Internal - entity currently processed by the mouse/touch inputs.

		chao.currentState				= undefined;						// Current state.
		chao.loadingState				= undefined;						// State to be used when the assets are being loaded.
		chao.newState					= undefined;						// Internal - state we are bount to switch to.

		chao.setLoadingState({
			draw: function(){
				var barWidth 		= chao.screenWidth * 0.6;
				var barHeight 		= barWidth * 0.1;
				var barX 			= chao.screenWidth / 2 - barWidth / 2;
				var barY 			= chao.screenHeight - (barHeight * 1.25);
				var barColor 		= chao.makeColor(255, 255, 255);

				chao.clearToColor(chao.canvas, chao.makeColor(30, 30, 30));
				chao.drawRect(chao.canvas, barX - 4, barY - 4, barWidth + 8, barHeight + 8, barColor);
				chao.drawRectFill(chao.canvas, barX, barY, barWidth * chao.getLoadingProgress(), barHeight, barColor);
			},
		});

		chao.switchState({});

		// Install all the listeners.
		canvas.addEventListener("mousedown", chao.onMouseDown);
		window.addEventListener("mouseup", chao.onMouseUp);
		canvas.addEventListener("mousemove", chao.onMouseMove);
		window.addEventListener('contextmenu',function(e){e.preventDefault();});
		window.addEventListener("wheel", chao.onMouseWheel);
		canvas.addEventListener("touchstart", chao.onTouchStart);
		canvas.addEventListener("touchmove", chao.onTouchMove);
		window.addEventListener("touchend", chao.onTouchEnd);
		window.addEventListener("keyup", chao.onKeyUp);
		window.addEventListener("keydown", chao.onKeyDown);
		window.addEventListener("resize", chao.resize);

	},

	/**
	 * Sets the update interval of the game.
	 *
	 * @param FPS - How many times per second shall the game be updated.
	 */
	setFPS: function(FPS){
		chao.framerate = FPS;
		chao.updateInterval = setInterval(chao.update, 1000/FPS);
	},

	/**
	 * Clears the whole canvas or fills it with chao.backgroundColor if it's set.
	 */
	clearScreen: function(){
		if (chao.backgroundColor=="none") { 
			chao.canvas.context.clearRect(0, 0, chao.canvas.canvas.width, chao.canvas.canvas.height); 
		}else{ 
			chao.canvas.context.fillStyle = chao.backgroundColor; 
			chao.canvas.context.fillRect(0, 0, chao.canvas.canvas.width, chao.canvas.canvas.height); 
		}
	},

	/**
	 * Called when the canvas loses/resumes focus.
	 *
	 * @param isFocused - Is the game window active.
	 */
	onFocusChange: function(isFocused){
		if(chao.hasFocus == isFocused){
			return;
		}
		if(isFocused){
			chao.hasFocus 	= true;
			chao.lastTime 	= Date.now();

			chao.mouse.suppressUntilUp = true;
			chao.resetInput();
			
			if(chao.muteOnFocusLost){
				chao.setMute(chao.wasMutedOnFocusLost);
			}
		} else {
			chao.hasFocus 	= false;
			chao.wasMutedOnFocusLost = chao.muted;
			if(chao.muteOnFocusLost){
				chao.setMute(true);
			}
		}
	},

	/**
	 * Resets all the input data.
	 */
	resetInput: function(){
		for (var i = 0; i < 0x80; ++i) {
			chao.keys[i] 			= false;
			chao.justPressed[i] 	= false;
			chao.justReleased[i] 	= false;
		}

		chao.mouse.pressed 				= false;
		chao.mouse.pressedRight 		= false;
		chao.mouse.pressedMiddle 		= false;

		chao.focusedEntity 				= null;
	},
	 
	/**
	 * Main game loop.
	 */
	update: function(){
		if(chao.enableFontsLoadCheck){
			chao.updateFontsLoading();
		}

		chao.clearScreen();

		chao.timeDelta 	= (Date.now() - chao.lastTime) * .001;
		chao.lastTime 	= Date.now();

		if(chao.countFPS){
			chao.FPSCounter ++;
			chao.FPSTimer += chao.timeDelta;
			if(chao.FPSTimer >= 1.0){
				chao.FPSTimer -= 1.0;
				chao.currentFPS = chao.FPSCounter;
				chao.FPSCounter = 0;
			}
		}


		var stateToProcess 	= chao.currentState;
		var focusPause		= !chao.hasFocus && chao.pauseOnFadeEnabled;

		if(chao.getLoadingProgress() < 1.0){
			stateToProcess = chao.loadingState;
		} else {
			if(chao.onAssetsLoaded){
				chao.onAssetsLoaded();
				chao.onAssetsLoaded = undefined;
			}
		}

		if(!focusPause){
			stateToProcess.rootEntity.update();
			if(stateToProcess.update){
				stateToProcess.update();
			}

			chao.updateTweens();
			chao.updateKeys();
			chao.updateMouse();
			chao.updateTouches();
		}

		stateToProcess.rootEntity.draw(0, 0, 1.0);
		if(stateToProcess.draw){
			stateToProcess.draw();
		}

		if(focusPause && chao.getLoadingProgress() >= 1){
			chao.drawImage(chao.canvas, chao.imagePauseFade, 0, 0);
		}

		for(var i = 0; i < chao.entitiesToDestroy.length; ++i){
			chao.entitiesToDestroy[i].destroy();
			if(chao.entitiesToDestroy[i].parent){
				chao.entitiesToDestroy[i].parent.remove(chao.entitiesToDestroy[i]);
				chao.entitiesToDestroy[i].parent = null;
			}
		}
		chao.entitiesToDestroy = [];


		if(chao.newState !== undefined){
			chao.destroyCurrentStateAndInitNewOne(chao.newState);
			chao.newState = undefined;
		}
	},

	/**
	 * Destroys current state, then init the given one and set it as the current.
	 *
	 * @param newState - State to be initialized and set as the current one.
	 */
	switchState: function(newState){
		if(chao.currentState === undefined){
			chao.destroyCurrentStateAndInitNewOne(newState);
		} else {
			chao.newState = newState;
		}
	},

	/**
	 * Immediately destroy the old state and init new one.
	 *
	 * @param newState - State to be initialized and set as the current one.
	 */
	destroyCurrentStateAndInitNewOne: function(newState){
		chao.resetInput();
		chao.clearTweens();

		chao.destroyState(chao.currentState);
		
		chao.currentState = newState;
		if(chao.getLoadingProgress() < 1.0){
			chao.onAssetsLoaded = this.initCurrentState;
		} else {
			chao.initState(chao.currentState);
		}
	},

	/**
	 * Sets the state to be used when the assets are being loaded.
	 *
	 * @param newLoadingState - State to be initialized and set as a loading state.
	 */
	setLoadingState: function(newLoadingState){
		chao.destroyState(chao.loadingState);

		chao.loadingState = newLoadingState;
		chao.initState(newLoadingState);
		
	},

	/**
	 * Inits given state. Called by switchState().
	 *
	 * @param state - State to be initialized.
	 */
	initState: function(state){
		state.rootEntity = new Entity("Root", 0, 0);
		state.rootEntity.width = chao.screenWidth;
		state.rootEntity.height = chao.screenHeight;
		state.add = function(entity){
			this.rootEntity.add(entity);
		};
		state.remove = function(entity){
			this.rootEntity.remove(entity);
		};

		if(state.create){
			state.create();
		}
		chao.resize();
	},

	/**
	 * Internal wrapper for initState(), to be called when assets loading finishes.
	 */
	initCurrentState: function(){
		chao.initState(chao.currentState);
	},

	/**
	 * Returns the currently processed state.
	 *
	 * @return - CurrentState if assets are loaded and ready, loadingState otherwise.
	 */
	getCurrentState: function(){
		return chao.getLoadingProgress() >= 1.0 ? chao.currentState : chao.loadingState;
	},

	/**
	 * Destroys the current state. Called internally when needed.
	 *
	 * @param state - State to be destroyed.
	 */
	destroyState: function(state){
		if(!state){
			return;
		}

		if(state.destroy){
			state.destroy();
		}
		state.rootEntity.destroy();
		state.rootEntity = null;
	},

	/**
	 * Safely destroys the given entity and all its children. Also removes all the attached tweens.
	 *
	 * @param entity - Entity to be destroyed.
	 */
	destroyEntity: function(entity){
		chao.removeTweensFromEntity(entity);
		chao.entitiesToDestroy.push(entity);
	},

	/**
	 * Returns all entities of given name in the entity and its children.
	 *
	 * @param name - Name of the entity to be found.
	 * @param entity - Entity from which we will begin the search. Defaults to the rootEntity from currentState.
	 * @param array - Used for recursion, just ignore it.
	 */
	findEntities: function(name, entity, array){
		entity = entity || chao.currentState.rootEntity;
		array = array || [];

		if(entity.name === name){
			array.push(entity);
		}

		for(var i = 0; i < entity.children.length; ++i){
			chao.findEntities(name, entity.children[i], array);
		}

		return array;
	},

	/**
	 * Returns all components of given name in the entity and its children.
	 *
	 * @param name - Name by which the components will be identified.
	 * @param entity - Entity from which we will begin the search. Defaults to the rootEntity from currentState.
	 * @param array - Used for recursion, just ignore it.
	 */
	findComponents: function(name, entity, array){
		entity = entity || chao.currentState.rootEntity;
		array = array || [];

		var foundComponent = entity.getComponentByName(name);
		if(foundComponent){
			array.push(foundComponent);
		}

		for(var i = 0; i < entity.children.length; ++i){
			chao.findComponents(name, entity.children[i], array);
		}

		return array;
	},

	/**
	 * Creates a blank image.
	 *
	 * @param key - String by which this image will be identified. Pass undefined if you don't want it to be booked by the engine by any name.
	 * @param width - Width of the image in pixels.
	 * @param height - Height of the image in pixels.
	 * @return - Created image.
	 */
	createImage: function(key, width, height){
		var newCanvas 		= document.createElement("canvas");
		var newContext 		= newCanvas.getContext("2d");
		newCanvas.width 	= width;
		newCanvas.height 	= height;

		var newImage 	= {
			key: 			key,
			canvas: 		newCanvas,
			context: 		newContext,
			width: 			width,
			height: 		height,
			rotationOrigin:	{x:0.5, y:0.5}, // {0.0 - 1.0}
			ready: 			true,
		};

		chao.setSmoothingForImage(newImage, chao.smoothing);

		if(key){
			chao.addImage(newImage);
		}

		return newImage;
	},

	/**
	 * Loads image from a file.
	 *
	 * @param key - String by which this image will be identified.
	 * @param path - Path to the file.
	 * @return - Loaded image.
	 */
	loadImage: function(key, path){
		var img = new Image();
		img.src = path;
		var newCanvas 	= document.createElement("canvas");
		var newContext 	= newCanvas.getContext("2d");
		var newImage 	= {
			key: 			key,
			canvas: 		newCanvas,
			context: 		newContext,
			width: 			-1,
			height: 		-1,
			rotationOrigin:	{x:0.5, y:0.5}, // {0.0 - 1.0}
			ready: 			false,
		};

		
		if(key){
			chao.addImage(newImage);
		}

		img.onload = function(){
			newImage.canvas.width 	= img.width;
			newImage.canvas.height 	= img.height;
			chao.setSmoothingForImage(newImage, chao.smoothing);
			newImage.context.drawImage(img, 0, 0);
			newImage.width 	= img.width;
			newImage.height = img.height;
			newImage.ready 	= true;
		};

		return newImage;
	},

	/**
	 * Adds image to the internal images array. Also makes sure that only one image with the given key exist at a time.
	 *
	 * @param image - Image to be added.
	 */
	addImage: function(image){
		var oldImage = -1;

		for(var i = 0; i < chao.images.length; ++i){
			if(chao.images[i].key == image.key){
				oldImage = i;
				break;
			}
		}

		if(oldImage != -1){
			chao.images.splice(oldImage, 1);
		}

		chao.images.push(image);
	},

	/**
	 * Gets the image object by the given key.
	 *
	 * @param key - String identifying the image we wish to get.
	 * @return - Image object if found. If not found, or if the key is not a string, the given key param will be returned. It's designed like this to make some internal stuff faster.
	 */
	getImage: function(key){
		if(typeof key === "string" || key instanceof String){
			var n = chao.images.length;
			for(var i = 0; i < n; ++i){
				if(chao.images[i].key == key){
					return chao.images[i];
				}
			}
		}
		return key;
	},

	/**
	 * Permanently tints the given image with a color.
	 *
	 * @param image - image object or image key.
	 * @param color - String identifying the image we wish to get.
	 */
	tintImage: function(image, color){
		image = chao.getImage(image);

		var tintImage = chao.createImage(undefined, image.width, image.height);
		chao.clearToColor(tintImage, color);

		image.context.globalCompositeOperation = "source-atop";
		image.context.drawImage(tintImage.canvas, 0, 0, image.width, image.height);
		tintImage.context.globalCompositeOperation = "destination-atop";
	},

	/**
	 * Enables or disables images smoothing. Won't affect already loaded images that are not stored in the chao.images array.
	 *
	 * @param value - True for smooth images, false for crispy, edgy pixels.
	 */
	setImagesSmoothing: function(value){
		chao.smoothing = value;

		chao.setSmoothingForImage(chao.canvas, value);

		var n = chao.images.length;
		for(var i = 0; i < n; ++i){
			chao.setSmoothingForImage(chao.images[i], value);
		}

	},

	/**
	 * Enables or disables smoothing for a single image.
	 *
	 * @param value - True for smooth images, false for crispy, edgy pixels.
	 */
	setSmoothingForImage: function(image, value){
		image.context.mozImageSmoothingEnabled 		= value;
		image.context.webkitImageSmoothingEnabled 	= value;
		image.context.msImageSmoothingEnabled		= value;
		image.context.oImageSmoothingEnabled		= value;
		image.context.imageSmoothingEnabled 		= value;

		if(value){
			image.canvas.style['image-rendering'] = 'auto';
			image.canvas.style.msInterpolationMode = 'bicubic';
		} else {
			var renderTypes = [ 'optimizeSpeed', 'crisp-edges', '-moz-crisp-edges', '-webkit-optimize-contrast', 'optimize-contrast', 'pixelated' ];
			for(var i = 0; i < renderTypes.length; ++i){
				image.canvas.style['image-rendering'] = renderTypes[i];
			}
	        image.canvas.style.msInterpolationMode = 'nearest-neighbor';
		}
	},

	/**
	 * Blits an image to the target image data.
	 *
	 * @param target - Target bitmap to blit the image into.
	 * @param image - Image to be blitted.
	 * @param x - X position to draw the image.
	 * @param y - Y position to draw the image.
	 * @param alpha - Opacity of the image.
	 * @param scaleX - X scale of the image.
	 * @param scaleY - Y scale of the image.
	 * @param angle - Rotation of the image (in degrees).
	 */
	drawImage: function(target, image, x, y, alpha, scaleX, scaleY, angle){

		alpha 	= alpha === undefined ? 1 : alpha;
		scaleX 	= scaleX === undefined ? 1 : scaleX;
		scaleY 	= scaleY === undefined ? 1 : scaleY;
		angle	= angle || 0;

		image = chao.getImage(image);

		target.context.save();
		
		target.context.globalAlpha = alpha;

		var rotationPivot	= {
			x:(x+((image.width*scaleX)/2)),
			y:(y+((image.height*scaleY)/2))
		};
		target.context.translate(rotationPivot.x, rotationPivot.y);
		target.context.rotate(chao.rad2deg(angle));
		target.context.translate(-rotationPivot.x, -rotationPivot.y);

		target.context.scale(scaleX, scaleY);
		
		target.context.drawImage(image.canvas, x, y);
		
		target.context.restore();
	},

	/**
	 * Blits a part of the image to the target image data.
	 *
	 * @param target - Target bitmap to blit the image into.
	 * @param image - Image to be blitted.
	 * @param x - X position to draw the image.
	 * @param y - Y position to draw the image.
	 * @param rect - Rect object describing what part of the image we wish to draw: {x, y, width, height}.
	 * @param angle - Rotation of the image (in degrees).
	 * @param scaleX - X scale of the image.
	 * @param scaleY - Y scale of the image.
	 * @param alpha - Opacity of the image.
	 */
	drawImagePart: function(target, image, x, y, rect, angle, scaleX, scaleY, alpha){
		angle 	= angle || 0;
		alpha 	= alpha === undefined ? 1 : alpha;
		scaleX 	= scaleX === undefined ? 1 : scaleX;
		scaleY 	= scaleY === undefined ? 1 : scaleY;

		image = chao.getImage(image);

		var w 				= rect.width;
		var h 				= rect.height;
		var rotationPivot	= {
			x:(x+(w*image.rotationOrigin.x)),
			y:(y+(h*image.rotationOrigin.y))
		};

		target.context.save();
		target.context.globalAlpha = alpha;

		target.context.translate(rotationPivot.x, rotationPivot.y);
		target.context.rotate(chao.rad2deg(angle));
		target.context.translate(-rotationPivot.x, -rotationPivot.y);

		x = scaleX >= 0 ? x : x - w * scaleX;
		y = scaleY >= 0 ? y : y - h * scaleY;
		target.context.translate(x, y);

		target.context.scale(scaleX, scaleY);

		target.context.drawImage(image.canvas,
			rect.x, rect.y, rect.width, rect.height,
			0, 0, w, h);
		
		target.context.restore();
	},

	/**
	 * Set fill color for the given image.
	 *
	 * @param image - Image to set the fill color for.
	 * @param color - Color in a form of hash of 0xFFFFFFFF.
	 */
	setFillStyle: function(image, color){
		if(typeof color === "string"){
			image.context.fillStyle = color;	
		} else {
			image.context.fillStyle = chao.getRGBAString(color);
		}
	},

	/**
	 * Set stroke color and width for the given image.
	 *
	 * @param image - Image to set the stroke style for.
	 * @param color - Color in a form of hash or 0xFFFFFFFF hex.
	 * @param width - Width of the stroke in pixels.
	 */
	setStrokeStyle: function(image, color, width){
		width = width || image.context.lineWidth;
		image.context.lineWidth 	= width;
		if(typeof color === "string"){
			image.context.strokeStyle = color;
		} else {
			image.context.strokeStyle = chao.getRGBAString(color);
		}
	},

	/**
	 * Creates a color in a 0xFFFFFFFF form.
	 *
	 * @param r - Red value of the color (0-255)
	 * @param g - Green value of the color (0-255)
	 * @param b - Bluee value of the color (0-255)
	 * @param a - Alpha value of the color (0-255)
	 * @return - Int color.
	 */
	makeColor: function(r, g, b, a){
		a = a || 255;
		return (a<<24)|((r&0xff)<<16)|((g&0xff)<<8)|((b&0xff));
	},

	/**
	 * Creates a color in a 0xFFFFFFFF form.
	 *
	 * @param r - Red value of the color (0-1)
	 * @param g - Green value of the color (0-1)
	 * @param b - Bluee value of the color (0-1)
	 * @param a - Alpha value of the color (0-1)
	 * @return - Int color.
	 */
	makeColorf: function(r, g, b, a){
		a = a || 1;
		return chao.makeColor(a*255, r*255, g*255, b*255);
	},

	/**
	 * Creates a color string in "rgba(r,g,b,a)" format.
	 *
	 * @param hexColor - Int color value to create a string from.
	 * @return - Color string in "rgba(r,g,b,a)" format.
	 */
	getRGBAString: function(hexColor){
		var r = (hexColor>>16)&0xff;
		var g = (hexColor>>8)&0xff;
		var b = hexColor&0xff;
		var a = (hexColor>>>24) / 255;
		return "rgba(" + r + "," + g + "," + b + "," + a + ")";
	},

	/**
	 * Gets a pixel from an image.
	 *
	 * @param image - Image to get the pixel from.
	 * @param x - X position of the pixel
	 * @param y - Y position of the pixel
	 * @return - Color of the pixel.
	 */
	getPixel: function(image, x, y){
		var data = image.context.getImageData(x, y, 1, 1).data;
		return (data[3]<<24)|((data[0]&0xff)<<16)|((data[1]&0xff)<<8)|((data[2]&0xff));
	},

	/**
	 * Puts a pixel on an image. Super slow!
	 *
	 * @param image - Image to put the pixel on.
	 * @param x - X position of the pixel
	 * @param y - Y position of the pixel
	 * @param color - Color of the pixel.
	 */
	putPixel: function(image, x, y, color){
		chao.setFillStyle(image, color);
		image.context.fillRect(x, y, 1, 1);
	},

	/**
	 * Clears the given image.
	 *
	 * @param image - Image to clear.
	 */
	clearImage: function(image){
		image.context.clearRect(0, 0, image.width, image.height);
	},

	/**
	 * Clears an image using the given color.
	 *
	 * @param image - Image to clear.
	 * @param color - Color to fill the image with.
	 */
	clearToColor: function(image, color){
		image.context.clearRect(0, 0, image.width, image.height);
		chao.setFillStyle(image, color);
		image.context.fillRect(0, 0, image.width, image.height);
	},

	/**
	 * Draws a straight line between given point.
	 *
	 * @param image - Image to draw the line upon.
	 * @param x1 - X from.
	 * @param y1 - Y from.
	 * @param x2 - X to.
	 * @param y2 - Y to.
	 * @param color - Color of the line.
	 * @param width - Width of the line.
	 */
	drawLine: function(image, x1, y1, x2, y2, color, width){
		chao.setStrokeStyle(image, color, width);
		image.context.beginPath();
		image.context.moveTo(x1, y1);
		image.context.lineTo(x2, y2);
		image.context.closePath();
		image.context.stroke();
	},

	/**
	 * Draws a rectangle.
	 *
	 * @param image - Image to draw the rectangle upon.
	 * @param x - X position of the rectangle.
	 * @param y - Y position of the rectangle.
	 * @param w - Width of the rectangle.
	 * @param h - Height of the rectangle.
	 * @param color - Color to draw with.
	 * @param width - Stroke width.
	 */
	drawRect: function(image, x, y, w, h, color, width){
		chao.setStrokeStyle(image, color, width);
		image.context.strokeRect(x, y, w, h);
	},

	/**
	 * Draws a filled rectangle.
	 *
	 * @param image - Image to draw the rectangle upon.
	 * @param x - X position of the rectangle.
	 * @param y - Y position of the rectangle.
	 * @param w - Width of the rectangle.
	 * @param h - Height of the rectangle.
	 * @param color - Color to draw with.
	 */
	drawRectFill: function(image, x, y, w, h, color){
		chao.setFillStyle(image, color);
		image.context.fillRect(x, y, w, h);
	},

	/**
	 * Draws lines used by polygons drawing functions.
	 *
	 * @param image - Image to draw upon.
	 * @param points - Array of points the polygon consists of.
	 */
	drawPolygonLines: function(image, points){
		image.context.beginPath();
		for (var i = 0; i < points.length; i++){
			if(i) {
				image.context.lineTo(points[i].x, points[i].y);
			} else {
				image.context.moveTo(points[i].x, points[i].y);
			}
		}
		image.context.closePath();
	},

	/**
	 * Draws a polygon.
	 *
	 * @param image - Image to draw upon.
	 * @param points - Array of points the polygon consists of.
	 * @param color - Color to stroke with.
	 */
	drawPolygon: function(image, points, color, width){
		chao.setStrokeStyle(image, color, width);
		chao.drawPolygonLines(image, points);
		image.context.stroke();
	},

	/**
	 * Draws a polygon.
	 *
	 * @param image - Image to draw upon.
	 * @param points - Array of points the polygon consists of.
	 * @param color - Color to fill the polygon with.
	 */
	drawPolygonFill: function(image, points, color){
		chao.setFillStyle(image, color);
		chao.drawPolygonLines(image, points);
		image.context.fill();
	},

	/**
	 * Creates pause fade image. Called by the engine every time the viewport resolution changes to fill the whole canvas.
	 */
	updatePauseFadeImage: function(){
		var playWidth 	= chao.screenWidth * 0.3;
		var playHeight  = chao.screenWidth * 0.3;
		var center 		= { x: chao.screenWidth/2, y: chao.screenHeight/2 };

		var poly = chao.makePolygon([
			center.x - playWidth/2, center.y - playHeight/2,
			center.x + playWidth/2, center.y,
			center.x - playWidth/2, center.y + playHeight/2
		]);

		chao.imagePauseFade = chao.createImage(undefined, chao.screenWidth, chao.screenHeight);
		chao.clearToColor(chao.imagePauseFade, chao.makeColor(0, 0, 0, 160));
		chao.drawPolygonFill(chao.imagePauseFade, poly.points, chao.makeColor(255, 255, 255, 170));
	},

	/**
	 * Loads font from the given path.
	 *
	 * @param key - String to identify this font with.
	 * @param path - Path to the font file.
	 * @return - Loaded font object.
	 */
	loadFont: function(key, path){
		var s = document.createElement('style');
		var fontname = "font" + (chao.loadedFontsNum++);
		s.id = fontname;
		s.type = "text/css";
		document.head.appendChild(s);
		s.textContent = "@font-face { font-family: " + fontname + "; src:url('" + path + "');}";

		var newFont = {key: key, element: s, name: fontname, type: "fnt", ready: false};
		chao.addFont(newFont);
		return newFont;
	},

	/**
	 * Loads font from base64 data.
	 *
	 * @param key - String to identify this font with.
	 * @param data - Base64 encoded font data.
	 * @return - Loaded font object.
	 */
	loadBase64Font: function(key, data){
		var s = document.createElement('style');
		var fontname = "font" + (chao.loadedFontsNum++);
		s.id = fontname;
		s.type = "text/css";
		document.head.appendChild(s);
		s.textContent = "@font-face { font-family: " + fontname + "; src:url('data:application/font-woff;base64," + data + "') format('woff');}";

		var newFont = {key: key, element: s, name: fontname, type: "fnt", ready: false};
		chao.addFont(newFont);
		return newFont;
	},

	/**
	 * Adds a font to the chao fonts array.
	 *
	 * @param font - Font to be added.
	 */
	addFont: function(font){
		var oldFont = -1;

		for(var i = 0; i < chao.fonts.length; ++i){
			if(chao.fonts[i].key == font.key){
				oldFont = i;
				break;
			}
		}

		if(oldFont != -1){
			chao.fonts.splice(oldFont, 1);
		}

		// add text image for font ready state checking
		font.testImage = chao.createImage(undefined, 1, 1);
		font.testImage.context.font = "20px " + font.name;
		chao.drawText(font.testImage, font, "M", 0, 0, 20);
		font.testSize = chao.getTextSize(font.testImage, chao.fontTestString);

		chao.fonts.push(font);
	},

	/**
	 * Gets the font object by the given key.
	 *
	 * @param key - String identifying the font we wish to get.
	 * @return - Font object if found. If not found, or if the key is not a string, the given key param will be returned. It's designed like this to make some internal stuff faster.
	 */
	getFont: function(key){
		if(typeof key === "string" || key instanceof String){
			var n = chao.fonts.length;
			for(var i = 0; i < n; ++i){
				if(chao.fonts[i].key == key){
					return chao.fonts[i];
				}
			}
		}
		return key;
	},

	/**
	 * Draws text on the image wit hthe given font.
	 *
	 * @param image - Image to draw upon.
	 * @param font - Font to draw with.
	 * @param text - Text to draw.
	 * @param x - X position of the text.
	 * @param y - Y position of the text.
	 * @param size - Size of the text.
	 * @param color - Text color.
	 * @param align - Align of the text: "left", "right" or "center".
	 * @param outlineColor - Color of the text outline.
	 * @param outlineSize - Size of the text outline.
	 */
	drawText: function(image, font, text, x, y, size, color, align, outlineColor, outlineSize){
		color 			= color || 0xff000000;
		align 			= align || "left";
		outlineColor	= outlineColor || 0xff000000;
		outlineSize 	= outlineSize || 0;

		image.context.font 		= size.toFixed() + "px " + font.name;
		image.context.textAlign = align;

		chao.setFillStyle(image, color);
		image.context.fillText(text, x, y + size);
		if(outlineSize > 0){
			chao.setStrokeStyle(image, outlineColor, outlineSize);
			image.context.strokeText(text, x, y+size);
		}
	},

	/**
	 * Measures the text dimensions.
	 * @param image - With the set font.
	 * @param text - Text to be measured.
	 * @return - Object containing dimensions of the text: {width, height}.
	 */
	getTextSize: function(image, text){
		return {
			width: 	image.context.measureText(text).width,
			height:	image.context.measureText("M").width // well, it seems good enough.
		};
	},

	/**
	 * Updates the "ready" state of all fonts.
	 */
	updateFontsLoading: function(){
		var n 			= chao.fonts.length;
		var currentSize = 0;
		for(var i = 0; i < n; ++i){
			var font = chao.fonts[i];
			if(!font.ready){
				chao.drawText(font.testImage, font, "M", 0, 0, 20);
				currentSize = chao.getTextSize(font.testImage, chao.fontTestString);
				if(currentSize != font.testSize){
					font.ready = true;
				}
			}
		}
	},

	/**
	 * Loads a sound file.
	 *
	 * @param key - String by which this sound shall be identified.
	 * @param path - Path to the sound file.
	 * @param volume - Volume at which the sound will be played. Defaults to 1.0 when not provided.
	 * @param looped - Is the sound looped. Defaults to false.
	 * @param channels - How many times this sound could be played simultaneously. Defaults to 1.
	 * @return - Loaded sound object.
	 */
	loadSound: function(key, path, volume, looped, channels){
		if(channels < 1){
			chao.log("Can't add a sound with no channels, you silly goose.");
			return null;
		}
		if(chao.getSound(key) !== null){
			chao.log("There is already a sound loaded with this key: \"" + key + "\".");
			return null;
		}
		if(!chao.canPlayAudioType(path.split('.').pop())){
			chao.log("This browser will be unable to play this sound: " + path + ". Skipping!");
			return null;
		}

		var sound 				= {};

		sound.key 				= key;
		sound.channels			= [];
		sound.currentChannel	= 0;
		sound.isMusic 			= false;
		sound.volume 			= volume || 1;
		sound.ready 			= false;

		sound.channels.push(new Audio(path));
		sound.channels[0].onloadeddata = function(){
			sound.ready = true;
			for(var i = 0; i < (channels || 1); ++i){
				if(i > 0){
					sound.channels.push(new Audio(path));
				}

				if(looped){
					sound.channels[i].addEventListener('ended', function(){ this.currentTime=0; }, false);
					sound.channels[i].loop = true;
				}
			}
		}
		sound.channels[0].onerror = function(e){
			var msg = "Loading the \"" + sound.key + "\" sound failed with error: ";
			switch(e.currentTarget.error.code){
				case 1: msg += "MEDIA_ERR_ABORTED"; break;
				case 2: msg += "MEDIA_ERR_NETWORK"; break;
				case 3: msg += "MEDIA_ERR_DECODE"; break;
				case 4: msg += "MEDIA_ERR_SRC_NOT_SUPPORTED"; break;
				case 5: msg += "MEDIA_ERR_ENCRYPTED"; break;
			}
			chao.log(msg);
			sound.ready = true;
		}

		chao.sounds.push(sound);

		return sound;
	},

	/**
	 * Similar to the loadSound, but sets some music-specific thing up and allows passing a fallback file if the default one is not supported by current browser/device.
	 *
	 * @param key - String by which this sound shall be identified.
	 * @param path - Path to the audio file.
	 * @param fallbackFormatPath - Path to a fallback audio file, used if the main audio file cannot be played because the browser desen't support it.
	 * @param volume - Volume at which the music will be played.
	 * @return - Loaded sound object.
	 */
	loadMusic: function(key, path, fallbackFormatPath, volume){
		var sound = null;
		var mainExtension = path.split('.').pop();
		var fallbackExtension = fallbackFormatPath ? fallbackFormatPath.split('.').pop() : "";  

		if(chao.canPlayAudioType(mainExtension)){
			sound = chao.loadSound(key, path, volume, true, 1);
		} else if(chao.canPlayAudioType(fallbackExtension)){
			sound = chao.loadSound(key, fallbackFormatPath, volume, true, 1);
		}

		if(sound){
			sound.isMusic = true;
		}

		return sound;
	},

	/**
	 * Gets the sound object by the given key.
	 *
	 * @param key - String identifying the sound we wish to get.
	 * @return - Sound object if found. If not found, or if the key is not a string, the given key param will be returned. It's designed like this to make some internal stuff faster.
	 */
	getSound: function(key){
		if(typeof key === "string" || key instanceof String){
			var n = chao.sounds.length;
			for(var i = 0; i < n; ++i){
				if(chao.sounds[i].key === key){
					return chao.sounds[i];
				}
			}

			return null;
		}

		return key;
	},

	/**
	 * Plays a sound.
	 *
	 * @param key - Sound object of string id of the sound to be played.
	 * @param force - When true, the sound will be forcibly played from the start. Deafults to true.
	 */
	playSound: function(key, force){

		if(force === undefined){
			force = true;
		}

		var sound = chao.getSound(key);
		if(!sound){
			chao.log("There is no loaded sound with this key: \"" + key + "\".");
		}

		if(sound.isMusic){
			if(sound == chao.currentMusic && sound.playing){
				return;
			}
			if(chao.currentMusic && sound != chao.currentMusic){
				chao.stopSound(chao.currentMusic);
			}
			chao.currentMusic = sound;
		}

		if(sound.isMusic || !chao.muted){
			// Play the sound. Don't if sound it muted, but if it's a music, play it regardless. It will be paused in the next lines. :)
			
			sound.currentChannel ++;
			if(sound.currentChannel >= sound.channels.length){
				sound.currentChannel = 0;
			}

			if(force){
				sound.channels[sound.currentChannel].currentTime = 0;
			}
			sound.channels[sound.currentChannel].volume = sound.volume;
			var promise = sound.channels[sound.currentChannel].play();

			if (promise !== undefined) {
				promise.then( function() {
					// Whoa! We are fine!
				}).catch(function() {
					// Was unable to play sound. :(
					// Prolly the browser is messing with audio permissions. Will try to remsume it on the first user's input.
					if(sound.isMusic){
						chao.musicWasSupressed = true;
					}
				});
			}

		}
		if(sound.isMusic && chao.muted){
			chao.pauseSound(sound);
		}
	},

	/**
	 * Mute or resume the audio playback.
	 *
	 * @param value - True stops and prevents further audio playback, also pauses the music. Pass false to resume playback.
	 */
	setMute: function(value){
		if(chao.muted != value){
			chao.muted = value;

			if(value){
				for(var i = 0; i < chao.sounds.length; ++i){
					if(chao.sounds[i] == chao.currentMusic){
						continue;
					}

					chao.stopSound(chao.sounds[i]);
				}
			}

			if(chao.currentMusic !== null){
				if(value){
					chao.pauseSound(chao.currentMusic);
				} else {
					chao.playSound(chao.currentMusic, false);
				}
			}
		}
	},

	/**
	 * Toggles between muted and unmuted state.
	 */
	toggleMute: function(){
		chao.setMute(!chao.muted);
	},

	/**
	 * Stops playback of the given sound object.
	 *
	 * @param key - Sound object or string id of the sound to stop.
	 */
	stopSound: function(key){
		var sound = chao.getSound(key);
		
		for(var i = 0; i < sound.channels.length; ++i){
			sound.channels[i].pause();
			sound.channels[i].currentTime = 0;
		}
	},

	/**
	 * Returns sound position.
	 *
	 * @param key - Sound object or string id of the sound.
	 * @return - Position of the sound.
	 */
	getSoundPosition: function(key){
		var sound = chao.getSound(key);
		return sound.channels[sound.currentChannel].currentTime;
	},

	/**
	 * Sets sound position.
	 *
	 * @param key - Sound object or string id of the sound.
	 * @param position - Position to set.
	 */
	setSoundPosition: function(key, position){
		var sound = chao.getSound(key);
		sound.channels[sound.currentChannel].currentTime = position;
	},

	/**
	 * Pauses a sound.
	 *
	 * @param key - Sound object or string id of the sound.
	 */
	pauseSound: function(key){
		var sound = chao.getSound(key);
		sound.channels[sound.currentChannel].pause();
	},

	/**
	 * Resumes the music playback if needed.
	 */
	resumeMusicPlaybackIfNeeded: function(){
		if(chao.musicWasSupressed){
			chao.musicWasSupressed = false;
			chao.playSound(chao.currentMusic);
		}
	},

	/**
	 * Checks if a sound file of a given format can be played in the current environment.
	 *
	 * @param extension - String containing the file extension.
	 * @return - True if a file of given extension can be played.
	 */
	canPlayAudioType: function(extension){
		var audioTest = document.createElement('audio');
		if(!audioTest || !audioTest.canPlayType){
			return false;
		}

		switch(extension){
			case "ogg": {
				if(audioTest.canPlayType('audio/ogg; codecs="vorbis"').replace(/^no$/, '')){
				   return true;
				}
				break;
			}
			case "opus": {
				if(audioTest.canPlayType('audio/ogg; codecs="opus"').replace(/^no$/, '') || audioTest.canPlayType('audio/opus;').replace(/^no$/, '')){
					return true;
				}
				break;
			}
			case "mp3": {
				if(audioTest.canPlayType('audio/mpeg;').replace(/^no$/, '')){
					return true;
				}
				break;
			}
			case "wav": {
				if(audioTest.canPlayType('audio/wav; codecs="1"').replace(/^no$/, '')){
					return true;
				}
				break;
			}
			case "m4a": {
				if(audioTest.canPlayType('audio/x-m4a;') || audioTest.canPlayType('audio/aac;').replace(/^no$/, '')){
					return true;
				}
			}
		}

		return false;
	},

	/**
	 * Updates the mouse inputs. Called in chao.update().
	 */
	updateMouse: function(){
		chao.mouse.wheelDelta 			= 0;
		chao.mouse.justPressed			= false;
		chao.mouse.justReleased			= false;
		chao.mouse.justPressedRight		= false;
		chao.mouse.justReleasedRight	= false;
		chao.mouse.justPressedMiddle	= false;
		chao.mouse.justReleasedMiddle	= false;
	},

	/**
	 * Finds an entity that happens to be under the mouse cursor.
	 *
	 * @return - An entity under the mouse cursor. If none were found, you get a sweet null.
	 */
	getEntityUnderMouse: function(){
		return chao.getCurrentState().rootEntity.getEntityAt(chao.mouse.x, chao.mouse.y);
	},

	/**
	 * Handles mouse clicks and the first touch input, which is handled like the left mouse button.
	 *
	 * @param button - Id of a button that is being handled. (1 - left, 2 - middle, 3 - right)
	 */
	handleMouseDown: function(button){
		chao.resumeMusicPlaybackIfNeeded();

		if(chao.mouse.suppressUntilUp){
			return;
		}

		switch(button){
			case 1:
				chao.mouse.pressed = chao.mouse.justPressed = true;
				chao.focusedEntity = chao.getEntityUnderMouse();
				if(chao.focusedEntity != null){
					chao.focusedEntity.onClick();
				}
				break;
			case 2:
				chao.mouse.pressedMiddle = chao.mouse.justPressedMiddle = true;
				break;
			case 3:
				chao.mouse.pressedRight = chao.mouse.justPressedRight = true;
				break;
		}
	},

	/**
	 * Handles mouse and the first touch input releases.
	 *
	 * @param button - Id of a button that is being handled. (1 - left, 2 - middle, 3 - right)
	 */
	handleMouseUp: function(button){

		if(chao.mouse.suppressUntilUp){
			chao.mouse.suppressUntilUp = false;
		}

		switch(button){
			case 1:
				chao.mouse.pressed 				= false;
				chao.mouse.justReleased 		= true;
				if(chao.focusedEntity != null){
					if(chao.getEntityUnderMouse() != chao.focusedEntity){
						chao.focusedEntity.onCancel();
					} else {
						chao.focusedEntity.onRelease();
					}
					chao.focusedEntity = null;
				}
				break;
			case 2:
				chao.mouse.pressedMiddle 		= false;
				chao.mouse.justReleasedMiddle 	= true;
				break;
			case 3:
				chao.mouse.pressedRight 		= false;
				chao.mouse.justReleasedRight 	= true;
				break;
		}
	},

	/**
	 * Handles mouse and the first touch input movement.
	 *
	 * @param x - X position of the pointer, from the left edge of the viewport.
	 * @param y - Y position of the pointer, from the top edge of the viewport.
	 */
	handleMouseMove: function(x, y){
		chao.mouse.x = x;
		chao.mouse.y = y;

		var currentEntity = chao.getEntityUnderMouse();
		if(currentEntity != chao.focusedEntity){
			if(chao.focusedEntity && !chao.focusedEntity.keepClickFocus){
				chao.focusedEntity.onCancel();
			}
			if(!chao.focusedEntity || !chao.focusedEntity.keepClickFocus){
				chao.focusedEntity = currentEntity;
			}
		}

		if(chao.focusedEntity){
			chao.focusedEntity.onMove();
		}
	},

	/**
	 * Mouse down callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onMouseDown: function(e){
		chao.handleMouseDown(e.which);
		e.preventDefault();
	},

	/**
	 * Mouse up callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onMouseUp: function(e){
		chao.handleMouseUp(e.which);
		e.preventDefault();
	},

	/**
	 * Mouse move callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onMouseMove: function(e){
		chao.handleMouseMove(e.offsetX, e.offsetY);
		e.preventDefault();
	},
	
	/**
	 * Mouse wheel callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onMouseWheel: function(e){
		chao.mouse.wheelDelta = e.deltaY;
		e.preventDefault();
	},

	/**
	 * Sets the visibility of the default system cursor.
	 *
	 * @param value - True if we want to see it, false otherwise.
	 */
	setMouseVisibility: function(value){
		canvas.canvas.style.cursor = value ? "auto" : "none";
	},

	/**
	 * Updates touch inputs. Called in chao.update();
	 */
	updateTouches: function(){
		for(var i = 0; i < chao.touches.length; ++i){
			chao.touches[i].justPressed = false;
		}
	},

	/**
	 * Touch start callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onTouchStart: function(e){
		var touches = e.changedTouches;
		for(var i = 0; i < touches.length; ++i){
			var touchPos = chao.getTouchPos(touches[i]);
			var newTouch = {
				id: touches[i].identifier,
				x: touchPos.x,
				y: touchPos.y,
				justPressed: true,
				isMouse: chao.touches.length == 0
			};
			chao.touches.push(newTouch);

			if(newTouch.isMouse){
				chao.handleMouseDown(1);
				chao.handleMouseMove(newTouch.x, newTouch.y);
			}
		}

		if(e.cancelable){
			e.preventDefault();
		}
	},

	/**
	 * Touch move callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onTouchMove: function(e){
		var touches = e.changedTouches;
		for(var i = 0; i < touches.length; ++i){
			var touch 		= chao.getTouch(touches[i].identifier);
			var touchPos 	= chao.getTouchPos(touches[i]);
			if(touch != null){
				touch.x = touchPos.x;
				touch.y = touchPos.y;

				if(touch.isMouse){
					chao.handleMouseMove(touch.x, touch.y);
				}
			}
		}
		if(e.cancelable){
			e.preventDefault();
		}
	},

	/**
	 * Touch end callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onTouchEnd: function(e){
		var touches = e.changedTouches;
		for(var i = 0; i < touches.length; ++i){
			var touch = chao.getTouch(touches[i].identifier);
			if(touch != null){
				if(touch.isMouse){
					chao.handleMouseUp(1);
				}

				var index = chao.touches.indexOf(touch);
				chao.touches.splice(index, 1); // BALEETED!
			}
		}

		if(e.cancelable){
			e.preventDefault();
		}
	},

	/**
	 * Finds and returns a touch object.
	 *
	 * @param id - Unique id of the touch.
	 * @return - A touch object if found or false if none exists with the given id.
	 */
	getTouch: function(id){
		for(var i = 0; i < chao.touches.length; ++i){
			if(chao.touches[i].id == id){
				return chao.touches[i];
			}
		}
		return null;
	},

	/**
	 * Internal helper to convert the convoluted touch coords to a reasonable ones.
	 *
	 * @param touch - Touch object to get fixed coords from.
	 */
	getTouchPos: function(touch){
		return {
			x: (touch.pageX - touch.target.offsetLeft) / chao.screenScaleX,
			y: (touch.pageY - touch.target.offsetTop) / chao.screenScaleY
		};
	},

	/**
	 * Updates keyboard inputs. Called in chao.update().
	 */
	updateKeys: function(){
		for (var i = 0; i < 0x80; ++i){
			chao.justPressed[i] = false;
			chao.justReleased[i] = false;
		}
	},

	/**
	 * Keyboard key down callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onKeyDown: function(e){
		if(!chao.keys[e.keyCode]){
			chao.justPressed[e.keyCode] = true;
		}
		chao.keys[e.keyCode] = true;

		e.preventDefault();
	},

	/**
	 * Keyboard key up callback.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	onKeyUp: function(e){
		chao.justReleased[e.keyCode] = true;
		chao.keys[e.keyCode] = false;

		e.preventDefault();
	},

	/**
	 * Called when game window is resized. Applies scaling modes.
	 *
	 * @param e - Event passed by the Event Listener.
	 */
	resize: function(e){
		if(chao.scalingMode <= chao.SCALING_MODE_NONE || chao.scalingMode >= chao.SCALING_MODE_END){
			return;
		}

		var canvas	= chao.canvas.canvas;
		var scaleX 	= window.innerWidth / chao.baseScreenWidth;
		var scaleY 	= window.innerHeight / chao.baseScreenHeight;
		var scale 	= Math.min(scaleX, scaleY);

		switch(chao.scalingMode){
			case chao.SCALING_MODE_STRETCH:{
				chao.setCanvasScale(scaleX, scaleY);
				break;
			}
			case chao.SCALING_MODE_KEEP_RATIO:{
				chao.setCanvasScale(scale, scale);
				var center = "horizontally";
				if((canvas.width > canvas.height && canvas.width * scale >= window.innerWidth)
					|| (canvas.width <= canvas.height && canvas.height * scale < window.innerHeight)){
					center = "vertically";
				}

				var margin;
				if (center === "horizontally") {
					margin = (window.innerWidth - canvas.width * scale) / 2;
					canvas.style.marginLeft = margin + "px";
					canvas.style.marginRight = margin + "px";
				} else {
					margin = (window.innerHeight - canvas.height * scale) / 2;
					canvas.style.marginTop = margin + "px";
					canvas.style.marginBottom = margin + "px";
				}

				break;
			}
			case chao.SCALING_MODE_EXTEND:{
				chao.setCanvasScale(scale, scale);

				if(chao.baseScreenWidth * scale < window.innerWidth){
					// extend the viewport horizontally
					var missingPixs 	= window.innerWidth - (chao.baseScreenWidth * scale);
					chao.screenWidth	= chao.baseScreenWidth + (missingPixs / scale);
					chao.screenHeight	= chao.baseScreenHeight;
				} else {
					// extend the viewport vertically
					var missingPixs 	= window.innerHeight - (chao.baseScreenHeight * scale);
					chao.screenWidth	= chao.baseScreenWidth;
					chao.screenHeight	= chao.baseScreenHeight + (missingPixs / scale);
				}
				chao.canvas.width = chao.screenWidth;
				chao.canvas.height = chao.screenHeight;
				canvas.setAttribute("width", chao.screenWidth);
				canvas.setAttribute("height", chao.screenHeight);

				chao.updatePauseFadeImage();
				break;
			}
		}

		// Fix Safari scaling
		var ua = navigator.userAgent.toLowerCase(); 
		if (ua.indexOf("safari") != -1) { 
			if (ua.indexOf("chrome") > -1) {
				// Chrome
			} else {
				// Safari
				chao.canvas.canvas.style.maxHeight = "100%";
				chao.canvas.canvas.style.minHeight = "100%";
			}
		}
		if(chao.loadingState && chao.loadingState.rootEntity){
			chao.loadingState.rootEntity.width 	= chao.screenWidth;
			chao.loadingState.rootEntity.height = chao.screenHeight;
			if(chao.loadingState.resize){
				chao.loadingState.resize();
			}
		}
		if(chao.currentState && chao.currentState.rootEntity){
			chao.currentState.rootEntity.width 	= chao.screenWidth;
			chao.currentState.rootEntity.height = chao.screenHeight;
			if(chao.currentState.resize){
				chao.currentState.resize();
			}
		}
	},

	/**
	 * Stretches the game viewport canvas to given scale.
	 *
	 * @param x - Horizontal scale of the canvas.
	 * @param y - Vertical scale of the canvas.
	 */
	setCanvasScale: function(x, y){
		chao.screenScaleX = x;
		chao.screenScaleY = y;

		var canvas = chao.canvas.canvas;
		canvas.style.marginLeft		= "0px";
		canvas.style.marginRight	= "0px";
		canvas.style.marginTop		= "0px";
		canvas.style.marginBottom	= "0px";
		canvas.style.paddingLeft 	= 0;
		canvas.style.paddingRight 	= 0;
		canvas.style.paddingTop 	= 0;
		canvas.style.paddingBottom 	= 0;
		canvas.style.display 		= "block";
		canvas.scaled				= true;

		canvas.style.transformOrigin 	= "0 0";
		canvas.style.transform 			= "scale(" + x + "," + y + ")";
	},

	/**
	 * Counts loading progress based on all assets "ready" state.
	 * 
	 * @return - Loading progress as a float between 0.0 and 1.0.
	 */
	getLoadingProgress: function(){
		var allData 	= chao.images.length + chao.sounds.length;
		var loadedData 	= 0;

		for(var i = 0; i < chao.images.length; ++i){
			if(chao.images[i].ready){
				loadedData ++;
			}
		}

		for(var i = 0; i < chao.sounds.length; ++i){
			if(chao.sounds[i].ready){
				loadedData ++;
			}
		}

		return loadedData / allData;
	},

	/**
	 * Returns milliseconds that passed since the last frame. Scaled by chao.timeScale.
	 *
	 * @return - Milliseconds that passed since the last frame.
	 */
	getTimeDelta: function() {
		return chao.timeDelta * chao.timeScale;
	},

	/**
	 * Returns milliseconds that passed since the last frame. Not affected by chao.timeScale so useful for pause menus etc.
	 *
	 * @return - Milliseconds that passed since the last frame.
	 */
	getUnscaledDelta: function() {
		return chao.timeDelta;
	},

	/**
	 * Creates a point object.
	 *
	 * @param x - X component of the point.
	 * @param y - Y component of the point.
	 * @return Created point object.
	 */
	makePoint: function(x, y) {
	 	return {x: x, y: y};
	},

	/**
	 * Creates a vector out of two given points.
	 *
	 * @param pointFrom - The beginning point of the vector.
	 * @param pointTo - The end point of the vector.
	 * @return Created vector object.
	 */
	makeVector: function(pointFrom, pointTo){
	 	return { x: pointTo.x - pointFrom.x, y : pointTo.y - pointFrom.y };
	},

	/**
	 * Calculates length of the given vector.
	 *
	 * @param vec - Vector object, in {x, y} format.
	 * @return Length of the vector.
	 */
	getVectorLength: function(vec){
		return Math.sqrt((vec.x*vec.x) + (vec.y*vec.y));
	},

	/**
	 * Normalizes the given vector.
	 *
	 * @param vec - Vector object to normalize, in {x, y} format.
	 */
	normalizeVector: function(vec){
		var len = chao.getVectorLength(vec);
		vec.x /= len;
		vec.y /= len;
	},

	/**
	 * Checks there is an intersection between the two lines.
	 *
	 * @param line1a - Beginning of the first line.
	 * @param line1b - End of the first line.
	 * @param line2a - Beginning of the second line.
	 * @param line2b - End of the second line.
	 * @return True if the two lines interset, false if there is no intersection or lines are the same/collinear.
	 */
	areLinesIntersecting: function(line1a, line1b, line2a, line2b){
		var det = (line1b.x - line1a.x) * (line2b.y - line2a.y) - (line2b.x - line2a.x) * (line1b.y - line1a.y);
		if(det === 0){
			return false;
		}
    
		var lambda = ((line2b.y - line2a.y) * (line2b.x - line1a.x) + (line2a.x - line2b.x) * (line2b.y - line1a.y)) / det;
		var gamma = ((line1a.y - line1b.y) * (line2b.x - line1a.x) + (line1b.x - line1a.x) * (line2b.y - line1a.y)) / det;
		return (lambda > 0 && lambda < 1) && (gamma > 0 && gamma < 1);
	},

	/**
	 * Creates an object describing a polygon.
	 *
	 * @param points - All the points shaping the polygon. Can be array of points (see makePoint()) or just a simple array built like this: [x1, y1, x2, y2, ...].
	 * @return An object with polygon data in it!
	 */
	makePolygon: function(points){
		if(!Array.isArray(points) || points.length < 1){
			chao.log("makePolygon: points param is not an array or has no elements. :(");
			return;
		}

		if(typeof points[0] != "object"){
			var newPoints = [];
			for(var i = 0; i < points.length/2; ++i){
				newPoints.push({x: points[i*2], y: points[i*2+1]});
			}
			points = newPoints;
		}

		var left 	= Number.MAX_VALUE;
		var right 	= -Number.MAX_VALUE;
		var top 	= Number.MAX_VALUE;
		var bottom 	= -Number.MAX_VALUE;

		for(var i = 0; i < points.length; ++i){
			if(points[i].x < left) 		left 	= points[i].x;
			if(points[i].x > right) 	right 	= points[i].x;
			if(points[i].y < top) 		top 	= points[i].y;
			if(points[i].y > bottom) 	bottom 	= points[i].y;
		}

		return {
			points: points,	// Points shaping the poly
			left: left,		// The far left point
			right: right,	// The far right point
			top: top,		// Highest point
			bottom: bottom	// Lowest point
		};
	},

	/**
	 * Checks if a point {x,y} is inside a polygon.
	 *
	 * @param point - The beginning point of the vector.
	 * @param polygon - The end point of the vector.
	 * @return True if the point is indeed inside the polygon.
	 */
	isPointInsidePolygon: function(point, polygon){
		if(point.x < polygon.left || point.x > polygon.right || point.y < polygon.top || point.y > polygon.bottom){
			return false;
		}

		var intersections 	= 0
		var raycastOrigin 	= {x: polygon.left - 1, y: point.y};
		var pointsNum 		= polygon.points.length;
		var polyLineA;
		var polyLineB;

		for(var i = 0; i < pointsNum; ++i){
			polyLineA = polygon.points[i];
			polyLineB = i === 0 ? polygon.points[pointsNum-1] : polygon.points[i-1];

			if(chao.areLinesIntersecting(raycastOrigin, point, polyLineA, polyLineB)){
				intersections ++;
			}
		}

		if(intersections & 1 == 1){
			return true;
		}

		return false;
	},

	/**
	 * Generates a random integer between 0 (inclusive) and max (exclusive).
	 *
	 * @param max - Maximum value of the generated integer will be lesser than this.
	 */
	getRandom: function(max){ 
		max -= 1;
		return Math.round(max*Math.random()); 
	},

	/**
	 * Converts radians to degrees.
	 *
	 * @param radians - Radians to convert.
	 * @return - Angle in degrees.
	 */
	rad2deg: function(radians){
		return radians/(180/Math.PI);
	},

	/**
	 * Clamps the given value between min (inclusive) and max (inclusive).
	 *
	 * @param value - Value to clamp.
	 * @param min - We don't want the value to be anything less than this (inclusive).
	 * @param max - We don't want the value to be anything more than this (inclusive).
	 * @return - Suprise! A clamped value!
	 */
	clamp: function(value, min, max){
		if(value < min) value = min;
		if(value > max) value = max;
		return value;
	},

	/**
	 * Sets up the place to put all the logs into.
	 *
	 * @param htmlElementId -  HTML element to insert all the logs into.
	 */
	setupLogTarget: function(htmlElementId){
		chao.debugLogTarget = document.getElementById(htmlElementId);
		window.addEventListener("error", function(e) {
			var fa = e.filename.split("/");
			fa.reverse();
			chao.log("["+ fa[0]+":"+e.lineno+":"+e.colno+"] " + e.message);
		});
	},

	/**
	 * Log a thing to the browser's js console. Set chao.loggingEnabled to false to prevent the logging.
	 * 
	 * @param thingie - A thingie to put in the console.
	 */
	log: function(thingie){
		if(chao.loggingEnabled){
			if(chao.debugLogTarget){
				chao.debugLogTarget.innerHTML += String(thingie).replace(/\n/g, "<br/>") + "<br/>";
			} else {
				console.log(thingie);
			}
		}
	},

	/**
	 * Draws a pretty log of the hierarchy of all the children sticked to the given entity,
	 *
	 * @param entity - An entity to draw the hierarchy log for.
	 */
	logHierarchy: function(entity){
		var logString = chao.logEntity(entity, 0);
		chao.log(logString);
	},

	/**
	 * Used by logHierarchy to recursively create a hierarchy log string for given entity.
	 *
	 * @param entity - Entity to make a log string for.
	 * @param indent - Indentation level for the currently generated string.
	 */
	logEntity: function(entity, indent){

		entity 	= entity || chao.getCurrentState().rootEntity;
		indent 	= indent || 0;

		var entityLog = "";
		for(var i = 0; i < indent; ++i){
			entityLog += "  ";
		}
		entityLog += entity.name;
		if(entity.components.length > 0){
			entityLog += " (";
			for(var i = 0; i < entity.components.length; ++i){
				entityLog += entity.components[i].name;
				if(i < entity.components.length - 1) entityLog += ", ";
			}
			entityLog += ")";
		}

		entityLog += " p:" + Math.ceil(entity.x) + "x" + Math.ceil(entity.y);
		entityLog += " s:" + Math.ceil(entity.width) + "x" + Math.ceil(entity.height);

		for(var i = 0; i < entity.children.length; ++i){
			entityLog += chao.logEntity(entity.children[i], indent + 1);
		}

		return "\n" + entityLog;
	},

	/**
	 * Sets up some event listeners so we know wneh the game's window loses focus. Called somewhere in chao.init().
	 */
	installVisibilityHandler: function(){

		if(chao.visibilityHandlerInstalled){
			chao.log("Visibility Handler is already installed!");
			return;
		}
		
		chao.visibilityHandlerInstalled = true;

		var hiddenVar;
		
		if (document.hidden !== undefined){
			hiddenVar = "visibilitychange";
		} else {
			var vendors = [ "webkit", "moz", "ms" ];

			vendors.forEach(function (prefix){
				if (document[prefix + "Hidden"] !== undefined){
					document.hidden = function (){
						return document[prefix + "Hidden"];
					};
					hiddenVar = prefix + "visibilitychange";
				}
			});
		}

		if(hiddenVar){
			document.addEventListener(hiddenVar, function(event){
				if(document.hidden || event.type === "pause"){
					chao.onFocusChange(false);
				} else {
					chao.onFocusChange(true);
				}
			}, false);
		}

		window.onblur = function(){
			chao.onFocusChange(false);
		}
		window.onfocus = function(){
			chao.onFocusChange(true);
		}
	},

	/**
	 * Updates all tween anims. Called in chao.update().
	 */
	updateTweens: function(){
		var tweensToRemove = [];

		for(var i = 0; i < chao.tweens.length; ++i){
			var tween = chao.tweens[i];

			tween.timer += tween.useUnscaledTime ? chao.getUnscaledDelta() : chao.getTimeDelta();

			if(tween.delay > 0){
				if(tween.timer >= tween.delay){
					tween.delay = 0;
					tween.timer = 0;
				} else {
					continue;
				}
			}

			if(tween.timer >= tween.lifetime){
				switch(tween.repeatMode){
					case chao.REPEAT_MODE_ONCE:{
						tween.timer = tween.lifetime;
						tween.finished = true;
						tweensToRemove.push(tween);
						break;
					}
					case chao.REPEAT_MODE_LOOP:{
						tween.timer = 0.0;
						if(tween.finishCallback){
							tween.finishCallback.call(tween.target);
						}
						break;
					}
					case chao.REPEAT_MODE_BOUNCE:{
						tween.timer = 0.0;
						tween.direction = tween.direction == 1 ? -1 : 1;
						if(tween.finishCallback){
							tween.finishCallback.call(tween.target);
						}
					}
				}
			}

			var v = tween.timer / tween.lifetime;
			if(tween.direction < 0){
				v = 1 - v;
			}
			switch(tween.interpolationType){
				case chao.INTERPOLATE_SMOOTH: {
					v = v * v * (3 - 2 * v);
					break;
				}
				case chao.INTERPOLATE_EASE_TO: {
					v = 1 - (1 - v) * (1 - v);
					break;
				}
				case chao.INTERPOLATE_EASE_FROM: {
					v = v * v;
					break;
				}
				case chao.INTERPOLATE_BOUNCE: {
					if ( v < ( 1.0 / 2.75 ) ) {
						v = 7.5625 * v * v;
					} else if ( v < ( 2.0 / 2.75 ) ) {
						v = 7.5625 * ( v -= ( 1.5 / 2.75 ) ) * v + 0.75;
					} else if ( v < ( 2.5 / 2.75 ) ) {
						v = 7.5625 * ( v -= ( 2.25 / 2.75 ) ) * v + 0.9375;
					} else {
						v = 7.5625 * ( v -= ( 2.625 / 2.75 ) ) * v + 0.984375;
					}
				}
			}
			tween.target[tween.varName] = (tween.to * v) + (tween.from * (1 - v));
		}

		// bring out your dead!
		for(var i = 0; i < tweensToRemove.length; ++i){
			var id = chao.tweens.indexOf(tweensToRemove[i]);
			var tween = chao.tweens[id];
			if(tween.finishCallback){
				tween.finishCallback.call(tween.target);
			}
			chao.tweens.splice(id, 1);
		}
	},

	/**
	 * Add a new tween anim.
	 *
	 * @param target - An object containing the variable we will be tweening.
	 * @param varName - Name of the variable that will be tweened.
	 * @param from - Value to tween from.
	 * @param to - Target value to tween to.
	 * @param time - Length of the tween in seconds.
	 * @param interpolationType - Type of interpolation. Use the chao.INTERPOLATE_* consts. Defaults to chao.INTERPOLATE_LINEAR.
	 * @param repeatMode - How (and if) the tween shall be looped. Use the chao.REPEAT_MODE_* consts. Defaults to chao.REPEAT_MODE_ONCE.
	 * @param delay - Delay in seconds before the tweening starts.
	 * @param finishCallback - Function to be called when the tween has finished its job.
	 * @return - Tween object containing info about this tween.
	 */
	addTween: function(target, varName, from, to, time, interpolationType, repeatMode, delay, finishCallback){
		var newTween = {
			target: 			target,								// Object containing the tweened var.
			varName: 			varName,							// Name of the tweened var.
			from: 				from,								// Value to tween from.
			to: 				to,									// Target value to tween to.
			lifetime: 			time != undefined ?  time : 1,		// Length of the tween in seconds.
			interpolationType: 	interpolationType != undefined ? interpolationType : chao.INTERPOLATE_LINEAR, // Type of interpolation.
			repeatMode: 		repeatMode != undefined ? repeatMode : chao.REPEAT_MODE_ONCE, // Ow (and if) the tween shall be looped.
			delay: 				delay || 0,							// Delay in seconds before the tweening starts.
			finishCallback: 	finishCallback,						// Function to be called when the tween has finished its job.

			finished: 			false,								// Just in case you would like to know for any reason.
			useUnscaledTime: 	false,								// If set, the unscaled time delta will be used.

			timer: 				0,
			direction: 			1,

		}

		chao.tweens.push(newTween);
		return newTween;
	},

	/**
	 * Removes a tween.
	 *
	 * @param tween - Tween object to be removed.
	 */
	removeTween: function(tween){
		var i = chao.tweens.indexOf(tween);
		if(i != -1){
			chao.tweens.splice(i, 1);
		}
	},

	/**
	 * Remove all tweens affecting the given entity or its components.
	 *
	 * @param entity - Entity to remove all the tweens from.
	 */
	removeTweensFromEntity: function(entity){
		var tweensToRemove = [];

		for(var i = 0; i < chao.tweens.length; ++i){
			if(chao.tweens[i].target == entity){
				tweensToRemove.push(chao.tweens[i]);
				continue;
			}

			for(var j = 0; j < entity.components.length; ++j){
				if(chao.tweens[i].target == entity.components[i]){
					tweensToRemove.push(chao.tweens[i]);
					break;
				}

			}
		}

		for(var i = 0; i < tweensToRemove.length; ++i){
			var id = chao.tweens.indexOf(tweensToRemove[i]);
			chao.tweens.splice(id, 1);
		}
	},

	/**
	 * Force finish all the tweens. Useful eg. for skipping cutscenes.
	 *
	 * @param includeLoops - When true, it will forcibly finish also the looping tweens.
	 */
	finishAllTweens: function(includeLoops){
		for(var i = 0; i < chao.tweens.length; ++i){
			var tween = chao.tweens[i];
			if(tween.repeatMode == chao.REPEAT_MODE_ONCE || includeLoops){
				tween.repeatMode = chao.REPEAT_MODE_ONCE; // in case "includeLoops" is on
				tween.timer = tween.lifetime;
			}
		}

	},

	/**
	 * Removes all the tweens without asking questions.
	 */
	clearTweens: function(){
		chao.tweens = [];
	},

	/**
	 * Sub-namespace containing some useful helper functions.
	 */
	helpers: {

		/**
		 * Creates an entity and sticks ComponentImage to it.
		 *
		 * @param entityName - Name for the created entity.
		 * @param image - Image or its id.
		 * @param x - X position of the entity.
		 * @param y - Y position of the entity.
		 * @return - Created ComponentImage component.
		 */
		createImage: function(entityName, image, x, y){
			return (new Entity(entityName, x, y)).addComponent(new ComponentImage(image));
		},

		/**
		 * Creates an entity and sticks ComponentText to it.
		 *
		 * @param entityName - Name for the created entity.
		 * @param x - X position of the entity.
		 * @param y - Y position of the entity.
		 * @param font - Font to use for the text object.
		 * @param text - Text that will be displayed.
		 * @param size - Size of the text.
		 * @return - Created ComponentText component.
		 */
		createText: function(entityName, x, y, font, text, size){
			return (new Entity(entityName, x, y)).addComponent(new ComponentText(font, text, size));
		},

		/**
		 * Creates an entity and sticks ComponentButton to it.
		 *
		 * @param entityName - Name for the created entity.
		 * @param x - X position of the entity.
		 * @param y - Y position of the entity.
		 * @param image - Button image or its id.
		 * @param imagePressed - Image to be used for the button's pressed state.
		 * @param font - Font to use as the label for this button.
		 * @param text - Text that will be displayed as a label.
		 * @param size - Size of the text.
		 * @return - Created ComponentButton component.
		 */
		createButton: function(entityName, x, y, image, imagePressed, font, fontSize, text){
			var newButton = (new Entity(entityName, x, y)).addComponent(new ComponentButton(image));

			if(imagePressed){
				newButton.setImagePressed(imagePressed);
			}

			if(font && fontSize && text){
				newButton.setText(text, font, fontSize);
			}

			return newButton;
		},

		/**
		 * Fades out the entity.
		 *
		 * @param entity - Entity to fade out.
		 * @param time - Seconds the fade will take.
		 * @param delay - Seconds before the fade will start.
		 */
		fadeEntityOut: function(entity, time, delay){
			chao.addTween(entity, "alpha", 1.0, 0.0, time || 0.25, chao.INTERPOLATE_LINEAR, chao.REPEAT_MODE_ONCE, delay || 0.0);
		},

		/**
		 * Fades in the entity.
		 *
		 * @param entity - Entity to fade in.
		 * @param time - Seconds the fade will take.
		 * @param delay - Seconds before the fade will start.
		 */
		fadeEntityIn: function(entity, time, delay){
			chao.addTween(entity, "alpha", 0.0, 1.0, time || 0.25, chao.INTERPOLATE_LINEAR, chao.REPEAT_MODE_ONCE, delay || 0.0);
		},

		/**
		 * Adds a bouncy-bounce to the entity.
		 *
		 * @param entity - Entity to apply the bounce tween to.
		 * @param amplitude - Bounce amplitude in pixels.
		 * @param time - Bounce time in seconds.
		 */
		addBounceTween: function(entity, amplitude, time){
			chao.addTween(entity, "y", entity.y-amplitude/2, entity.y+amplitude/2, time, chao.INTERPOLATE_SMOOTH, chao.REPEAT_MODE_BOUNCE);
		},

		/**
		 * Shake an entity. This is done by adding a disposable ComponentShake to the entity that will vanish after the shake is performed.
		 *
		 * @param entity - An entity to be shaken.
		 * @param force - Max shake range in pixels.
		 * @param time - How long the the shaking will last, in seconds.
		 * @param damped - Is the shake supposed to be damped over time.
		 */
		shake: function(entity, force, time, damped){
			var shakerName = "Disposable Shake";

			var oldShake = entity.getComponentByName(shakerName);
			if(oldShake != null){
				entity.removeComponent(oldShake);
			}

			var newShake 		= new ComponentShake(force, time, damped);
			newShake.name 		= shakerName;
			newShake.disposable = true;
			entity.addComponent(newShake);
			newShake.shake();
		}
	}

};

/**
 * A basic game entity.
 *
 * @param x - Horizontal position of the entity
 * @param y - Vertical position of the entity
 */
function Entity(name, x, y){
	this.name 			= name || "Entity",	// Name by which this component is identified.
	this.x 				= x || 0,			// Horizontal position of this entity, from the left edge of the viewport.
	this.y 				= y || 0,			// Vertical position of this entity, from the top edge of the viewport.
	this.alpha 			= 1.0;				// Entity's opacity.
	this.width 			= 0,				// Width of the entity.
	this.height 		= 0,				// Height of the enity.
	this.scaleX 		= 1.0;				// Horizontal scale.
	this.scaleY 		= 1.0;				// Vertical scale.
	this.rotation 		= 0.0;				// Angle, in degrees.
	this.children 		= [],				// Sub-entities attached to this entity. They will follow this entity's position and alpha values.
	this.components 	= [],				// Components attached to this entity.
	this.parent 		= null,				// Parent object.
	this.visible 		= true,				// Is rendering enabled for this entity.
	this.paused			= false,			// When true, no updates will happen for this entity, its components and all the children.
	this.clickable 		= false,			// When true, this entity will receive mouse/touch input events.
	this.keepClickFocus	= false; 			// When true, the entity will keep the click focus even when the pointer slides off it. when false, the onCancel will be called when pointer slides off.

	/**
	 * Destroys all children and components sticked to this entity.
	 */
	this.destroy = function(){
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].destroy){
				this.components[i].destroy();
			}
		}

		for(var i = 0; i < this.children.length; ++i){
			this.children[i].destroy();
		}

		this.children	= [];
		this.components	= [];
	}

	/**
	 * Draws this entity, its components and all the children.
	 *
	 * @param x - X position that parent of this entity is displayed.
	 * @param y - Y position that parent of this entity is displayed.
	 * @param alpha - Our parent's opacity.
	 */
	this.draw = function(x, y, alpha){
		if(!this.visible){
			return;
		}

		var componentsNum = this.components.length;
		for(var i = 0; i < componentsNum; ++i){
			if(this.components[i].draw){
				this.components[i].draw(x, y, alpha);
			}
		}

		var childrenNum = this.children.length;
		for(var i = 0; i < childrenNum; ++i){
			if(this.children[i].draw){
				this.children[i].draw(this.x + x, this.y + y, this.alpha * alpha);
			}
		}
	}

	/**
	 * Updates this entity, components and all the children.
	 */
	this.update = function(){
		if(this.paused || !this.visible){
			return;
		}

		var componentsNum = this.components.length;
		for(var i = 0; i < componentsNum; ++i){
			if(this.components[i].update){
				this.components[i].update();
			}
		}

		var childrenNum = this.children.length;
		for(var i = 0; i < childrenNum; ++i){
			if(this.children[i].update){
				this.children[i].update();
			}
		}
	}

	/**
	 * Adds a child entity.
	 *
	 * @param child - A child entity to be added to the hierarchy. Could also be a component and this method will add the entity its sticked to.
	 */
	this.add = function(child){
		if(child.parent === undefined){
			chao.log("The object you are trying to add as an entity is not an Entity.");
			chao.log(child);
			return;
		}
		this.children.push(child);
		child.parent = this;
		return child;
	}

	/**
	 * Adds a child entity.
	 *
	 * @param child - A child entity to be added to the hierarchy.
	 */
	this.remove = function(child){
		if(child.parent === this){
			this.children.splice(this.children.indexOf(child), 1);
			return child;
		}
	}

	/**
	 * Adds a new component to this entity and initializes it via the component's create() function.
	 *
	 * @param component - A new component to be added.
	 * @return - A reference to the newly added component.
	 */
	this.addComponent = function(component){
		if(!component.entity){
			component.entity = this;
			if(component.create){
				component.create();
			}
			this.components.push(component);

			return component;
		}else{
			chao.log("Hey, this component is already bound to an Entity: " + component.entity);
		}

		return null;
	}

	/**
	 * Returns a component sticked to this entity.
	 *
	 * @param componentName - Name of the component to be searched for.
	 * @return - A component, if found. Otherwise, a null.
	 */
	this.getComponentByName = function(componentName){
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].name === componentName){
				return this.components[i];
			}
		}

		return null;
	}

	/**
	 * Recursively searches for a component of given name in this entity and all its children.
	 *
	 * @param componentName - Name of the component to be searched for.
	 * @return - A component, if found. Otherwise, a null.
	 */
	this.getComponentInChildrenByName = function(componentName){
		var foundComponent = this.getComponentByName(componentName);
		if(foundComponent != null){
			return foundComponent;
		}

		for(var i = 0; i < this.children.length; ++i){
			foundComponent = this.children[i].getComponentInChildrenByName(componentName);
			if(foundComponent != null){
				return foundComponent;
			}
		}

		return null;
	}

	/**
	 * Removes a component from tis entity. :(
	 *
	 * @param component - Reference to the component about to be removed.
	 */
	this.removeComponent = function(component){
		if(component.entity === this){
			this.components.splice(this.components.indexOf(component), 1);
		} else {
			chao.log("Entity " + this + " have no such component: " + component);
		}
	}

	/**
	 * Removes a component from tis entity. :(
	 *
	 * @param componentName - Name of the component about to be removed.
	 */
	this.removeComponentByName = function(componentName){
		this.removeComponent(this.getComponentByName(componentName));
	}

	/**
	 * Called when this entity is clicked. Tries to call onClick on all its children.
	 */
	this.onClick = function(){
		var relativeX = chao.mouse.x - this.x;
		var relativeY = chao.mouse.y - this.y;
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].onClick){
				this.components[i].onClick(relativeX, relativeY);
			}
		}
	}

	/**
	 * Called when this entity is clicked and the pointer moves around. Tries to call onMove on all its children.
	 */
	this.onMove = function(){
		var relativeX = chao.mouse.x - this.x;
		var relativeY = chao.mouse.y - this.y;
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].onMove){
				this.components[i].onMove(relativeX, relativeY);
			}
		}
	}

	/**
	 * Called when the mouse/touch input on this entity is canceled. This happens when an entity is clickable and keepClickFocus is set to false. Tries to call onCancel on all its children.
	 */
	this.onCancel = function(){
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].onCancel){
				this.components[i].onCancel();
			}
		}
	}

	/**
	 * Called when the mouse/touch input on this entity is released. Tries to call onRelease on all its children.
	 */
	this.onRelease = function(){
		var relativeX = chao.mouse.x - this.x;
		var relativeY = chao.mouse.y - this.y;
		for(var i = 0; i < this.components.length; ++i){
			if(this.components[i].onRelease){
				this.components[i].onRelease(relativeX, relativeY);
			}
		}
	}

	/**
	 * Aligns this entity inside its parent. If no arguments are passed, it sets the centered position inside the parenting entity.
	 *
	 * @param alignX - Normalized x position inside the parent entity to align to. (where 0.0 means 0 pixels from the left and 1.0 the total width)
	 * @param alignY - Normalized y position inside the parent entity to align to. (where 0.0 means 0 pixels from the top and 1.0 the total height)
	 * @param anchorX - Place in this entity that will be snapped to the alignX point.
	 * @param anchorY - Place in this entity that will be snapped to the alignX point.
	 * @param pxOffsetX - Horizontal offset in pixels.
	 * @param pxOffsetY - Vertical offset in pixels.
	 */
	this.alignToParent = function(alignX, alignY, anchorX, anchorY, pxOffsetX, pxOffsetY) {
		this.alignToParentHorizontally(alignX, anchorX, pxOffsetX);
		this.alignToParentVertically(alignY, anchorY, pxOffsetY);
	}
	
	/**
	 * Aligns this entity inside its parent horizontally. If no arguments are passed, it sets the centered position inside the parenting entity.
	 *
	 * @param alignX - Normalized x position inside the parent entity to align to. (where 0.0 means 0 pixels from the left and 1.0 the total width)
	 * @param anchorX - Place in this entity that will be snapped to the alignX point.
	 * @param pxOffset - Horizontal offset in pixels.
	 */
	this.alignToParentHorizontally = function(alignX, anchorX, pxOffset) {
		alignX 	= alignX 	!= undefined ? alignX : 0.5;
		anchorX = anchorX 	!= undefined ? anchorX : 0.5;
		if (this.parent != null) {
			this.x = Math.ceil((this.parent.width * alignX) - (this.width * anchorX));
			this.x += pxOffset || 0;
		}
	}
	
	/**
	 * Aligns this entity inside its parent vertically. If no arguments are passed, it sets the centered position inside the parenting entity.
	 *
	 * @param alignY - Normalized y position inside the parent entity to align to. (where 0.0 means 0 pixels from the top and 1.0 the total height)
	 * @param anchorY - Place in this entity that will be snapped to the alignX point.
	 * @param pxOffset - Vertical offset in pixels.
	 */
	this.alignToParentVertically = function(alignY, anchorY, pxOffset) {
		alignY 	= alignY 	!= undefined ? alignY : 0.5;
		anchorY = anchorY 	!= undefined ? anchorY : 0.5;
		if (this.parent != null) {
			this.y = Math.ceil((this.parent.height * alignY) - (this.height * anchorY));
			this.y += pxOffset || 0;
		}
	}

	/**
	 * Calculates and returns the screen position of this entity.
	 *
	 * @return - Position on the screen, relative to its left edge.
	 */
	this.getScreenX = function(){
		return this.parent == null ? this.x : this.x + this.parent.getScreenX();
	}

	/**
	 * Calculates and returns the screen position of this entity.
	 *
	 * @return - Position on the screen, relative to its top edge.
	 */
	this.getScreenY = function(){
		return this.parent == null ? this.y : this.y + this.parent.getScreenY();
	}

	/**
	 * Checks if any child entity can be found at given coords and returns it.
	 *
	 * @param x - X position to look for the entity at.
	 * @param y - Y position to look for the entity at.
	 * @return - If this entity or one of its children were found at given coords, you will get them. If this is not the case, you get a null.
	 */
	this.getEntityAt = function(x, y){
		if(!this.visible || this.alpha <= 0){
			return null;
		}

		var childrenNum = this.children.length;
		for(var i = childrenNum - 1; i >= 0 ; --i){
			var child = this.children[i].getEntityAt(x, y);

			if(child !== null){
				return child;
			}
		}

		var screenX = this.getScreenX();
		var screenY = this.getScreenY();

		if(this.clickable 
			&& x >= screenX && x <= screenX + this.width*this.scaleX
			&& y >= screenY && y <= screenY + this.height*this.scaleY)
		{
			return this;
		}

		return null;
	}

	/**
	 * Checks collision agains another entity.
	 *
	 * @return - True if a collision against given entity occurs.
	 */
	this.checkCollision = function(entity){
		var thisX = this.getScreenX();
		var thisY = this.getScreenY();
		var otherX = entity.getScreenX();
		var otherY = entity.getScreenY();

		return otherX + entity.width*entity.scaleX > thisX
			&& otherY + entity.height*entity.scaleY > thisX
			&& thisX + this.width*this.scaleX > otherX
			&& thisX + this.height*this.scaleY > otherY;
	}
}

/**
 * Renders images, static and animated.
 *
 * @param key - Name/id of the image to be used
 */
function ComponentImage(key, frameWidth, frameHeight){
	this.name 				= "Image"; 				// Name by which this component is identified.
	this.entity 			= null; 				// The Entity this component is attached to.
	this.image 				= null;					// Image object this component will display.
	this.imageKey 			= key;					// Key by which we identify the image to use.

	this.frameWidth 		= frameWidth || 0;		// Animation frame width.
	this.frameHeight 		= frameHeight || 0;		// Animation frame height.

	this.flipX 				= false;				// Is this image flipped horizontally.
	this.flipY 				= false;				// Is this image flipped vertically.

	this.scrollFactorX		= 1.0; 					// How much the Entity will be affected by the camera crolling horizontally (e.g. 0=no movement and 1=same movement as the camera.
	this.scrollFactorY		= 1.0; 					// How much the Entity will be affected by the camera crolling vertically (e.g. 0=no movement and 1=same movement as the camera.

	this.anims 				= [];					// All the anims defined for this image.
	this.currentAnim		= -1;					// Currently played anim. -1 if none.
	this.currentFrame 		= 0;					// Current animation frame index.  (always counted from 0 to whatever the length of the current anim is)
	this.animTimer 			= 0;					// Used to know when to switch to the next animation frame.
	this.animPaused 		= false;				// Is the anim paused.
	this.animFinished		= false;				// Is the playback of the current anim finished.

	this.ready 				= true; 				// Syncs with the used image's "ready" state.

	/**
	 * A constructor of sorts. Called when the component is added to an entity.
	 */
	this.create = function(){
		this.setImage(this.imageKey, this.frameWidth, this.frameHeight);
	}

	/**
	 * Draw whatever this component is supposed to draw.
	 *
	 * @param x - X position of the parent of the entity this component is sticked to.
	 * @param y - Y position of the parent of the entity this component is sticked to.
	 * @param alpha - Opacity the parent of the entity this component is sticked to.
	 */
	this.draw = function(x, y, alpha){
		if(!this.image){
			return;
		}

		if(!this.entity.visible){
			return;
		}

		var anim = {key: "dummy", frames: [0], delay: 0, loop: true};
		if(this.currentAnim != -1){
			anim = this.anims[this.currentAnim];
		}

		var drawX 		= this.entity.x + x * this.scrollFactorX;
		var drawY 		= this.entity.y + y * this.scrollFactorY;
		var drawAlpha	= this.entity.alpha * alpha;
		var drawScaleX 	= this.flipX ? -this.entity.scaleX : this.entity.scaleX;
		var drawScaleY 	= this.flipY ? -this.entity.scaleY : this.entity.scaleY;
		if(drawAlpha > 1.0) drawAlpha = 1.0;

		var drawArea = {x:0, y:0, width:this.entity.width, height:this.entity.height};

		if(this.currentAnim != -1){
			var framesNumX = this.image.width / this.entity.width;
			var frameX = anim.frames[this.currentFrame];
			var frameY = Math.floor(frameX/framesNumX);
			frameX -= frameY * framesNumX;

			drawArea.x = Math.ceil(frameX * this.entity.width);
			drawArea.y = Math.ceil(frameY * this.entity.height);
		}

		chao.drawImagePart(chao.canvas, this.image, 
			drawX, drawY, drawArea, 
			this.entity.rotation, drawScaleX, drawScaleY, 
			drawAlpha);
	}

	/**
	 * Called every frame if the entity is visible in the hierarchy.
	 */
	this.update = function(){

		if(!this.ready && this.image.ready){
			this.ready = true;
			if(this.entity.width == -1){
				this.entity.width = this.image.width;
			}
			if(this.entity.height == -1){
				this.entity.height = this.image.height;
			}
		}

		if(this.currentAnim != -1 && !this.animPaused){
			var anim = this.anims[this.currentAnim];

			this.animTimer += chao.getTimeDelta();

			if(this.animTimer >= anim.delay){
				this.animTimer -= anim.delay;

				this.currentFrame ++;
				if(this.currentFrame >= anim.frames.length){
					if(anim.loop){
						this.currentFrame = 0;
					} else {
						this.currentFrame --;
						this.animPaused = true;
						this.animFinished = true;
					}
				}
			}
		}
	}

	/**
	 * Sets an image used by this component.
	 *
	 * @param image - Image object to be set.
	 * @param frameWidth - Width of a single animation frame if the used image is an anim sequence. Optional.
	 * @param frameHeight - Height of a single animation frame if the used image is an anim sequence. Optional.
	 */
	this.setImage = function(image, frameWidth, frameHeight){
		if(!image){
			return;
		}

		this.image = chao.getImage(image);

		this.entity.width 	= frameWidth || this.image.width;
		this.entity.height = frameHeight || this.image.height;

		if(!this.image.ready && (!frameWidth || !frameHeight)){
			this.ready = false;
		}
	}

	/**
	 * Defines a new animation sequence for this image.
	 *
	 * @param key - Name of the new animation.
	 * @param frames - Array of frames indexes to be played.
	 * @param delay - Delay between frames when playing the anim, in seconds.
	 * @param loop - True if this anim is supposed to play indefinitely.
	 */
	this.addAnim = function(key, frames, delay, loop){
		this.anims.push({
			key: 	key,
			frames:	frames,
			delay:	delay,
			loop:	loop,
		});
	}

	/**
	 * Start playback of the specified anim.
	 *
	 * @param key - Name of the animation to play.
	 * @param force - Whether to force the animation to restart.
	 */
	this.playAnim = function(key, force){
		for(var i = 0; i < this.anims.length; ++i){
			if(this.anims[i].key === key){
				if(this.currentAnim == i && !force){
					return;
				}

				this.currentAnim 	= i;
				this.currentFrame 	= 0;
				this.animTimer 		= 0;
				this.animPaused 	= false;
				this.animFinished	= false;
				return;
			}
		}

		chao.log("Entity \"" + this.entity.name + "\" has no anim named \"" + key + "\".");
	}

	/**
	 * Returns anim that is played. Null when none were defined.
	 *
	 * @return - Reference to the currently played anim.
	 */
	this.getCurrentAnim = function(){
		if(this.currentAnim >= 0 && this.currentAnim < this.anims.length){
			return this.anims[this.currentAnim];
		}

		return null;
	}

	/**
	 * Sets the paused state of the current anim playback.
	 * @param value - A new pause state.
	 */
	this.setAnimPause = function(value){
		this.animPaused = value;
	}
}

/**
 * Text rendering entity.
 * Use "\n" to break lines.
 * You can colorize parts of the text using color codes, eg. `2 is a color code for green and `` removes the effect of the last color code. 
 * Color codes are defined in colorCodes array in the chao namespace.
 * Example: "This is a text\nwith a `14yellow`` word inside." will create a two-line text object with the "yellow" word colored yellow.
 *
 * @param font - Font or id of the font used to display the text
 * @param text - Text to be displayed
 * @param size - Size of the text
 */
function ComponentText(font, text, size){
	this.name				= "Text";		// Name by which this component is identified.
	this.entity				= null;			// The Entity this component is attached to.
	
	this._text				= text;			// Text displayed by this component.
	this._font				= font;			// Font used to display the text.
	this._size				= size;			// Size of the text.
	this._color				= 0xff000000;	// Color of the text.
	this._backgroundColor	= undefined;	// Color of the text background.
	this._align 			= "left";		// Horizontal align of the text. "left", "roght" or "center".
	this._outlineColor		= 0xffffffff;	// Outline color.
	this._outlineSize 		= 0;			// Size of the outline.

	this.image 				= null;
	this.rawText 			= text;			// Text without the formatting whatnots.
	this.ready				= true;			// Syncs with the "ready" state of the loaded font.

	/**
	 * A constructor of sorts. Called when the component is added to an entity.
	 */
	this.create = function(){
		this._font = chao.getFont(this._font);
		if(chao.enableFontsLoadCheck){
			this.ready = this._font.ready;
		}
		this.image = chao.createImage(undefined, 1, 1);
		this.changeText();
	}

	/**
	 * Draw whatever this component is supposed to draw.
	 *
	 * @param x - X position of the parent of the entity this component is sticked to.
	 * @param y - Y position of the parent of the entity this component is sticked to.
	 * @param alpha - Opacity the parent of the entity this component is sticked to.
	 */
	this.draw = function(x, y, alpha){
		if(this.text === ""){
			return;
		}

		var drawX 		= this.entity.x + x;
		var drawY 		= this.entity.y + y;
		var drawAlpha	= this.entity.alpha * alpha;

		if(!this.ready && this.font.ready){
			this.ready = true;
			this.changeText(); // Font that was previously not ready was finally loaded, so we need to redraw this image.
		}

		switch(this.align){
			case "center": drawX -= this.entity.width / 2; break;
			case "right": drawX -= this.entity.width; break;
		}

		chao.drawImage(chao.canvas, this.image, 
			drawX, drawY, 
			drawAlpha, 
			this.entity.scaleX, this.entity.scaleY,
			this.entity.rotation);
	}

	/**
	 * Update the text displayed by this entity.
	 *
	 * @param text - New text to be set.
	 */
	this.changeText = function(text){
		text = text || this.text;

		var textLines		= text.split("\n");
		var chunks			= [];
		var rawLines		= [];
		var colorStack		= [];
		var currentColor	= this.color;

		this.rawText 		= "";

		for(var i = 0; i < textLines.length; ++i){

			var currentChunk	= "";
			chunks.push([]);

			for(var j = 0; j < textLines[i].length; ++j){
				var breakChunk 	= false;
				var newColor 	= undefined;
				if(textLines[i][j] == "`"){
					if(this.isNumber(textLines[i][j+1]) && this.isNumber(textLines[i][j+2])){
						colorStack.push(currentColor);
						newColor = chao.colorCodes[parseInt(textLines[i][j+1] + textLines[i][j+2], 10)];
						breakChunk = true;
						j += 2;
					} else if(this.isNumber(textLines[i][j+1])){
						colorStack.push(currentColor);
						newColor = chao.colorCodes[parseInt(textLines[i][j+1], 10)];
						breakChunk = true;
						j += 1;
					} else if(textLines[i][j+1] == "`"){
						if(colorStack.length > 0){
							newColor = colorStack.pop();
						}
						breakChunk = true;
						j += 1;
					}
				}

				if(breakChunk){
					if(currentChunk.length > 0){
						chunks[i].push({text:currentChunk, color:currentColor});
					}
					currentChunk = "";
					currentColor = newColor || currentColor;
				} else {
					currentChunk += textLines[i][j];
				}
			}

			if(currentChunk.length > 0){
				chunks[i].push({text:currentChunk, color:currentColor});
			}

			var currentRawLine = "";
			for(var j = 0; j < chunks[i].length; ++j){
				this.rawText += chunks[i][j].text;
				currentRawLine += chunks[i][j].text;
			}
			if(chunks.length > i-1) this.rawText += "\n";

			rawLines.push(currentRawLine);
		}

		this.image.context.font = this.size.toFixed() + "px " + this.font.name;
		chao.drawText(this.image, this.font, "M", 0, 0, this.size);

		var linesSizes		= [];
		var longestLineSize = 0;
		for(var i = 0; i < rawLines.length; ++i){
			linesSizes.push(chao.getTextSize(this.image, rawLines[i]).width);
			if(linesSizes[i] > longestLineSize){
				longestLineSize = linesSizes[i];
			}
		}
		this.image.canvas.width = this.image.width = this.entity.width = longestLineSize;
		this.image.canvas.height = this.image.height = this.entity.height = this.size * textLines.length;

		if(this.backgroundColor){
			chao.clearToColor(this.image, this.backgroundColor);
		} else {
			chao.clearImage(this.image);
		}

		for(var i = 0; i < chunks.length; ++i){
			var currentX = 0;
			switch(this.align){
				case "center": currentX = (longestLineSize - linesSizes[i]) / 2; break;
				case "right": currentX = longestLineSize - linesSizes[i];
			}
			for(var j = 0; j < chunks[i].length; ++j){
				chao.drawText(this.image, 
					this.font,
					chunks[i][j].text,
					currentX, -this.size*.25 + (i * this.size),
					this.size, 
					chunks[i][j].color, 
					"left", 
					this.outlineColor, 
					this.outlineSize);

				currentX += chao.getTextSize(this.image, chunks[i][j].text).width;;
			}
		}
	}

	/**
	 * Checks if give sign is a numeral.
	 *
	 * @param c - A character to check.
	 * @return - True if the given character is a numeral.
	 */
	this.isNumber = function(c){
		return "0123456789".indexOf(c) !== -1;
	}

}
ComponentText.prototype = {
	// This is a serious mess, even by the messiness of all the other things here. Is there a better way of doing this?
	get text() { return this._text; },
	set text(newText){ this._text = newText; this.changeText(); },
	get font() { return this._font; },
	set font(newFont){ this._font = newFont; this.changeText(); },
	get size() { return this._size; },
	set size(newSize){ this._size = newSize; this.changeText(); },
	get color() { return this._color; },
	set color(newcolor){ this._color = newcolor; this.changeText(); },
	get backgroundColor() { return this._backgroundColor; },
	set backgroundColor(newBackgroundColor){ this._backgroundColor = newBackgroundColor; this.changeText(); },
	get align() { return this._align; },
	set align(newAlign){ this._align = newAlign; this.changeText(); },
	get outlineColor() { return this._outlineColor; },
	set outlineColor(newOutlineColor){ this._outlineColor = newOutlineColor; this.changeText(); },
	get outlineSize() { return this._outlineSize; },
	set outlineSize(newOutlineSize){ this._outlineSize = newOutlineSize; this.changeoutlineSize(); },
};


/**
 * Simple, clickable button.
 *
 * @param image - Name/id of the image to be used.
 */
function ComponentButton(image){
	this.name 			= "Button";		// Name by which this component is identified.
	this.entity 		= null;			// The Entity this component is attached to.
	this.imageKey 		= image;		// Name of the image used as this button's graphic.
	this.image 			= null;			// Image used as this button's graphic.
	this.imagePressed 	= null;			// Image used as this button's pressed state graphic.
	this.text 			= null;			// Text displayed on top this button.

	this.disableDim		= false;		// When false and imagePressed is not set, the button will react to a press by dimming itself.
	this.disabled 		= false;		// When true, the button will ignore all input.
	this.dimInactive 	= true;			// When true, the button will be dimmed when disabled.

	this.onPress 		= function(button) {};	// Function called when this button is pressed.
	this.onHold 		= function(button) {};	// Function called when this button stays pressed.
	this.onReleased 	= function(button) {};	// Function called when this button stops being pressed.

	this.pressed 		= false;

	/**
	 * A constructor of sorts. Called when the component is added to an entity.
	 */
	this.create = function(){
		this.setImage(this.imageKey);
		this.entity.clickable = true;
	}

	/**
	 * Called every frame if the entity is visible in the hierarchy.
	 */
	this.update = function(){
		var screenX = this.entity.getScreenX();
		var screenY = this.entity.getScreenY();

		if(!this.entity.visible){
			return;
		}

		var mouseOver 	= this.isAbove(chao.mouse.x, chao.mouse.y);
		var buttonAlpha = 1.0; 

		if(mouseOver && !this.disabled){
			if(chao.mouse.justReleased){
				this.pressed = false;
				buttonAlpha = 1.0;
				if(this.onReleased){
					this.onReleased(this);
				}
			}

			if(chao.mouse.pressed){
				buttonAlpha = 0.5;

				if(!this.pressed && this.onPress){
					this.onPress(this);
				}
				if(this.onHold){
					this.onHold(this);
				}

				this.pressed = true;

			} else if(this.pressed){
				buttonAlpha = 1.0;
			}

		} else {
			buttonAlpha = (!this.disabled || !this.dimInactive) ? 1.0 : 0.5;
			this.pressed = false;
		}

		if(this.imagePressed){
			this.image.entity.visible 			= buttonAlpha > 0.5;
			this.imagePressed.entity.visible 	= buttonAlpha <= 0.5;
		} else if(!this.disableDim){
			this.image.entity.alpha = buttonAlpha;
			if(this.text){
				this.text.entity.alpha = buttonAlpha;
			}
		}
	}

	/**
	 * Sets an image used by this component.
	 *
	 * @param key - String id of an image or an image object to be used.
	 */
	this.setImage = function(key){
		if(this.image){
			this.entity.remove(this.image.entity);
		}

		this.imageKey = key;

		this.image = (new Entity("Button Image")).addComponent(new ComponentImage(this.imageKey));
		this.entity.add(this.image.entity);

		this.updateSize();
	}

	/**
	 * Sets an image that will be displayed when the button is pressed.
	 *
	 * @param key - String id of an image or an image object to be used.
	 */
	this.setImagePressed = function(key){
		if(this.imagePressed){
			this.entity.remove(this.imagePressed.entity);
		}

		this.imagePressed = (new Entity("Button Image Pressed")).addComponent(new ComponentImage(key));
		this.entity.add(this.imagePressed.entity);
		this.imagePressed.entity.visible = false;
	}

	/**
	 * Sets a text that will be displayed on top of the button object.
	 *
	 * @param text - Text to be displayed.
	 * @param font - Font to use for the text.
	 * @param size - Size of the text.
	 */ 
	this.setText = function(text, font, size){
		if(!this.text){
			this.text = (new Entity("Button Text", 0, 0)).addComponent(new ComponentText(font, text, size));
			this.text.align = "left";
			this.entity.add(this.text.entity);
		} else {
			if(font) this.text.font = font;
			if(text) this.text.text = text;
			if(size) this.text.size = size;
		}

		this.text.entity.alignToParent();

		return this.text;
	}

	/**
	 * Updates size of the entity this component is sticked to based on the image used as as this button image.
	 */
	this.updateSize = function(){
		this.entity.width = this.image.entity.width;
		this.entity.height = this.image.entity.height;
	}

	/**
	 * Checks if the pointer is above the entity.
	 *
	 * @param x - X position of the pointer.
	 * @param y - Y position of the pointer.
	 * @return - boolean value describing the outcome of the performed check.
	 */
	this.isAbove = function(x, y){
		if(!this.entity.visible){
			return false;
		}

		return this.entity.getEntityAt(x, y) === this.entity;
	}

}

/**
 * A camera component.
 */
function ComponentCamera(){
	this.name 					= "Camera";							// Name by which this component is identified.
	this.entity 				= null;								// The Entity this component is attached to.

	this.trackedEntity			= null;								// Entity followed by the camera.
	this.trackPositionBuffer	= [];								// Position buffer for smoothing the camera movement.
	this.trackSmoothness		= 1;								// The more, the smoother the camera movement is.

	this.offsetX				= 0;								// Horizontal position offset from the followed entity.
	this.offsetY				= 0;								// Vertical position offset from the followed entity.
	this.deadzone				= {x:0, y:0, width:0, height:0};	// Camera will start following the target only when it's bumping agains this deadzone bounds.
	this.previousPos			= {x:0, y:0};						// Remembers last camera pos, for easier calculations.
	this.bounds 				= {x:0, y:0, width:-1, height:-1};	// Size of the world. The camera will not be supposed to show anything outsize these bounds.

	this.slideTweens			= [];								// Array containing all tweens used for the camera slide effects.

	/**
	 * Called every frame if the entity is visible in the hierarchy.
	 */
	this.update = function(){
		if(this.trackedEntity == null){
			this.addPositionToBuffer(this.entity.x, this.entity.y);
			return;
		}

		// figuring out some basic stuff
		var targetPos = {
			x: this.trackedEntity.x + this.trackedEntity.width/2,
			y: this.trackedEntity.y + this.trackedEntity.height/2
		}

		var relativePos = {
			x: targetPos.x + this.entity.x,
			y: targetPos.y + this.entity.y
		}

		var cameraPos = {
			x: (-targetPos.x + (chao.screenWidth/2)) - this.offsetX,
			y: (-targetPos.y + (chao.screenHeight/2)) - this.offsetY
		}

		// contrived deadzone calculations
		if(this.deadzone.width > 0 && this.deadzone.height > 0){
			if(targetPos.x > (this.deadzone.x-this.entity.x) + this.deadzone.width/2){
				cameraPos.x += Math.abs(((this.deadzone.x-cameraPos.x)+this.deadzone.width)-targetPos.x);
			}else {
				cameraPos.x -= Math.abs((this.deadzone.x-cameraPos.x)-targetPos.x);
			}
			if(targetPos.y > (this.deadzone.y-this.entity.y) + this.deadzone.height/2){
				cameraPos.y += Math.abs(((this.deadzone.y-cameraPos.y)+this.deadzone.height)-targetPos.y);
			}else {
				cameraPos.y -= Math.abs((this.deadzone.y-cameraPos.y)-targetPos.y);
			}

			var deadZonedX = relativePos.x > this.deadzone.x && relativePos.x < this.deadzone.x+this.deadzone.width;
			var deadZonedY = relativePos.y > this.deadzone.y && relativePos.y < this.deadzone.y+this.deadzone.height;
			if(deadZonedX){
				cameraPos.x = this.previousPos.x;
			} else {
				this.previousPos.x = cameraPos.x;
			}
			if(deadZonedY){
				cameraPos.y = this.previousPos.y;
			} else {
				this.previousPos.y = cameraPos.y;
			}
		}

		// clamping camera position to the set bounds
		if(this.bounds.width > 0){
			cameraPos.x = -chao.clamp(-cameraPos.x, this.bounds.x, (this.bounds.x+this.bounds.width)-chao.screenWidth);
		}
		if(this.bounds.height > 0){
			cameraPos.y = -chao.clamp(-cameraPos.y, this.bounds.y, (this.bounds.y+this.bounds.height)-chao.screenHeight);
		}

		// smoothing the camera movement
		this.addPositionToBuffer(cameraPos.x, cameraPos.y);
		
		var smoothedPos = { x:0, y:0 };
		for(var i = 0; i < this.trackPositionBuffer.length; ++i){
			smoothedPos.x += this.trackPositionBuffer[i].x;
			smoothedPos.y += this.trackPositionBuffer[i].y;
		}
		if(this.trackPositionBuffer.length > 0){
			smoothedPos.x /= this.trackPositionBuffer.length;
			smoothedPos.y /= this.trackPositionBuffer.length;
		}
		
		// and after a hard day's work, finally setting the camera position!
		this.entity.x = smoothedPos.x;
		this.entity.y = smoothedPos.y;

	}

	/**
	 * Starts following an antity.
	 *
	 * @param entity - An entity to be followed.
	 * @param smoothness - Integer, the more the smoother the camera movement will be.
	 */
	this.follow = function(entity, smoothness){
		this.trackedEntity		= entity;
		this.trackSmoothness 	= smoothness || this.trackSmoothness;
		if(this.trackSmoothness <= 0){
			this.trackSmoothness = 1;
		}
	}

	/**
	 * Unfollows entity if any was indeed followed.
	 */
	this.unfollow = function(){
		this.trackedEntity = null;
	}

	/**
	 * Stops tracking whatever was followed and slides to the center of given position.
	 *
	 * @param x - X position of a place we wish to slide to.
	 * @param y - Y position of a place we wish to slide to.
	 * @param time - Seconds the slide will take.
	 * @param interpolationType - Type of interpolation to apply. (chao.INTERPOLATE_*)
	 * @param callback - A function to call when the slide is finished.
	 */
	this.slideToPosition = function(x, y, time, interpolationType, callback){
		interpolationType = interpolationType != undefined ? interpolationType : chao.INTERPOLATE_SMOOTH;

		for(var i = 0; i < this.slideTweens.length; ++i){
			chao.removeTween(this.slideTweens[i]);
		}
		this.slideTweens = [];
		this.unfollow();

		x = -x + chao.screenWidth/2;
		y = -y + chao.screenHeight/2;

		this.slideTweens.push(chao.addTween(this.entity, "x", this.entity.x, x, time, interpolationType, chao.REPEAT_MODE_ONCE, 0, callback));
		this.slideTweens.push(chao.addTween(this.entity, "y", this.entity.y, y, time, interpolationType, chao.REPEAT_MODE_ONCE));
	}

	/**
	 * Adds a new position to the position buffer for some internal calculations.
	 *
	 * @param x - X position to add to buffer.
	 * @param y - Y position to add to buffer.
	 */
	this.addPositionToBuffer = function(x, y){
		this.trackPositionBuffer.push( {x:x, y:y} );
		while(this.trackPositionBuffer.length > this.trackSmoothness){
			this.trackPositionBuffer.splice(0, 1);
		}
	}
}

/**
 * A single, basic particle.
 *
 * @param image - Name/id of the image to be used. Will try to use existing image component when ommited.
 */
function ComponentParticle(image){
	this.name 				= "Particle";	// Name by which this component is identified.
	this.entity 			= null;			// The Entity this component is attached to.

	this.image				= null;
	this.imageKey			= image;

	this.lifetime			= 1.0;
	this.fadeOutMode		= ComponentParticle.FADE_MODE_NONE;
	this.velocity			= {x:0, y:0};
	this.acceleration		= {x:0, y:0};
	this.rotationVel		= 0;
	this.rotationAcc		= 0;

	this.useUnscaledTime	= false;

	this.timer 				= 0;

	/**
	 * A constructor of sorts. Called when the component is added to an entity.
	 */
	this.create = function(){
		if(!this.imageKey){
			this.image = this.entity.getComponentByName("Image");
		} else {
			this.image = this.entity.addComponent(new ComponentImage(this.imageKey));
		}

		if(!this.image){
			chao.log("ComponentParticle needs an existing ComponentImage or image key passed in constructor.");
		}
	}

	/**
	 * Called every frame if the entity is visible in the hierarchy.
	 */
	this.update = function(){
		if(!this.image){
			return;
		}

		var delta = this.useUnscaledTime ? chao.getUnscaledDelta() : chao.getTimeDelta();

		this.timer += delta;

		if(this.timer >= this.lifetime){
			chao.destroyEntity(this.entity);
			return;
		}

		switch(this.fadeOutMode){
			case ComponentParticle.FADE_MODE_NONE: {
				this.entity.alpha = 1.0;
				break;
			}
			case ComponentParticle.FADE_MODE_LINEAR: {
				this.entity.alpha = 1.0 - (this.timer / this.lifetime);
				break;
			}
			case ComponentParticle.FADE_MODE_LAST_SECOND: {
				var v = this.lifetime - this.timer;
				this.entity.alpha = v > 1.0 ? 1.0 : v;
				break;
			}
		}

		this.velocity.x 	+= this.acceleration.x * delta;
		this.velocity.y 	+= this.acceleration.y * delta;
		this.entity.x		+= this.velocity.x * delta;
		this.entity.y		+= this.velocity.y * delta;

		this.rotationVel	+= this.rotationAcc * delta;
		this.entity.rotation+= this.rotationVel * delta;

	}
}
ComponentParticle.FADE_MODE_NONE		= 0; // No fade at all shall be applied.
ComponentParticle.FADE_MODE_LINEAR		= 1; // The particle will progressively.
ComponentParticle.FADE_MODE_LAST_SECOND	= 2; // The particle will fade in span of the last second of its life.

/**
 * Used to shake entities around.
 *
 * @param force - Max shake range in pixels.
 * @param time - How long the the shaking will last, in seconds.
 * @param damped - Is the shake supposed to be damped over time.
 */
function ComponentShake(force, time, damped){
	this.name 					= "Shake";							// Name by which this component is identified.
	this.entity 				= null;								// The Entity this component is attached to.

	this.damped					= damped;							// Is the shake supposed to be damped over time.
	this.force					= force || 0.0;						// Max shake range in pixels.
	this.duration				= time || 0.0;						// How long the the shaking will last, in seconds.
	this.useUnscaledTime		= false;							// If set, the unscaled time delta will be used.

	this.disposable				= false;							// If true, this component will destroy itself after performing a single shake.

	this.entityPosition 		= {x:0, y:0};						// Internal for remembering the original position of the entity.
	this.shakenPosition 		= {x:0, y:0};						// Watches out for position changes that was not performed by this component.
	this.timer					= 0.0;								// Internal timer.

	/**
	 * Called every frame if the entity is visible in the hierarchy.
	 */
	this.update = function(){
		if(this.timer > 0.0){
			this.timer -= this.useUnscaledTime ? chao.getUnscaledDelta() : chao.getTimeDelta();
			if(this.timer <= 0.0){
				this.timer = 0.0;
				this.entity.x = this.entityPosition.x;
				this.entity.y = this.entityPosition.y;

				if(this.disposable){
					this.entity.removeComponent(this);
				}

				return;
			}

			// Put this component at the end if it's not. So we can take into account the position changes made by other components.
			var id = this.entity.components.indexOf(this);
			if(id != this.entity.components.length-1){
				this.entity.components.splice(id, 1);
				this.entity.components.push(this);
			}

			var range = this.force;
			if(this.damped){
				range *= this.timer / this.duration;
			}

			// check for entity movement since the last frame.
			if(this.shakenPosition.x != this.entity.x || this.shakenPosition.y != this.entity.y){
				this.entityPosition = {x:this.entity.x, y: this.entity.y};
			}

			var xChange = chao.getRandom(range * 2) - range;
			var yChange = chao.getRandom(range * 2) - range;

			this.entity.x = this.shakenPosition.x = this.entityPosition.x + xChange;
			this.entity.y = this.shakenPosition.y = this.entityPosition.y + yChange;
		}		
	}

	/**
	 * Start the shake! You don't have to pass the arguments every time.
	 */
	this.shake = function(force, time, damped){
		if(force) this.force = force;
		if(time) this.duration = time;
		if(damped) this.damped = damped;

		if(this.timer <= 0.0){
			this.entityPosition = {x:this.entity.x, y: this.entity.y};
		}

		this.timer = this.duration;
	}
}
