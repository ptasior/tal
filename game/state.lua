class = require('lua_lib/class')

local states = {
	boardAction = {'abilities'},
	abilities = {'castDice'},
	castDice = {'move'},
	move = {'wait', 'landOnField'},
	-- move = {'passField', 'landOnField'},
	passField = {'decideDirection', 'battle', 'move'},
	decideDirection = {'battle', 'move'},
	landOnField = {'battle'},
	battle = {'updatePlayer'},
	updatePlayer = {'wait', 'move'},
	wait = {'boardAction'},
}

GameState = class(function(self)
		self.state = 'wait';
		self.nextState = nil;
		self.waitUserInput = false;
		self.myTurn = false;
	end)

function GameState:process()
	-- Implement waitUserInput and modal messageBoxes
	if(self.waitUserInput) then
		return;
	end

	log('Processing '..self.state);

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

function GameState:_boardAction()
	if(board.action) then
		board.action:perform();
	end
end

function GameState:_abilities()
end

function GameState:_castDice()
	gui.message('Dice says', 'random number = 3');
end

function GameState:_move()
	log('go, go!');
	self.myTurn = false;
end

return GameState;

