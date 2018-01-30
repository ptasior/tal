Game = class(function(self)
		self.gm = sharedData:root():at('game');
		self.onInit = {};
		self.onStart = {};
		self.onTurn = {};
		self._isStarted = false;
	end)


function Game:start()
	server:transaction(function()
			-- Check if someone else hasn't started the game before
			if(self.gm:at('started'):get() == 'true') then
				log("Game already started");
				return;
			end

			self.gm:at('started'):set('true');
			self._isStarted = true;
			for i=1,#self.onInit do
				self.onInit[i]();
			end
		end)
end


function Game:isStarted()
	return self._isStarted;
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


function Game:update(line)
	if(startsWith(line, 'game\1started\1true')) then
		if(#self.onStart == 0) then return; end

		server:transaction(function()
			self._isStarted = true;
			for i=1,#self.onStart do
				self.onStart[i]();
			end
		end)
	end
	if(startsWith(line, 'game\1turn')) then
		if(self:isMyTurn()) then
			server:transaction(function()
				for i=1,#self.onTurn do
					self.onTurn[i]();
				end
			end)
		else
			log('Not my turn but '..self.gm:at('turn'):get())
		end
	end
end


function Game:isMyTurn()
	return self.gm:at('turn'):get() == players.meName;
end


function Game:initTurn()
	local ap = players:getNames();
	local turn = ap[math.random(#ap)];

	self.gm:at('turn'):set(turn);
	log('turn = '..turn);
end


function Game:nextTurn()
	local ap = players:getPlaying();
	local pos = find(players.meName, ap)+1; -- TODO fix me

	if(pos > #ap) then pos = 1; end -- Modulo with offset +1 is too complicated :(

	log('nextTurn: pos = '..pos..' pl = '..var_dump(ap))

	self.gm:at('turn'):set(ap[pos]);
end

return Game

