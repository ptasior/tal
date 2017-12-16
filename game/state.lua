Dice = require('game/dice')
GuiHelpers = require('game/gui_helpers')
class = require('lua_lib/class')

local states = {
	wait = {'preMove'},
	preMove = {'castDice'},
	castDice = {'decideDirection'},
	decideDirection = {'move'},
	move = {'wait'},
	battle = {'wait'},

}

GameState = class(function(self)
		self.state = 'wait';
		self.nextState = nil;
		self.waitUserInput = false;
		self.myTurn = false;
	end)

function GameState:process()
	if(self.waitUserInput) then
		return;
	end

	print('Processing '..self.state);

	self.nextState = states[self.state][1];
	self['_'..self.state](self);
	self.state = self.nextState;
end

function GameState:isMyTurn()
	return self.myTurn;
end

function GameState:_wait()
	if(not self:isMyTurn()) then
		self.nextState = 'wait';
		return;
	end
end

function GameState:_preMove()
	if(board.action) then
		board.action:perform();
	end
	-- abilities
end

function GameState:_castDice()
	local r = Dice:cast();
	player.steps = r;
	GuiHelpers:message('You\'ve casted '..r, nil);
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
		if(#keys(field.directions) > 2 or not inList(keys(field.directions), player.direction)) then
			self.nextState = 'decideDirection';
			return;
		end
		self.nextState = 'move';
	else
		field:onLand();
		player.direction = nil;
		log('end turn')
		self.myTurn = false;
	end
end

return GameState;

