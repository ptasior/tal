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
end

