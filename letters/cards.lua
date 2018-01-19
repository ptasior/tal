Cards = class(function(self, var)
		self.deck = {};
		self.shvar = var;
	end)


function Cards:set(arr, names)
	self.names = names;
	self.deck = arr;
	self:shuffle();
end


function Cards:shuffle()
	local p;
	for i = #self.deck, 2, -1 do
		p = math.random(i);
		self.deck[i], self.deck[p] = self.deck[p], self.deck[i];
	end
end


function toName(no)
	return self.names[tonumber(no)];
end


function toNumber(name)
	for i = 0,#names do
		if(self.names[i] == name) then
			return i;
		end
	end
	return -1;
end


function drawOne()
	self:read();
	return table.remove(self.deck,1);
end


function save()
	self.shvar:set(join('-', self.deck));
end


function read()
	local s = self.shvar:get()
	self.deck = split('-', s);
end


return Cards

