Board = require('game/board')
GameState = require('game/state')
Player = require('game/player')
Journal = require('game/journal')
Action = require('game/action')
GuiHelpers = require('game/gui_helpers')
Hud = require('game/hud')
var_dump = require('lua_lib/var_dump')


function setup()
	math.randomseed(os.time());

	scene:getSkybox():init("game/map/skybox.png");

	local m = scene:getMap();
	m:setRect(0, 0, 2, 2, -1, 0);
	m:init("game/map/map.png", "game/map/grass.png");

	journal = Journal();
	board = Board();
	gameState = GameState();
	player = Player.randomPlayer();
	hud = Hud();

	-- Temporarily
	setLoopResolution(100);

	GuiHelpers:randomButton('My turn', function()
			gameState.myTurn=true;
		end);

	log('Lua setup done');
end

function resizeWindow()
	hud:update();
end

function loop()
	gameState:process();
	hud:update();
end

