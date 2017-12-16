class = require('lua_lib/class')

GuiHelpers = class(function(self)
	end)

function GuiHelpers:message(label, handler)
	gameState.waitUserInput = true;

	local bx = ButtonBox.new("Message");
	bx:setRect(200, 300, 320, 100);

	bx:addLabel(Label.new(label));

	local l = Button.new('OK');
	l:onClickLua(function()
			gui.rootWidget():removeButtonBox(bx);
			gameState.waitUserInput = false;
			bx = nil;
			if handler then
				handler();
			end
		end)
	bx:addBottomButton(l);

	gui.rootWidget():addButtonBox(bx);
end

function GuiHelpers:askQuestion(label, options, handler)
	gameState.waitUserInput = true;

	local bx = ButtonBox.new("Question");
	bx:setRect(200, 300, 320, 100);

	bx:addLabel(Label.new(label));

	for i,o in ipairs(options) do
		local l = Button.new(o);
		l:onClickLua(function()
				gui.rootWidget():removeButtonBox(bx);
				gameState.waitUserInput = false;
				handler(o);
				bx = nil;
			end)
		bx:addBottomButton(l);
	end

	gui.rootWidget():addButtonBox(bx);
end

function GuiHelpers:randomButton(label, handler)
	local b = Button.new(label)
	b:setRect(50, 50, 80, 30);
	b:onClickLua(handler)
	gui.rootWidget():addButton(b);
end


return GuiHelpers

