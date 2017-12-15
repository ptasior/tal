Board = require('game/board')
GameState = require('game/state')
Player = require('game/player')
var_dump = require('lua_lib/var_dump')


function setup()
	scene:getSkybox():init("game/map/skybox.png");

	local m = scene:getMap();
	m:setRect(0, 0, 2, 2, -1, 0);
	m:init("game/map/map.png", "game/map/grass.png");

	board = Board();

	gameState = GameState();

	player = Player.randomPlayer();

	-- Temporarily
	setLoopResolution(100);

	log('Lua setup done');
end

function loop()
	gameState:process();
end

