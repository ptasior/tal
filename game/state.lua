Dice = require('game/dice')
GuiHelpers = require('game/gui_helpers')
class = require('lua_lib/class')

local states = {
	wait = {'preMove'},
	preMove = {'castDice'},
	castDice = {'decideDirection'},
	decideDirection = {'move'},
	move = {'wait'},


	passField = {'decideDirection'},
	landOnField = {'battle'},
	battle = {'updatePlayer'},
	updatePlayer = {'wait'},

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
	local r = Dice:cast();
	player.steps = r;
	GuiHelpers:askQuestion(
		'Which direction do you want to go?',
		player:getPossibleDirections(), function(answer)
			player.direction = answer;
			log('chosen direction: '..answer);
		end)
end

function GameState:_move()
	log('go, go!');
	self.myTurn = false;
end

return GameState;

