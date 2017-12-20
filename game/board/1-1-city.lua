return {
	label = 'city',
	realm = "outer",
	pos_x = 77,
	pos_z = 23,
	directions = {
		ccw = "1-23-fields",
		cw = "1-2-fields"
	},
	description = 
			'You MAY visit one of the following:\n'..
			'Enchantress - gain or loose strength, craft or spell or be tuned into a toad\n'..
			'Alchemist - turn any of your object into gold\n'..
			'Doctor - heal up to two lives at the cost of 1 gold each',
	onLandFnc = function()
		local ans = GuiHelpers:askQuestion(self.description
				{'Enchantress', 'Alchemist', 'Doctor', 'No one'}
			);

		if(ans == 'Enchantress') then
			local r = Dice:cast();
			if(r == 1) then
				player:turnIntoToad(3);
				journal:add('You are turned into a toad for 3 turns');
			elseif(r == 2) then
				player.strength = player.strength +1;
				journal:add('You gained 1 strength');
			elseif(r == 3) then
				player.strength = player.strength -1;
				journal:add('You lost 1 strength');
			elseif(r == 4) then
				player.craft = player.craft +1;
				journal:add('You gained 1 craft');
			elseif(r == 5) then
				player.craft = player.craft -1;
				journal:add('You lost 1 craft');
			elseif(r == 6) then
				player:drawSpell()
			end

		elseif(ans == 'Alchemist') then
			local _,dis = GuiHelpers:selectFrom(
					'Pick objects to be turned into gold',
					player.objects, {'OK'});

			player.gold = player.gold + #dis;
			journal:add('You gained '..#dis..' gold');

		elseif(ans == 'Doctor') then
			local opts = {};

			if(player.gold <1) then       return;
			elseif(player.gold == 1) then opts = {1};
			else                          opts = {1, 2};
			end

			local ans = GuiHelpers:askQuestion(
					'How many coins do you wan to exchange?', opts
				);

			player.gold = player.gold - ans
			player.lives = player.lives + ans
			journal:add('You gained '..ansr..' lives');
		end
	end,
}

