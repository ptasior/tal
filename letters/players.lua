-- Requires Game and Server to be initialised

Players = class(function(self)
		self.pl = sharedData:root():at('players');

		server:addOnConnect(function()
				server:transaction(function() self:logIn() end);
			end);

	end)


function Players:logIn()
	print("logging in")
	-- Executed in transaction
	if(not game:isStarted()) then
		print("not started")
		self.meName = GuiHelpers:input("What is your name?");
		while(true) do
			if(inList(self:getNames(), self.meName)) then
				self.meName = GuiHelpers:input("This name is already in use.\nPick another one.");
			else
				break;
			end
		end
	else
		print("started")
		local inactive = self:getInactive();

		if(#inactive == 0) then
			GuiHelpers:message('The game is full, you cannot join in');
			return;
		else
			local names = map(inactive, function(e) return e['name']; end);
			self.meName = GuiHelpers:selectFrom('Which player are you?', names);
		end
	end
	self.pl:at(self.meName):at('cliNo'):set(server:meNo());
end


function Players:getNames()
	return self.pl:branches();
end


function Players:getPlaying()
	local names = self:getNames();
	local ret = {};
	for i = 1, #names do
		if(self:get(names[i]):at('lost'):get() ~= 'true') then
			ret[#ret+1] = names[i];
		end
	end
	return ret;
end


function Players:lose(name)
	self:get(name):at('lost'):set_i('true');
	log('Player '..name..' has lost');
end


-- Marks al players as playing, called from Game:finishGame()
function Players:reset()
	local pl = self:getNames();
	for i = 1, #pl do
		self:get(pl[i]):at('lost'):set('false');
	end
end


function Players:isActive(name)
	local cli = server:getClients();
	local cliNo = tonumber(self.pl:at(name):at('cliNo'):get());
	return hasKey(cli, cliNo);
end


function Players:getInactive()
	local nm = self:getNames();
	local cli = server:getClients();
	local ret = {};

	for i = 1, #nm do
		local cliNo = tonumber(self.pl:at(name):at('cliNo'):get());
		if(not hasKey(cli, cliNo)) then
			ret[#ret+1] = nm[i];
		end
	end

	return ret;
end


function Players:update(line)
	-- TODO Fix me
	if(startsWith(line, 'players')) then
		self:updateWidget();
	end
	if(self.meName and
		startsWith(line, 'players\1'..self.meName..'\1lost\1true')) then
		game:doOnLost();
	end
end


function Players:showWidget()
	self.widget = {};

	self.widget['box'] = Box.new("Players");
	self.widget['box']:setPosition(40,40);
	self.widget['box']:setLayout(0);
	self.widget['box']:setCenter(false);
	self.widget['box']:setPadding(3,3);

	self.widget['list'] = Scroll.new();
	self.widget['list']:setRect(3,6,300,150);
	self.widget['box']:addScroll(self.widget['list']);

	self:toggleStartButton();

	gui:rootWidget():addBox(self.widget['box']);
end


function Players:toggleStartButton()
	if(game:isStarted()) then
		self.widget['box']:removeButton(players['widget']['start']);
		self.widget['box']:setSize(306,220-36);
		self.widget['start'] = nil;
	else
		self.widget['start'] = Button.new('Start the game');
		self.widget['start']:setRect(3,162,300,33);
		self.widget['start']:onClickLua(function()game:start(); end);
		self.widget['box']:addButton(self.widget['start']);
		self.widget['box']:setSize(306,220);

	end
end


function Players:updateWidget()
	local names = self:getNames();

	self.widget['list']:clear();
	for i = 1,#names do
		local txt = names[i];
		if(not self:isActive(txt)) then
			txt = 'X ' .. txt;
		elseif(self.pl:at(txt):at('lost'):get() == 'true') then
			txt = '- ' .. txt;
		end

		self.widget['list']:addLabel(Label.new(txt));
	end

	if(self.meName) then
		self.widget['box']:setTitle('Players - '..self.meName)
	end
end


function Players:get(name)
	return self.pl:at(name);
end


function Players:me()
	return self:get(self.meName);
end

return Players

