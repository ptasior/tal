Battle = class(function(self)
		self:clear();
	end)

function Battle:clear()
	self.player = {};

	self.enemy = {};
	self.enemy.strength = 1;
	self.enemy.name = "unknown";
end

function Battle:preparePlayer()
	self.player.strength = player.strength;
	self.player.craft = player.craft;
end

function Battle:fight()
	journal:add("You are fighting with a "..self.enemy.name)
	local pl = Dice:cast();
	journal:add("You casted "..pl.. ' total '..pl+self.player.strength)
	local en = Dice:cast();
	journal:add("The enemy casted "..en.. ' total '..en+self.enemy.strength)
	if(pl+self.player.strength > en+self.enemy.strength) then
		self:onWin();
	elseif(pl+self.player.strength < en+self.enemy.strength) then
		self:onLoss();
	else
		self:onDraw();
	end
end

function Battle:onWin()
	journal:add("You win")

end

function Battle:onLoss()
	journal:add("You loose")
	player:looseLife();
	gameState:nextState("endTurn");
end

function Battle:onDraw()
	journal:add("It's a draw")
	gameState:nextState("endTurn");
end

return Battle

