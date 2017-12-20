GuiHelpers = class(function(self)
	end)

function GuiHelpers:message(label, handler)
	local bx = ButtonBox.new("Message");
	bx:setRect(200, 300, 320, 100);

	bx:addLabel(Label.new(label));

	local l = Button.new('OK');
	l:onClickLua(function()
			gui.rootWidget():removeButtonBox(bx);
			bx = nil;
			if handler then
				handler();
			end
			appContinue();
		end)
	bx:addBottomButton(l);

	gui.rootWidget():addButtonBox(bx);
	appWait();
end

function GuiHelpers:askQuestion(label, options, handler)
	local bx = ButtonBox.new("Question");
	bx:setRect(200, 300, 380, 100);

	bx:addLabel(Label.new(label));

	for i,o in ipairs(options) do
		local l = Button.new(o);
		l:onClickLua(function()
				gui.rootWidget():removeButtonBox(bx);
				bx = nil;
				handler(o);
				appContinue();
			end)
		bx:addBottomButton(l);
	end

	gui.rootWidget():addButtonBox(bx);
	appWait();
end

randomButton_t = 50;
function GuiHelpers:randomButton(label, handler)
	local b = Button.new(label)
	b:setRect(50, randomButton_t, 80, 30);
	b:onClickLua(handler)
	gui.rootWidget():addButton(b);
	randomButton_t = randomButton_t + 35;
	return b;
end


return GuiHelpers

