Hud = class(function(self)
		self.player = {};
		self:setup();
	end)

function Hud:update()
	local bg = self.player.bg;
	bg.widget:setRect(gui.getSceneWidth()-bg.w, gui.getSceneHeight()-bg.h, bg.w, bg.h);

	self.player.character:setText('Character: '..player.character);
	self.player.alignment:setText('Alignment: '..player.alignment);
	self.player.field:setText('Location: '..player.field);

	self.player.strength:setText('Strength: '..player.strength);
	self.player.craft:setText('Craft: '..player.craft);
	self.player.lives:setText('Lives: '..player.lives);
	self.player.gold:setText('Gold: '..player.gold);
end

function Hud:setup()
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

	self.player.strength = Label.new("strength");
	bg:addLabel(self.player.strength);

	self.player.craft = Label.new("craft");
	bg:addLabel(self.player.craft);

	self.player.lives = Label.new("lives");
	bg:addLabel(self.player.lives);

	self.player.gold = Label.new("gold");
	bg:addLabel(self.player.gold);

	self.player.objects = Button.new("Objects");
	self.player.objects:onClickLua(function()self:objectsClick();end);
	self.player.objects:setSize(200, 30);
	bg:addButton(self.player.objects);

	self.player.followers = Button.new("Followers");
	self.player.followers:onClickLua(function()self:followersClick();end);
	self.player.followers:setSize(200, 30);
	bg:addButton(self.player.followers);

	self:update();
end

function Hud:objectsClick()
	log('Objects click');
end

function Hud:followersClick()
	log('Followers click');
end

return Hud;

