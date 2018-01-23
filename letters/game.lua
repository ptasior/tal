class = require('lua_lib/class')
var_dump = require('lua_lib/var_dump')
require('lua_lib/functions')

GuiHelpers = require('letters/gui_helpers')
Cards = require('letters/cards')
Players = require('letters/players')
Server = require('letters/server')


function waitFor(s_data)
	local v = s_data:get();
	while(v == '') do
		appWait(100);
		v = s_data:get();
	end
	return v;
end


function startGame()
	log('Start game');

	local names = {"Guard", "Priest", "Baron", "Handmaid",
					"Prince", "King", "Countess", "Princess"};
	local set = {8,7,6,5,5,4,4,3,3,2,2,1,1,1,1,1};

	cards = Cards(sharedData.root():at('deck'));
	cards:set(set, names);


	local ap = players.getActive();

	for i = 1,#ap do
		local c = cards.drawOne();
		players:get(ap[i]):at('card'):set(c);
	end

	cards.save();
	local turn = ap[math.random(#ap)];
	sharedData.root():at('turn'):set(turn);
	log('turn = '..turn);
end


function perform(card)
	log('Performing '..card)
end


function playTurn()
	log('My Turn!')

	-- Draw a card
	local drawn = cards.drawOne();
	log('drawn '..drawn)

	-- Play a card
	-- sharedData.at('players'):at(meName):at('protected'):set(false);
	local hand = players:me():at('card'):get();
	local cards = {cards:toName(drawn), cards:toName(hand)};
	local played = GuiHelpers:askQuestion("Which card would you like to play?", cards);

	log('played = '..played)
	local left = without(cards, played);
	log('left'..var_dump(left))

	players:me():at('card'):set(cards:toNumber(left[1]));
	perform(played);

	-- Next player's turn
	local nxt = players.nextPlayer();
	sharedData.root():at('turn'):set(nxt);
	log('next turn = '..nxt);

	if(#cards.deck == 0) then
		-- Check who win
		GuiHelpers:message("end of the game");
	end
end


function setup()
	math.randomseed(os.time());

	-- Temporarily
	setLoopResolution(500);

	server = Server();
	players = Players();
	players:showWidget(startGame);

	server:showWindow();

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
end

function isMyTurn()
	if(sharedData.root():at('turn'):get() == server.meName) then
		return true;
	end

	print('not my turn - ', sharedData.root():at('turn'):get(), ' me: ', server.meName);
	return false;
end

function newSharedData(line)
	server:update(line);
	players:update(line);
	if(startsWith(line, 'turn') and isMyTurn()) then
		playTurn();
	end
end

