-- test.lua
function setupGui()
	log("setupGui")

	w = gui:rootWidget()

	b = Widget.new("assets/gui/box.png")
	b:setRect(100, 100, 320, 100)
	b:onClickLua(function() log("onclick") end)
	w:addWidget(b)

	l = Label.new("llaabbeell")
	l:setPosition(50,50)
	b:addLabel(l)


	c = Widget.new("assets/gui/box.png")
	c:setRect(100, 500, 320, 100)
	c:setColor(255, 0,0, 128)
	c:onClickLua(function()
			log("xxxxx");
			w:removeWidget(c)
		end)
	w:addWidget(c)

	lc = Label.new("Don't click me")
	c:addLabel(lc)
end

