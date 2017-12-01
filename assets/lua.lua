-- test.lua
function setupGui()
	log("setupGui")

	w = gui:rootWidget()
	
	b = Box.new()
	b:setRect(100, 100, 320, 100)
	w:addBox(b)

	l = Label.new("llaabbeell")
	l:setPosition(50,50)
	b:addLabel(l)
end

