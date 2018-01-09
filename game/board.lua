Field = class(function(self, name)
		self.onPassFnc = nil;
		self.onLandFnc = nil;
		self.description = "No description";

		self:loadField(name);
		self.name = name
		self.pos_x = (self.pos_x/252)*2
		self.pos_z = (self.pos_z/252)*2
		self.pos_y = scene:getMap():getAltitude(self.pos_x, self.pos_z);

		self.objects = {};
		self.followers = {};

		self:draw();
	end)

function Field:loadField(name)
	local c = require('game/board/'..name);
	for k,v in pairs(c) do
		self[k] = v;
	end
end

function Field:draw()
	self.obj = {}
	local pos = Glm_Vec3.new(self.pos_x, self.pos_y+0.0001, self.pos_z)

	local mat = Matrix.new();
	mat:translate(pos);
	mat:scale(0.05);
	mat:rotate(Glm_Vec3.new(-90, 0, 0));

	self.obj.field = Sprite.new();
	self.obj.field:init("game/gfx/field.png", "triangle");
	self.obj.field:setPosition(mat:val());
	scene.addSprite(self.obj.field);

	local mat = Matrix.new();
	mat:translate(pos);
	mat:translate(Glm_Vec3.new(0, 0.05, 0));

	self.obj.label = Sprite.new();
	self.obj.label:setSize(0.007*string.len(self.label), 0.007);
	self.obj.label:init("text-"..self.label, "sprite");
	self.obj.label:setPosition(mat:val());
	scene.addSprite(self.obj.label);
end

function Field:getPlayers()
	ret = {};
	for i,v in ipairs(other_players.players) do
		if(v.field == self.name) then
			ret[#ret+1] = v;
		end
	end
	return ret;
end


function Field:onPass()
	if(self.onPassFnc) then
		self.onPassFnc();
	else
		log('passing '..self.label);
	end
end

function Field:onLand()
	journal:add('You are in: '..self.label)

	if(self.onLandFnc) then
		self.onLandFnc();
	end
end

-------------------------------------------------------------------------------

Board = class(function(self)
		self:loadFields();

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

function Board:getField(name)
	if(self.fields[name] == nil) then
		log('No key in fields list');
		print(name)
		return nil;
	end
	return self.fields[name];
end

function Board:opositeDirection(dir)
	if(dir == 'cw') then return 'ccw';end
	if(dir == 'ccw') then return 'cw';end
	if(dir == 'up') then return 'down';end
	if(dir == 'down') then return 'up';end
end

return Board

