Server = class(function(self)
	end)


function Server:newGame()
	sharedData.at('gameStarted'):set('true');
	self.gameStarted = true;
end


function Server:isGameStarted()
	return self.gameStarted;
end


function Server:update(line)
	if(startsWith(line, 'server\1me\1no')) then
		self:logIn()
	end
end


function Server:logIn()
	self.meNo = sharedData.at('server'):at('me'):at('no'):get();
	self.meName = GuiHelpers:input("What is your nickname?");
	self.meName = self.meName..' - '..sharedData.at('server'):at('me'):at('addr'):get();

	sharedData.at('server'):at('clients'):at(self.meNo):at('name'):set(self.meName);

	if(sharedData.at('gameStarted'):get() == 'true') then
		GuiHelpers:message('The game is on', 'The game is on, you cannot join it now.');
	end
end

return Server

