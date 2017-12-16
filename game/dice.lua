class = require('lua_lib/class')

Dice = class(function(self)
		self.result = nil;
	end)

function Dice:cast()
	self.result = math.ceil(math.random()*6);
	journal:add('Casted: '..self.result);
	return self.result;
end

return Dice;

