 
class = require('lua_lib/class')

Player = class(function(self)
		self.character = 'Barbarian';
		self.alignment = 'neutral';
		self.location = 'forest';
	end)

function Board:getPath()
	return self.name;
end
