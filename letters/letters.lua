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


function playersToPick(includeSelf)
	local pls = players:getPlaying();
	local ret = {};

	for i = 1, #pls do
		if(players:get(pls[i]):at('protected'):get() ~= 'true') then
			if(includeSelf or pls[i] ~= players.meName) then
				ret[#ret+1] = pls[i];
			end
		end
	end
	return ret;
end


function perform(card)
	log('Performing '..card);
	if(card == 1) then
		local pl = GuiHelpers:selectFrom("Pick the player", playersToPick(false));
		local crd = GuiHelpers:selectFrom("Guess the card",
											without(cards.names, 'Guard'));
		if(players:get(pl):at('card'):get() == tostring(cards:toNumber(crd))) then
			players:lose(pl);
			server:broadcast(pl..' has lost');
		else
			server:broadcast(players.meName..' used guard but '..pl..' does not have '..crd);
		end

	elseif(card == 2) then
		local pl = GuiHelpers:selectFrom("Pick the player", playersToPick(false));
		GuiHelpers:message(pl..' has '..cards:toName(players:get(pl):at('card'):get()));
		server:broadcast(players.meName..' checked '..pl..'\'s card');

	elseif(card == 3) then
		local pl = GuiHelpers:selectFrom("Pick the player", playersToPick(false));
		local mc = players:me():at('card'):get();
		local pc = players:get(pl):at('card'):get();
		if(pc == mc) then
			server:broadcast(pl..'\'s and '..players.meName..'\'s cards are identical');
		elseif(pc > mc) then
			players:lose(players.meName);
			server:broadcast(players.meName..' has lost');
		elseif(pc < mc) then
			players:lose(pl);
			server:broadcast(pl..' has lost');
		end
	elseif(card == 4) then
		players:get(players.meName):at('protected'):set('true');
		server:broadcast(players.meName..' is protected');

	elseif(card == 5) then
		local pl = GuiHelpers:selectFrom("Pick the player", playersToPick(true));
		local c = cards:drawOne();
		players:get(pl):at('card'):set(c);
		cards:save();
		server:broadcast(pl..' was chosen to draw new cards');

	elseif(card == 6) then
		local pl = GuiHelpers:selectFrom("Pick the player", playersToPick(false));
		local tmp = players:get(pl):at('card'):get();
		players:get(pl):at('card'):set(players:me():at('card'):get());
		players:me():at('card'):set(tmp);
		server:broadcast(pl..' and '..players.meName..' traded hands');

	elseif(card == 7) then
		server:broadcast(players.meName..' played Countess');

	elseif(card == 8) then
		server:broadcast(players.meName..' played Princess and lost');
		players:lose(players.meName);

	end
end


function isGameOver()
	local playing = players:getPlaying();
	if(#playing > 1 and #cards.deck > 0) then
		return false;
	end

	local m_v = 0;
	local m_n = nil;

	-- Find the one with the biggest card
	for i = 1,#playing do
		local p_card = tonumber(players:get(playing[i]):at('card'):get());
		if(p_card > m_v) then
			m_v = p_card;
			m_n = playing[i];
		end
	end

	server:broadcast("End of the game, "..m_n..' won');
	return true;
end


function playTurn()
	log('My Turn!')

	-- Draw a card
	local drawn = cards:drawOne();
	cards:save();
	log('drawn '..drawn)

	-- Play a card
	players:me():at('protected'):set('false');
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
	perform(cards:toNumber(played));

	if(not isGameOver()) then
		game:nextTurn();
	end
end


function showCheatsheet()
	local cheatsheet = Box.new("Cheatsheet");
	cheatsheet:setRect(100,100,420,224);
	gui:rootWidget():addBox(cheatsheet);

	local cs_scroll = Scroll.new();
	cs_scroll:addLabel(Label.new("Princess (1) - lose when discarded"));
	cs_scroll:addLabel(Label.new("Countess (1) - discard when with King or Prince"));
	cs_scroll:addLabel(Label.new("King (1) - trade hands"));
	cs_scroll:addLabel(Label.new("Prince (2) - discard card and draw new one"));
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
	--
	myCard = Label.new("myCard");
	myCard:setRect(10, 10,100,50);
	gui:rootWidget():addLabel(myCard);

	log('Lua setup done');
end


function resizeWindow()
end


function loop()
end


function updateHand()
	if(not game:isStarted()) then return; end
	local c = cards:toName(players:me():at('card'):get());
	if(not c) then c = '-'; end
	myCard:setText(c);
end


function sharedDataChange(line)
	server:update(line);
	game:update(line);
	players:update(line);

	if(startsWith(line, 'deck')) then
		cards:read();
	end

	if(startsWith(line, 'players\1')) then
		updateHand();
	end
end

