Board = require('game/board')
GameState = require('game/state')
var_dump = require('lua_lib/var_dump')


local function drawField(x, y)
	local mat = Matrix.new()
	local alt = scene:getMap():getAltitude(x,y)
	mat:translate(Glm_Vec3.new(x, alt+0.0001, y))
	mat:scale(0.05)
	mat:rotate(Glm_Vec3.new(90, 0, 0))

	local s = Sprite.new()
	s:init("game/gfx/field.png", "triangle")
	s:setPosition(mat:val())
	scene.addSprite(s)
end

local function loadMapTiles()
	board = Board.loadFrom('game/board/list')

	for i,b in ipairs(board) do
		drawField(b.pos_x, b.pos_y)
	end
end

function setup()
	scene:getSkybox():init("game/map/skybox.png")

	local m = scene:getMap()
	m:setRect(0, 0, 2, 2, -1, 0)
	m:init("game/map/map.png", "game/map/grass.png")

	loadMapTiles();
	gameState = GameState();

	-- Temporarily
	setLoopResolution(100)

	log('Lua setup done')
end

function loop()
	gameState:process();
end

