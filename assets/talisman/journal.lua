Journal = class(function(self)
		self.entries = {};
	end)

function Journal:add(line)
	self.entries[#self.entries+1] = line;
end

return Journal

