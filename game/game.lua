-- require'lfs'
Board = require('game/board')
var_dump = require('lua_lib/var_dump')
class = require('lua_lib/class')


function setup()
	scene:getSkybox():init("game/map/skybox.png")

	local m = scene:getMap()
	m:setRect(0, 0, 2, 2, -1, 0)
	m:init("game/map/map.png", "game/map/grass.png")
end

local function drawField(x, y)
	local mat = Matrix.new()
	local alt = scene:getMap():getAltitude(x,y)
	log(alt)
	mat:translate(Glm_Vec3.new(x, alt+0.0001, y))
	mat:scale(0.05)
	mat:rotate(Glm_Vec3.new(90, 0, 0))

	local s = Sprite.new()
	s:init("game/gfx/field.png", "triangle")
	s:setPosition(mat:val());
	scene.addSprite(s)
end

local function loadMapTiles()
	local board = Board.loadFrom('game/board/list')

	for i,b in ipairs(board) do
		drawField(b.pos_x, b.pos_y)
	end
end

function start()
	loadMapTiles()
end

