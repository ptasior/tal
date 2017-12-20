local states = {
	wait = {'preMove'},
	preMove =  {'move'},
	move = {'fieldAction'},
	fieldAction = {'endTurn'},
	fieldDice = {'endTurn'},
	battle = {'endTurn'},
	endTurn = {'wait'},
}

GameState = class(function(self)
		self.state = 'wait';
		self.nextStateVar = nil;
		self.myTurn = false;
	end)

function GameState:process()
	print('Processing '..self.state);

	self.nextStateVar = states[self.state][1];
	self['_'..self.state](self);
	self.state = self.nextStateVar;
end

function GameState:isMyTurn()
	return self.myTurn;
end

function GameState:nextState(state)
	if(not hasKey(states, state)) then
		log('State not found: '..state);
	end
	self.nextStateVar = state;
end

function GameState:_wait()
	if(not self:isMyTurn()) then
		self:nextState('wait');
	else
		-- Update state from network
	end
end

function GameState:_preMove()
	if(player.waitTurn) then
		journal:add("Skipping turn");
		player.waitTurn = player.waitTurn -1;
		self:nextState("endTurn");
	end

	if(board.action) then
		board.action:perform();
	end

	if(player:getCurrentField().realm == "crown") then
		-- select player
		-- local r = Dice:cast();
		-- if(r >= 4)
		--  other_players:player(selected):looselife();
		-- self:nextState("endTurn");
		-- return;
		-- TODO Not sure what happens when there is > player here
	end
	-- abilities
	-- cast spells

	if(inList({"outer", "middle"}, player:getCurrentField().realm)) then
		local r = Dice:cast();
		player.steps = r;
	elseif(player:getCurrentField().realm == "inner") then
		player.steps = 1;
	end

end

function GameState:_move()
	GuiHelpers:askQuestion(
		'Which direction do you want to go?',
		player:getPossibleDirections(), function(answer)
			player.direction = answer;
			log('chosen direction: '..answer);
		end)

	while(player.steps > 0) do
		local nf = player:getCurrentField().directions[player.direction];
		player.field = nf;
		player:update();
		appRefresh();
		appSleep(500);
		player.steps = player.steps-1;

		local field = board:getField(nf);

		if(player.steps > 0) then
			field:onPass();
		end

		if(player.steps > 0) then -- Check again if onPass did not change anything
			if(#keys(field.directions) > 2 or not hasKey(field.directions, player.direction)) then
				GuiHelpers:askQuestion(
					'Which direction do you want to go?',
					player:getPossibleDirections(), function(answer)
						player.direction = answer;
						log('chosen direction: '..answer);
					end)
			end
		end
	end
	player.direction = nil;
end

function GameState:_fieldAction()
	player:getCurrentField():onLand();
end

function GameState:_battle()
	battle:preparePlayer()
	battle:fight()
end

function GameState:_endTurn()
	battle:clear()
	self.myTurn = false;
end

return GameState;

