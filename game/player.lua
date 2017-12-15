class = require('lua_lib/class')

Player = class(function(self, character)
		log('Loading player '..character)
		self.fileName = character;

		self:load(character);
		-- print(var_dump(self));

		self.model = ModelObj.new();
		self.model:init('game/models/'..self.fileName..'.obj');
		scene.addModel(self.model);

		self:update();
	end)

function Player:load(path)
	local c = require('game/characters/'..path)
	for k,v in pairs(c) do
		self[k] = v;
	end
end

function Player:randomPlayer()
	return Player('thief');
end

function Player:getCurrentField()
	return board:getField(self.field);
end

function Player:getPossibleDirections()
	local f = board:getField(self.field);
	-- TODO
	return {'left', 'right', 'up'};
end

function Player:update()
	local field = self:getCurrentField();

	local mat = Matrix.new()
	mat:translate(Glm_Vec3.new(field.pos_x, field.pos_y, field.pos_z));
	mat:scale(0.05); -- TODO Change in the model

	self.model:setPosition(mat:val());
end

return Player
