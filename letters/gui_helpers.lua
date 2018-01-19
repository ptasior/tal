GuiHelpers = class(function(self)
	end)

function GuiHelpers:input(question)
	local bx = ButtonBox.new("Question");
	bx:setRect(200, 300, 320, 100);

	local ml = MultiLine.new(question);
	ml:resize();

	local edit = Edit.new('');

	bx:setRect(200, 300, 20+ml:getWidth(), 95+ ml:getHeight());
	bx:addMultiLine(ml);
	bx:addEdit(edit);

	local text = '';

	local l = Button.new('OK');
	l:onClickLua(function()
			gui.rootWidget():removeButtonBox(bx);
			bx = nil;
			text = edit:getText();
			appContinue();
		end)
	bx:addBottomButton(l);

	gui.rootWidget():addButtonBox(bx);
	appWait();

	return text
end

function GuiHelpers:message(label, handler)
	local bx = ButtonBox.new("Message");
	bx:setRect(200, 300, 320, 100);

	local ml = MultiLine.new(label);
	ml:resize();

	bx:setRect(200, 300, 20+ml:getWidth(), 80+ ml:getHeight());
	bx:addMultiLine(ml);

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

function GuiHelpers:askQuestion(label, options)
	local bx = ButtonBox.new("Question");

	local ml = MultiLine.new(label);
	ml:resize();

	bx:setRect(200, 300, 20+ml:getWidth(), 80+ ml:getHeight());
	bx:addMultiLine(ml);

	local ans = nil;

	for i,o in ipairs(options) do
		local l = Button.new(o);
		l:onClickLua(function()
				gui.rootWidget():removeButtonBox(bx);
				bx = nil;
				ans = o;
				appContinue();
			end)
		bx:addBottomButton(l);
	end

	gui.rootWidget():addButtonBox(bx);
	appWait();

	return ans;
end

function GuiHelpers:selectFrom(label, choices, answers)
	local bx = ButtonBox.new("Select");

	local ml = MultiLine.new(label);
	ml:resize();

	bx:setRect(200, 300, 20+ml:getWidth(), 80+ ml:getHeight());
	bx:addMultiLine(ml);


	for i,o in ipairs(choices) do
		local l = Button.new(o);
		l:onClickLua(function()
			end)
		bx:addButton(l);
	end


	local ans = nil;

	for i,o in ipairs(answers) do
		local l = Button.new(o);
		l:onClickLua(function()
				gui.rootWidget():removeButtonBox(bx);
				bx = nil;
				ans = o;
				appContinue();
			end)
		bx:addBottomButton(l);
	end

	gui.rootWidget():addButtonBox(bx);
	appWait();

	return ans, {};
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

