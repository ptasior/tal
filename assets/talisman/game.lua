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
Battle = require('game/battle')



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
	battle = Battle();

	-- Temporarily
	setLoopResolution(500);

	gg_btn = GuiHelpers:randomButton('My turn', function()
			gameState.myTurn=true;
		end);

	tt_btn = GuiHelpers:randomButton('Net', function()
			sharedData.at('test'):at("rq"):set('qq');
		end);

	pp_btn = GuiHelpers:randomButton('Print', function()
			sharedData.print();
		end);

	mat = Matrix.new();
	-- mat:translate(Glm_Vec3.new(0.0, 0.5, -2))
	mat:scaleVec(Glm_Vec3.new(0.5, 0.5, 0.5));


	ss = Scroll.new();
	ss:setRect(100,100,110,104)
	gui.rootWidget():addScroll(ss);
	aa = Label.new("aaaaa")
	ss:addLabel(aa)
	aa = Label.new("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb")
	ss:addLabel(aa)
	aa = Label.new("ccccc")
	ss:addLabel(aa)
	aa = Label.new("ddddd")
	ss:addLabel(aa)
	aa = Label.new("eeeee")
	ss:addLabel(aa)
	aa = Label.new("fffff")
	ss:addLabel(aa)
	aa = Label.new("ggggg")
	ss:addLabel(aa)


	ss = Checkbox.new();
	ss:setRect(300,100,20,20)
	gui.rootWidget():addCheckbox(ss);

	log('Lua setup done');
end

function resizeWindow()
	hud:update();
end

function loop()
	gameState:process();
	hud:update();
end

