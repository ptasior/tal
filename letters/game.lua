class = require('lua_lib/class')
var_dump = require('lua_lib/var_dump')
require('lua_lib/functions')

GuiHelpers = require('letters/gui_helpers')

players = {};
meNo = ''


function shuffle(arr)
	local p;
	for i = #arr, 2, -1 do
		p = math.random(i);
		arr[i], arr[p] = arr[p], arr[i];
	end
	return arr;
end


function waitFor(s_data)
	local v = s_data:get();
	while(v == '') do
		appWait(100);
		v = s_data:get();
	end
	return v;
end


function activePlayers()
	local ret = {};

	local cliNo = sharedData.at('server'):at('clients'):get();
	if(cliNo == '') then return {}; end

	for i = 0,cliNo-1 do
		local addr = sharedData.at('server'):at('clients'):at(i):at('addr'):get();
		local name = sharedData.at('server'):at('clients'):at(i):at('name'):get();
		if(addr ~= '') then
			ret[#ret+1] = name;
		end
	end

	return ret;
end


-- Card name to number
function cName(no)
	local names = {"Guard", "Priest", "Baron", "Handmaid",
					"Prince", "King", "Countess", "Princess"};
	return names[tonumber(no)];
end


-- Card number to name
function cNumber(name)
	local names = {"Guard", "Priest", "Baron", "Handmaid",
					"Prince", "King", "Countess", "Princess"};
	
	for i = 0,#names do
		if(names[i] == name) then
			return i;
		end
	end
	return -1;
end


function startGame()
	log('Start game');
	local set = {8,7,6,5,5,4,4,3,3,2,2,1,1,1,1,1};
	set = shuffle(set);

	local ap = activePlayers();

	for i = 1,#ap do
		local head = table.remove(set,1);
		sharedData.at('players'):at(ap[i]):at('card'):set(head);
	end

	sharedData.at('deck'):set(join('-', set));
	local turn = ap[math.random(#ap)];
	sharedData.at('turn'):set(turn);
	log('turn = '..turn);
end


function perform(card)
	log('Performing '..card)
end


function playTurn()
	log('My Turn!')

	-- Draw a card
	local set = sharedData.at('deck'):get();
	set = split('-', set);
	local drawn = table.remove(set,1);
	log('drawn '..drawn)
	sharedData.at('deck'):set(join('-', set));

	-- Play a card
	-- sharedData.at('players'):at(meName):at('protected'):set(false);
	local hand = sharedData.at('players'):at(meName):at('card'):get();
	local cards = {cName(drawn), cName(hand)};
	local played = GuiHelpers:askQuestion( "Which card would you like to play?", cards);

	log('played = '..played)
	local left = without(cards, played);
	log('left'..var_dump(left))
	sharedData.at('players'):at(meName):at('card'):set(cNumber(left[1]));
	perform(played);

	-- Turn moves to the next player
	local ap = activePlayers();
	local pos = find(meName, ap)+1;
	if(pos > #ap) then pos = 1; end -- Modulo with offset +1 is too complicated :(

	log('mename= '..meName..' pos = '..pos..' vd = '..var_dump(ap))

	sharedData.at('turn'):set(ap[pos]);
	log('next turn = '..ap[pos]);

	if(#set == 0) then
		-- Check who win
		GuiHelpers:message("end of the game");
	end
end


function updatePlayers()
	-- local _ = waitFor(sharedData.at('server'):at('clients'):at(meNo):at('name'));

	local ap = activePlayers();
	local j_ap = var_dump(ap);

	if(j_ap ~= players['active']) then
		log('Update players');

		players['active'] = j_ap;

		players['widget']['box']:removeScroll(players['widget']['list']);
		players['widget']['list'] = Scroll.new();
		players['widget']['box']:addScroll(players['widget']['list']);

		for i = 1,#ap do
			players['widget']['list']:addLabel(Label.new(ap[i]));
		end
	end
end


function initNet()
	if(meNo ~= '') then return; end

	meNo = waitFor(sharedData.at('server'):at('me'):at('no'));
	log('received Meno '..meNo)

	meName = GuiHelpers:input("What is your name?");
	meName = meName..' - '..waitFor(sharedData.at('server'):at('clients'):at(meNo):at('addr'));

	-- TODO Make sure name is unique
	sharedData.at('server'):at('clients'):at(meNo):at('name'):set(meName);
end


function setup()
	math.randomseed(os.time());

	-- Temporarily
	setLoopResolution(500);

	tt_btn = GuiHelpers:randomButton('Net', function()
			sharedData.at('test'):at("rq"):set('qq');
		end);

	pp_btn = GuiHelpers:randomButton('Print', function()
			sharedData.print();
		end);


	players['count'] = 0;
	players['widget'] = {};

	players['widget']['box'] = Box.new("Players");
	players['widget']['box']:setRect(400,100,300,200);
	gui.rootWidget():addBox(players['widget']['box']);

	players['widget']['list'] = Scroll.new();
	players['widget']['box']:addScroll(players['widget']['list']);

	-- players['widget']['startbox'] = Box.new("Start");
	-- players['widget']['startbox']:setRect(150,150,150,70);
	-- gui.rootWidget():addBox(players['widget']['startbox']);

	players['widget']['start'] = Button.new('Start the game');
	players['widget']['start']:setRect(400,100,300,20);
	players['widget']['box']:addButton(players['widget']['start']);

	players['widget']['start']:onClickLua(function()
			startGame();
		end);

	-- local cheatsheet = Box.new("Cheatsheet");
	-- cheatsheet:setRect(100,100,420,224);
	-- gui.rootWidget():addBox(cheatsheet);
    --
	-- local cs_scroll = Scroll.new();
	-- cs_scroll:addLabel(Label.new("Princess (1) - loose when discarded"));
	-- cs_scroll:addLabel(Label.new("Countess (1) - discard when with King or Prince"));
	-- cs_scroll:addLabel(Label.new("King (1) - trade hands"));
	-- cs_scroll:addLabel(Label.new("Prince (2) - discard cards and draw new"));
	-- cs_scroll:addLabel(Label.new("Handmaid (2) - protection till the end of round"));
	-- cs_scroll:addLabel(Label.new("Baron (2) - compare hands; lower is out"));
	-- cs_scroll:addLabel(Label.new("Priest (2) - look at a hand"));
	-- cs_scroll:addLabel(Label.new("Guard (5) - guess player's hand"));
	-- cheatsheet:addScroll(cs_scroll);

	log('Lua setup done');
end

function resizeWindow()
end

function loop()
	initNet();

	if(sharedData.at('turn'):get() == meName) then
		playTurn();
	else
		print('not my turn', sharedData.at('turn'):get(), meName);
	end
end

function sharedDataUpdated(line)
	updatePlayers();
end

