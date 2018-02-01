Dice = class(function(self)
		self.result = nil;

	end)

function Dice:cast()
	if(self.force) then
		self.result = self.force;
		return self.force;
	end

	self.result = math.ceil(math.random()*6);
	journal:add('Casted: '..self.result);
	return self.result;
end

return Dice;

