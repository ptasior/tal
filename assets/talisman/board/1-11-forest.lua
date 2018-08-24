return {
	label = 'forest',
	realm = "outer",
	pos_x = 234,
	pos_z = 152,
	directions = {
		ccw = "1-10-plains",
		cw = "1-12-village"
	},
	description = 'Roll 1 die:\n'..
					'1 - Attacked by a brigade of strength 4\n'..
					'2-3 - Lost for 1 turn\n'..
					'4-5 - Safe'..
					'6 - A ranger guides you out, gain 1 craft',
	onLandFnc = function()
		local r = Dice:cast();
		if(r == 1) then
			journal:add("You are attacked by a brigade of strength 4");
			battle.enemy.name = "brigade";
			battle.enemy.strength = 4;
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

