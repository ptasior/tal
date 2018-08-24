Hud = class(function(self)
		self.player = {};
		self.journal = {};
		self:setup();
	end)

function Hud:update()
	local pl = self.player.bg;
	pl.widget:setRect(gui.getSceneWidth()-pl.w, gui.getSceneHeight()-pl.h, pl.w, pl.h);

	self.player.character:setText('Character: '..player.character);
	self.player.alignment:setText('Alignment: '..player.alignment);
	local f = board:getField(player.field).label;
	self.player.field:setText('Location: '..f);

	self.player.strength:setText('Strength: '..player.strength);
	self.player.craft:setText('Craft: '..player.craft);
	self.player.lives:setText('Lives: '..player.lives);
	self.player.gold:setText('Gold: '..player.gold);

	-----------------------

	local jr = self.journal.bg;
	jr.widget:setRect(0, gui.getSceneHeight()-jr.h, jr.w, jr.h);

	for i = math.min(self.journal.linesCount, #journal.entries), 1, -1 do
		ln = journal.entries[#journal.entries-i+1];
		self.journal.labels[i]:setText(ln);
	end
end

function Hud:setupPlayer()
	self.player.bg = {};
	self.player.bg.w = 200;
	self.player.bg.h = 190;

	local bg = Widget.new("");
	bg:setColor(100,100,100,150);
	bg:setLayout(2);
	bg:setOverflow(1);
	bg:setStretch(false);

	self.player.bg.widget = bg;
	gui.rootWidget():addWidget(self.player.bg.widget);

	self.player.character = Label.new("Character");
	bg:addLabel(self.player.character);

	self.player.alignment = Label.new("Alignment");
	bg:addLabel(self.player.alignment);

	self.player.field = Label.new("Field");
	bg:addLabel(self.player.field);

	self.player.strength = Label.new("Strength");
	bg:addLabel(self.player.strength);

	self.player.craft = Label.new("Craft");
	bg:addLabel(self.player.craft);

	self.player.lives = Label.new("Lives");
	bg:addLabel(self.player.lives);

	self.player.gold = Label.new("Gold");
	bg:addLabel(self.player.gold);

	self.player.objects = Button.new("Objects");
	self.player.objects:onClickLua(function()self:objectsClick();end);
	self.player.objects:setSize(200, 30);
	bg:addButton(self.player.objects);

	self.player.followers = Button.new("Followers");
	self.player.followers:onClickLua(function()self:followersClick();end);
	self.player.followers:setSize(200, 30);
	bg:addButton(self.player.followers);
end

function Hud:setupJournal()
	self.journal.bg = {};
	self.journal.bg.w = 400;
	self.journal.linesCount = 15;
	self.journal.bg.h = self.journal.linesCount*18;

	local bg = Widget.new("");
	bg:setColor(100,100,100,150);
	bg:setLayout(2);
	bg:setOverflow(1);
	bg:setStretch(false);

	self.journal.bg.widget = bg;
	gui.rootWidget():addWidget(self.journal.bg.widget);

	self.journal.labels = {};
	for i = self.journal.linesCount, 1, -1 do
		self.journal.labels[i] = Label.new("---");
		bg:addLabel(self.journal.labels[i]);
	end
end

function Hud:setup()
	self:setupPlayer()
	self:setupJournal()

	self:update();
end

function Hud:objectsClick()
	log('Objects click');
end

function Hud:followersClick()
	log('Followers click');
end

return Hud;

