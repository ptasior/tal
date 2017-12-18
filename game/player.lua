Player = class(function(self, character)
		log('Loading player '..character)
		self.fileName = character;

		self:load(character);
		-- print(var_dump(self));

		self.model = ModelObj.new();
		self.model:init('game/models/'..self.fileName..'.obj');
		scene.addModel(self.model);

		self.initialCraft = self.craft;
		self.lives = 4;

		self.defeatedEnemies = {};
		self.spells = {};
		self.objects = {};
		self.followers = {};

		self:update();
	end)

function Player:load(path)
	local c = require('game/characters/'..path)
	for k,v in pairs(c) do
		self[k] = v;
	end
end

randomPlayerCnt = 0; -- TODO Global, fix me

function Player:randomPlayer()
	randomPlayerCnt = randomPlayerCnt + 1;
	local list = require('game/characters/list')
	return Player(list[randomPlayerCnt]);
end

function Player:getCurrentField()
	return board:getField(self.field);
end

function Player:getPossibleDirections()
	local f = board:getField(self.field);

	if(self.direction) then
		return without(keys(f.directions),
						Board.opositeDirection(self.direction));
	else
		return keys(f.directions);
	end
end

function Player:update()
	local field = self:getCurrentField();

	local mat = Matrix.new()
	mat:translate(Glm_Vec3.new(field.pos_x, field.pos_y, field.pos_z));
	mat:scale(0.07); -- TODO Change in the model

	self.model:setPosition(mat:val());
end

return Player

