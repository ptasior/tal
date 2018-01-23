Server = class(function(self)
		self.pendingTransactions = {}
	end)


function Server:newGame()
	sharedData.root():at('gameStarted'):set('true');
	self.gameStarted = true;
end


function Server:isGameStarted()
	return self.gameStarted;
end


function Server:update(line)
	if(startsWith(line, 'server\1me\1no')) then
		self:logIn()
	end

	if(startsWith(line, 'server\1me\1transaction\1true')) then
		self:executeTransactions()
	end
end


function Server:executeTransactions()
	-- Execute all handlers
	for _, h in ipairs(self.pendingTransactions) do
		h();
	end
	sharedData.root():at('server'):at('transaction'):set('stop');
end


function Server:inTransaction(handle)
	sharedData.root():at('server'):at('transaction'):set('start');
	self.pendingTransactions[#self.pendingTransactions+1] = handle;
end


function Server:logIn()
	self.meNo = sharedData.root():at('server'):at('me'):at('no'):get();

	-- If the game has not yet started
	self:inTransaction(function()
		if(sharedData.root():at('gameStarted'):get() ~= 'true') then
			self.meName = GuiHelpers:input("What is your name?");
			local ok = false;
			while(not ok) do
				ok = true;
				for i=0,sharedData.root():at('server'):at('clients'):get()-1 do
					if(sharedData.root():at('server'):at('clients'):at(i):at('name'):get() == self.meName) then
						self.meName = GuiHelpers:input("This name is already in use. Pick another one.");
						ok = false;
						break;
					end
				end
			end
		else
			local inactive = {};
			for i=0,sharedData.root():at('server'):at('clients'):get()-1 do
				if(sharedData.root():at('server'):at('clients'):at(i):at('addr'):get() == '') then
					local name = sharedData.root():at('server'):at('clients'):at(i):at('name'):get();
					if(name ~= '') then
						inactive[#inactive+1] = name;
					end
				end
			end

			if(#inactive == 0) then
				GuiHelpers:message('The game is on, you cannot join it');
				return;
			else
				self.meName = GuiHelpers:selectFrom('Which player are you?', inactive, inactive);
			end
		end
		sharedData.root():at('server'):at('clients'):at(self.meNo):at('name'):set(self.meName);
	end)
end

function Server:showWindow()
	self.widget = {};

	self.widget['box'] = Box.new("Server");
	self.widget['box']:setPosition(40,140);
	-- self.widget['box']:setLayout(0);
	-- self.widget['box']:setCenter(false);
	self.widget['box']:setPadding(3,3);
	self.widget['box']:setSize(306,220);


	self.widget['print'] = Button.new('Print tree');
	self.widget['print']:onClickLua(function()
			sharedData.print();
		end);
	self.widget['box']:addButton(self.widget['print']);

	self.widget['tstart'] = Button.new('Start transaction');
	self.widget['tstart']:onClickLua(function()
			sharedData.root():at('server'):at('transaction'):set('start');
		end);
	self.widget['box']:addButton(self.widget['tstart']);

	self.widget['tstop'] = Button.new('Finish transaction');
	self.widget['tstop']:onClickLua(function()
			sharedData.root():at('server'):at('transaction'):set('stop');
		end);
	self.widget['box']:addButton(self.widget['tstop']);

	self.widget['write'] = Button.new('write Something');
	self.widget['write']:onClickLua(function()
			local v = sharedData.root():at('test'):at('variable'):get();
			sharedData.root():at('test'):at('variable'):set(v+1);
		end);
	self.widget['box']:addButton(self.widget['write']);

	gui.rootWidget():addBox(self.widget['box']);
end

return Server

