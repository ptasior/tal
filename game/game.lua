class = require('lua_lib/class')
var_dump = require('lua_lib/var_dump')
require('lua_lib/functions')

Board = require('game/board')
GameState = require('game/state')
Player = require('game/player')
OtherPlayers = require('game/other_players')
Journal = require('game/journal')
Action = require('game/action')
GuiHelpers = require('game/gui_helpers')
Hud = require('game/hud')
Dice = require('game/dice')


function setup()
	math.randomseed(os.time());

	scene:getSkybox():init("game/map/skybox.png");

	local m = scene:getMap();
	m:setRect(0, 0, 2, 2, -0.5, 0);
	m:init("game/map/map.png");
	m:addTexture("game/map/grass.png");
	m:addTexture("game/map/rocks.png");
	m:addTexture("game/map/paths.png");

	journal = Journal();
	board = Board();
	gameState = GameState();
	player = Player.randomPlayer();
	other_players = OtherPlayers();
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

