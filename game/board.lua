class = require('lua_lib/class')

Field = class(function(self, name)
		self:loadField(name);
		self.pos_y = scene:getMap():getAltitude(self.pos_x, self.pos_z);
	end)

function Field:loadField(name)
	local c = require('game/board/'..name);
	for k,v in pairs(c) do
		self[k] = v;
	end
end

function Field:onPass()
	log('passing '..self.name);
end

function Field:onLand()
	log('landing '..self.name);
	journal:add('You are in: '..self.name)
end

-------------------------------------------------------------------------------

Board = class(function(self)
		self:loadFields();
		self:drawFields();

		self.action = nil; -- Action aplied to all players
	end)

function Board:loadFields()
	self.fields = {};
	local fields = require('game/board/list');

	for i,field in ipairs(fields) do
		local f = Field(field);
		self.fields[f.name] = f;
	end
end

function Board:drawFields()
	for k,v in pairs(self.fields) do
		self:drawField(v);
	end
end

function Board:drawField(f)
	local pos = Glm_Vec3.new(f.pos_x, f.pos_y+0.0001, f.pos_z)
	local posLab = Glm_Vec3.new(f.pos_x, f.pos_y+0.01, f.pos_z)

	local mat = Matrix.new();
	mat:translate(pos);
	mat:scale(0.05);
	mat:rotate(Glm_Vec3.new(-90, 0, 0));

	local s = Sprite.new();
	s:init("game/gfx/field.png", "triangle");
	s:setPosition(mat:val());
	scene.addSprite(s);

	local mat = Matrix.new();
	mat:translate(pos);
	mat:translate(Glm_Vec3.new(0, 0.05, 0));
	mat:scale(0.05);

	local s = Sprite.new();
	s:init("text-"..f.name, "triangle");
	s:setPosition(mat:val());
	scene.addSprite(s);
end

function Board:getField(name)
	if(self.fields[name] == nil) then
		log('No key in fields list');
		print(name)
		return nil;
	end
	return self.fields[name];
end

return Board

