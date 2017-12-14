class = require('lua_lib/class')

Board = class(function(self, path)
		local f = require('game/board/'..path)
		print(var_dump(f))
		self:_apply(f)
		self.path = path
	end)

function Board:getPath()
	return self.name
end

function Board:_apply(field)
	for k,v in pairs(field) do
		self[k] = v
	end
end

function Board.loadFrom(path)
	local tiles = require(path)

	local board = {}
	for i,field in ipairs(tiles) do
		board[#board+1] = Board(field)
	end

	return board
end

return Board

