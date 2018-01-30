class = require('lua_lib/class')
var_dump = require('lua_lib/var_dump')
require('lua_lib/functions')

GuiHelpers = require('letters/gui_helpers')
Cards = require('letters/cards')
Game = require('letters/game')
Players = require('letters/players')
Server = require('letters/server')

server = nil;
players = nil;
game = nil;


function setupGame()
	local names = {"Guard", "Priest", "Baron", "Handmaid",
					"Prince", "King", "Countess", "Princess"};
	local set = {8,7,6,5,5,4,4,3,3,2,2,1,1,1,1,1};

	cards = Cards(sharedData:root():at('deck'));
	cards:set(set, names);
end


function startGame()
	log('Start game');

	cards:shuffle();
	cards:save();

	local ap = players:getNames();

	for i = 1,#ap do
		local c = cards:drawOne();
		players:get(ap[i]):at('card'):set(c);
	end

	cards:save();
	game:initTurn();
end


function perform(card)
	log('Performing '..card)
end


function playTurn()
	log('My Turn!')

	-- Draw a card
	local drawn = cards:drawOne();
	cards:save();
	log('drawn '..drawn)

	-- Play a card
	-- players:me():at('protected'):set(false);
	local hand = players:me():at('card'):get();
	local choice = {cards:toName(drawn), cards:toName(hand)};
	local played = GuiHelpers:askQuestion("Which card would you like to play?", choice);

	log('played = '..played)
	local left = nil;
	if(drawn ~= hand) then
		left = without(choice, played);
	else
		-- When two cards are the same, list is cleaned
		left = {played};
	end
	log('left'..var_dump(left))

	players:me():at('card'):set(cards:toNumber(left[1]));
	perform(played);

	if(#cards.deck == 0) then
		-- Check who win
		GuiHelpers:message("End of the game");
	end

	-- Next player's turn
	game:nextTurn();
end


function showCheatsheet()
	local cheatsheet = Box.new("Cheatsheet");
	cheatsheet:setRect(100,100,420,224);
	gui:rootWidget():addBox(cheatsheet);

	local cs_scroll = Scroll.new();
	cs_scroll:addLabel(Label.new("Princess (1) - loose when discarded"));
	cs_scroll:addLabel(Label.new("Countess (1) - discard when with King or Prince"));
	cs_scroll:addLabel(Label.new("King (1) - trade hands"));
	cs_scroll:addLabel(Label.new("Prince (2) - discard cards and draw new"));
	cs_scroll:addLabel(Label.new("Handmaid (2) - protection till the end of round"));
	cs_scroll:addLabel(Label.new("Baron (2) - compare hands; lower is out"));
	cs_scroll:addLabel(Label.new("Priest (2) - look at a hand"));
	cs_scroll:addLabel(Label.new("Guard (5) - guess player's hand"));
	cheatsheet:addScroll(cs_scroll);
end


function setup()
	math.randomseed(os.time());

	-- Temporarily
	setLoopResolution(500);

	server = Server();
	server:showWindow();

	game = Game();
	game:addOnInit(startGame);
	game:addOnTurn(playTurn);

	players = Players();
	players:showWidget();

	setupGame();
	-- showCheatsheet();

	log('Lua setup done');
end


function resizeWindow()
end


function loop()
end


function sharedDataChange(line)
	server:update(line);
	game:update(line);
	players:update(line);

	if(startsWith(line, 'deck')) then
		cards:read();
	end
end

