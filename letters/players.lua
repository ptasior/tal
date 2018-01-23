Players = class(function(self)
		self.players = {};
	end)


function Players:getActive()
	self.players = {};

	local cliNo = sharedData.root():at('server'):at('clients'):get();
	if(cliNo == '') then return {}; end

	for i = 0,cliNo-1 do
		local addr = sharedData.root():at('server'):at('clients'):at(i):at('addr'):get();
		local name = sharedData.root():at('server'):at('clients'):at(i):at('name'):get();
		if(addr ~= '') then
			self.players[#self.players+1] = name;
		end
	end

	return self.players;
end

function Players:update(line)
	if(startsWith(line, 'gameStarted')) then
		self:toggleStartButton();
	end

	if(startsWith(line, 'server\1clients\1')) then
		log('updat')
		self:updateWidget();
	end
end


function Players:showWidget(onNewGame)
	self.onNewGame = onNewGame;

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

	gui.rootWidget():addBox(self.widget['box']);
end


function Players:toggleStartButton()
	if(server:isGameStarted()) then
		self.widget['box']:removeButton(players['widget']['start']);
		self.widget['box']:setSize(306,220-36);
		self.widget['start'] = nil;
	else
		self.widget['start'] = Button.new('Start the game');
		self.widget['start']:setRect(3,162,300,33);
		self.widget['box']:addButton(self.widget['start']);
		self.widget['box']:setSize(306,220);

		self.widget['start']:onClickLua(function()
				server.newGame();
				players:toggleStartButton();
				-- TODO Block more than one user click
				players:onNewGame();
			end);
	end
end


function Players:updateWidget()
	self:getActive()

	self.widget['list']:clear();
	for i = 1,#self.players do
		self.widget['list']:addLabel(Label.new(self.players[i]));
	end
end


function Players:get(name)
	return sharedData.root():at('players'):at(name);
end


function Players:me()
	return sharedData.root():at('players'):at(self.meName);
end


function Players:nextPlayer()
	local ap = self.players;
	local pos = find(server.meName, ap)+1;

	if(pos > #ap) then pos = 1; end -- Modulo with offset +1 is too complicated :(

	-- log('mename= '..meName..' pos = '..pos..' vd = '..var_dump(ap))

	return ap[pos];
end


return Players

