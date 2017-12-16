Dice = require('game/dice')
GuiHelpers = require('game/gui_helpers')
class = require('lua_lib/class')

local states = {
	wait = {'preMove'},
	preMove =  {'decideDirection'},
	decideDirection = {'move'},
	move = {'fieldAction'},
	fieldAction = {'endTurn'},
	fieldDice = {'endTurn'},
	battle = {'endTurn'},
	endTurn = {'wait'},
}

GameState = class(function(self)
		self.state = 'wait';
		self.nextStateVar = nil;
		self.waitUserInput = false;
		self.myTurn = false;
	end)

function GameState:process()
	if(self.waitUserInput) then
		return;
	end

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
		return;
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
	-- abilities

	local r = Dice:cast();
	player.steps = r;
end

function GameState:_decideDirection()
	GuiHelpers:askQuestion(
		'Which direction do you want to go?',
		player:getPossibleDirections(), function(answer)
			player.direction = answer;
			log('chosen direction: '..answer);
		end)
end

function GameState:_move()
	player.steps = player.steps-1;

	local nf = player:getCurrentField().directions[player.direction];
	player.field = nf;
	player:update();
	local field = board:getField(nf);

	if(player.steps ~= 0) then
		field:onPass();
		if(#keys(field.directions) > 2 or not hasKey(field.directions, player.direction)) then
			self:nextState('decideDirection');
			return;
		end
			self:nextState('move');
	else
		player.direction = nil;
	end
end

function GameState:_fieldAction()
	player:getCurrentField():onLand();
end

function GameState:_battle()
	journal:add('battle');
end

function GameState:_endTurn()
	self.myTurn = false;
end

return GameState;

