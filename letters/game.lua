Game = class(function(self)
		self.gm = sharedData.root():at('game');
		self.onStart = {};
		self.onInit = {};
	end)


function Game:start()
	server:transaction(function()
			-- Check if someone else hasn't started the game before
			if(self:isStarted()) then return; end

			self.gm:at('started'):set('true');
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
	self.onStart[#self.onStart+1] = handler;
end


-- Whrnthe game started, executed by all clients
function Game:addOnStart(handler)
	self.onStart[#self.onStart+1] = handler;
end


function Game:update(line)
	if(startsWith(line, 'game\1started\1true')) then
		for i=1,#self.onStart do
			self.onStart[i]();
		end
	end
end


function Game:whoseTurn()
end


function Game:isMyTurn()
end


return Game

