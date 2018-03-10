class = require('lua_lib/class')
var_dump = require('lua_lib/var_dump')
require('lua_lib/functions')

GuiHelpers = require('game/gui_helpers')
Cards = require('game/cards')
Game = require('game/game')
Players = require('game/players')
Server = require('game/server')

server = nil;
players = nil;
game = nil;


function initCards()
	local names = {"Guard", "Priest", "Baron", "Handmaid",
					"Prince", "King", "Countess", "Princess"};
	local set = {8,7,6,5,5,4,4,3,3,2,2,1,1,1,1,1};

	cards = Cards(sharedData:root():at('deck'));
	cards:set(set, names);
end


function initGame()
	log('Init game');

	cards:shuffle();
	cards:save();

	local ap = players:getNames();

	for i = 1,#ap do
		local c = cards:drawOne();
		players:get(ap[i]):at('card'):set(c);
	end

	cards:save();
	local turn = game:initTurn();

	server:broadcast(turn.."'s turn");

	updateHand();
end


function startGame()
	log('Start game');
	players:toggleStartButton();
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
	local msg = players.meName..' played '..cards:toName(card)..'\n';

	if(card == 1) then
		local all_players = playersToPick(false);
		if(#all_players == 0) then
			GuiHelpers:message('All players are protected');
			return msg..'but all players were protected';
		end

		local pl = GuiHelpers:selectFrom("Pick a player", all_players);
		local crd = GuiHelpers:selectFrom("Guess the card",
											without(cards.names, 'Guard'));
		if(players:get(pl):at('card'):get() == tostring(cards:toNumber(crd))) then
			players:lose(pl);
			server:broadcast(pl..' has lost');
			return msg..pl..' had '..crd..' and lost the round';
		else
			GuiHelpers:message('No!');
			return msg..'but '..pl..' does not have '..crd;
		end

	elseif(card == 2) then
		local all_players = playersToPick(false);
		if(#all_players == 0) then
			GuiHelpers:message('All players are protected');
			return msg..'but all players were protected';
		end

		local pl = GuiHelpers:selectFrom("Pick a player", all_players);
		GuiHelpers:message(pl..' has '..cards:toName(players:get(pl):at('card'):get()));
		return msg..'and now knows '..pl..'\'s card';

	elseif(card == 3) then
		local all_players = playersToPick(false);
		if(#all_players == 0) then
			GuiHelpers:message('All players are protected');
			return msg..'but all players were protected';
		end

		local pl = GuiHelpers:selectFrom("Pick a player", all_players);
		local mc = players:me():at('card'):get();
		local pc = players:get(pl):at('card'):get();
		if(pc == mc) then
			GuiHelpers:message(pl..' has the same card');
			return msg..pl..'\'s and '..players.meName..'\'s cards are identical';
		elseif(pc > mc) then
			players:lose(players.meName);
			return msg..players.meName..' has lost';
		elseif(pc < mc) then
			players:lose(pl);
			return msg..pl..' has lost';
		end

	elseif(card == 4) then
		players:get(players.meName):at('protected'):set('true');
		return msg..' and is protected';

	elseif(card == 5) then
		local all_players = playersToPick(false);
		if(#all_players == 0) then
			GuiHelpers:message('All players are protected');
			return msg..'but all players were protected';
		end

		local pl = GuiHelpers:selectFrom("Pick a player", all_players);
		local c = cards:drawOne();
		players:get(pl):at('card'):set(c);
		cards:save();
		return msg..pl..' was chosen to draw new cards';

	elseif(card == 6) then
		local all_players = playersToPick(false);
		if(#all_players == 0) then
			GuiHelpers:message('All players are protected');
			return msg..'but all players were protected';
		end

		local pl = GuiHelpers:selectFrom("Pick a player", all_players);
		local tmp = players:get(pl):at('card'):get();
		players:get(pl):at('card'):set(players:me():at('card'):get());
		players:me():at('card'):set(tmp)
		return msg..pl..' and '..players.meName..' traded hands';

	elseif(card == 7) then
		return msg..' nothing happens';

	elseif(card == 8) then
		players:lose(players.meName);
		return msg..' and lost';
	
	else
		log('Perform: No such card'..card);

	end
end


function isGameOver()
	local playing = players:getPlaying();
	if(#playing > 1 and #cards.deck > 0) then
		return false;
	end

	log('GAME OVER!');

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

	-- Inform players
	for i = 1,#playing do
		if(playing[i] ~= m_n) then
			players:lose(playing[i]);
		end
	end

	-- Clean cards
	local pl = players:getNames();
	for i = 1,#pl do
		players:get(pl[i]):at('card'):set('');
	end
	game:finishGame();

	server:broadcast("End of the game, "..m_n..' won');
	players:toggleStartButton();

	return true;
end


function playTurn()
	log('My Turn!')
	-- server:broadcast(players.meName..'\'s turn');
	players:me():at('protected'):set('false');

	-- Draw a card
	drawnCardValue = sharedData:root():at('drawnCard'):get();

	if(drawnCardValue == '' or drawnCardValue == nil) then -- True unless resuming game
		drawnCardValue = cards:drawOne();
		cards:save();
		sharedData:root():at('drawnCard'):set(drawnCardValue);
	end

	-- Display it
	updateHand();

	if(drawnCardValue) then -- When resuming game
		resizeWindow(); -- Set drawnCard position
	end
end


function cardSelected(card)
	if(not game:isMyTurn()) then return; end

	sharedData:root():at('drawnCard'):set('');

	local hand = players:me():at('card'):get();
	local played = nil;
	if(card == 'hand') then
		players:me():at('card'):set(drawnCardValue);
		played = tonumber(hand);
	else
		players:me():at('card'):set(hand);
		played = tonumber(drawnCardValue);
	end

	drawnCardValue = nil;
	log('played = '..var_dump(played))

	local message = perform(played);

	if(not isGameOver()) then
		message = message.."\n"..game:whoseTurnNext().."'s turn"
		server:broadcast(message);
		game:nextTurn();
	end
end


function gameLost()
	players:me():at('card'):set('');
	GuiHelpers:message("You've lost the round");
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


function updateHand()
	myCard:setVisible(false);
	drawnCard:setVisible(false);

	if(not game:isStarted()) then return; end
	local c = tostring(players:me():at('card'):get());

	if(c and c ~= '') then
		myCard:setVisible(true);
		myCard:setTexture('game/assets/c'..c..'.png');
	end

	if(drawnCardValue and drawnCardValue ~= '') then
		drawnCard:setVisible(true);
		drawnCard:setTexture('game/assets/c'..drawnCardValue..'.png');
	end
end


function sharedDataUpdate(line)
	if(startsWith(line, 'deck')) then
		cards:read();
	end

	if(startsWith(line, 'players\1')) then
		if(players.meName) then
			-- myName:setText(players.meName);
			updateHand();
		end
		updatePlayersWidget();
	end

	if(startsWith(line, 'game\1started')) then
		players:toggleStartButton();
	end
end


function setup()
	math.randomseed(os.time());
	main_addOnSharedDataUpdate(sharedDataUpdate);

	-- Temporarily
	setLoopResolution(500);

	server = Server();
	server:addOnBroadcast(function(txt) statusLabel:setText(txt); end);
	-- server:showWindow();

	game = Game();
	game:addOnInit(initGame);
	game:addOnStart(startGame);
	game:addOnTurn(playTurn);
	game:addOnLost(gameLost)

	players = Players();
	players:showWidget();

	initCards();
	-- showCheatsheet();

	myCard = Widget.new('');
	myCard:setRect(10, 50,200,300);
	myCard:onClickLua(function() execute(function() cardSelected('hand'); end) end);
	gui:rootWidget():addWidget(myCard);

	drawnCard = Widget.new('');
	drawnCard:setRect(300, 50,200,300);
	drawnCard:onClickLua(function() execute(function() cardSelected('drawn'); end) end);
	gui:rootWidget():addWidget(drawnCard);

	myName = Label.new('');
		myName:setRect(5, 0, 100, 80);
	gui:rootWidget():addLabel(myName);

	statusBar = Widget.new('');
	statusBar:setTexture('game/assets/banner.png');
	statusBar:setLayout(2);
	statusBar:setCenter(true);
	statusBar:setPadding(0, 30);
	statusLabel = MultiLine.new('');
	statusBar:addMultiLine(statusLabel);
	gui:rootWidget():addWidget(statusBar);

	gui:rootWidget():setTexture("game/assets/bg.png");
	gui:rootWidget():setTextureRepeat(3,3);

	updateHand();
	updatePlayersWidget();
	resizeWindow();

	log('Lua setup done');
end


function updatePlayersWidget()
	if not players.meName then
		return;
	end

	local pl = players:getNames();
	local str = "me: "..players.meName.." - "
	for i=1, #pl do
		str = str..pl[i]..', ';
	end

	myName:setText(str);
end


function resizeWindow()
	local width = gui:getSceneWidth();
	local height = gui:getSceneHeight();
	statusBar:setRect(10, height-120, width-10, 120);
	statusLabel:setSize(math.floor(width*2/3), 70);

	local cardHeight = math.floor(height/2);
	local cardWidth = math.floor(cardHeight*2/3);
	local cardSpace = math.floor(width/20);

	if(drawnCardValue and drawnCardValue ~= '') then
		myCard:setRect(math.floor(width/2-cardWidth-cardSpace), math.floor(height/4),
						cardWidth, cardHeight);
		drawnCard:setRect( math.floor(width/2+cardSpace), math.floor(height/4),
						cardWidth, cardHeight);
	else
		myCard:setRect(math.floor((width-cardWidth)/2), math.floor(height/4),
						cardWidth, cardHeight);
	end
end


function loop()
end

