Game = class(function(self)
		self.gm = sharedData:root():at('game');
		self.onInit = {};
		self.onStart = {};
		self.onTurn = {};
		self.onLost = {};

		main_addOnSharedDataUpdate(function(l) self:update(l); end);
	end)


function Game:start()
	if(#players:getNames() < 2) then
		execute(function()
				GuiHelpers:message('Not enough players to start game');
			end);
		return;
	end
	server:transaction(function()
			-- Check if someone else hasn't started the game before
			if(self.gm:at('started'):get() == 'true') then
				log("Game already started");
				return;
			end

			self.gm:at('started'):set_i('true');
			for i=1,#self.onInit do
				self.onInit[i]();
			end
		end)
end


function Game:isStarted()
	return self.gm:at('started'):get() == 'true';
end


-- Initializes the game, executed by only one client
function Game:addOnInit(handler)
	self.onInit[#self.onInit+1] = handler;
end

-- Execute when the game started by all clients
function Game:addOnStart(handler)
	self.onStart[#self.onStart+1] = handler;
end

function Game:addOnTurn(handler)
	self.onTurn[#self.onTurn+1] = handler;
end

function Game:addOnLost(handler)
	self.onLost[#self.onLost+1] = handler;
end

-- Called from Player
function Game:doOnLost()
	for i=1,#self.onLost do
		self.onLost[i]();
	end
end


function Game:update(line)
	if(startsWith(line, 'game\1started\1true')) then
		if(#self.onStart == 0) then return; end

		server:transaction(function()
			for i=1,#self.onStart do
				self.onStart[i]();
			end
		end);
	end
	if(startsWith(line, 'game\1turn')) then
		if(self:isMyTurn()) then
			for i=1,#self.onTurn do
				self.onTurn[i]();
			end
		else
			log('Not my turn but '..self.gm:at('turn'):get());
		end
	end
end


function Game:isMyTurn()
	return self:whoseTurn() == players.meName;
end


function Game:whoseTurn()
	return self.gm:at('turn'):get();
end


function Game:initTurn()
	local ap = players:getNames();
	local turn = ap[math.random(#ap)];

	self.gm:at('turn'):set(turn);
	log('turn = '..turn);
	return turn;
end


function Game:whoseTurnNext()
	local ap = players:getPlaying();
	local pos = find(players.meName, ap)+1; -- TODO fix me

	if(pos > #ap) then pos = 1; end -- Modulo with offset +1 is too complicated :(
	log('whoseTurnNext: pos = '..pos..' pl = '..var_dump(ap))

	return ap[pos];
end


function Game:nextTurn()
	local nxt = self:whoseTurnNext();

	self.gm:at('turn'):set(nxt);
	players:reset();
end


function Game:finishGame()
	self.gm:at('turn'):set('');
	self.gm:at('started'):set_i('false');
	players:reset();
end

return Game

