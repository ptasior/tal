Cards = class(function(self, var)
		self.deck = {};
		self.shvar = var;
	end)


function Cards:set(arr, names)
	self.names = names;
	self.deck = arr;
end


function Cards:shuffle()
	local p;
	for i = #self.deck, 2, -1 do
		p = math.random(i);
		self.deck[i], self.deck[p] = self.deck[p], self.deck[i];
	end
end


function Cards:toName(no)
	return self.names[tonumber(no)];
end


function Cards:toNumber(name)
	for i = 1,#self.names do
		if(self.names[i] == name) then
			return i;
		end
	end
	return -1;
end


function Cards:drawOne()
	return table.remove(self.deck,1);
end


function Cards:save()
	log('saving'..join('-', self.deck))
	self.shvar:set(join('-', self.deck));
end


function Cards:read()
	local s = self.shvar:get()
	self.deck = split('-', s);
end


return Cards

