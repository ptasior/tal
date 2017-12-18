return {
	applyFnc = function() return true; end,
	executeFnc = function()
		log('here here')
		local r = Dice:cast();
		if(r == 1) then
			journal:add("You are attacked by a brigade of strength 4");
			gameState:nextState('battle');
		elseif(r==2 or r==3) then
			journal:add("You are lost for 1 turn");
			player.waitTurn = 1;
		elseif(r==4 or r==5) then
			journal:add("You are safe");
		elseif(r==6) then
			journal:add("Ranger guides you out. Gain 1 craft");
			player.craft = player.craft + 1;
		end
	end,
} 
