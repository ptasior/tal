-- test.lua
function setupGui()
	log("setupGui")

	w = gui.rootWidget()

	b = Widget.new("assets/gui/box.png")
	b:setRect(100, 30, 320, 100)
	b:onClickLua(function()
			w:removeWidget(b)
			log("onclick")
			b = nil -- TODO Find how to delete it
		end)

	l = Label.new("llaabbeell")
	l:setPosition(50,50)
	b:addLabel(l)
	w:addWidget(b)


	c = Widget.new("assets/gui/box.png")
	c:setRect(100, 500, 320, 100)
	c:setColor(255, 0,0, 128)
	c:onClickLua(function()
			log("xxxxx");
			w:removeWidget(c)
			c = nil -- TODO Find how to delete it
		end)

	lc = Label.new("Don't click me")
	c:addLabel(lc)
	w:addWidget(c)
end

function setupScene()
	log("setupScene")

	scene:getSkybox():init("assets/skybox.png")

	m = scene:getMap()
	m:setRect(-2, -2, 2, 2, -2, 0)
	m:init("assets/map/map2.png", "assets/grass.png")


	mat = Matrix.new()
	mat:scale(0.1)
	mat:translate(Glm_Vec3.new(0.0, 0, 0.0))

	m = ModelObj.new()
	m:init("assets/models/worker.obj")
	-- m:init("assets/models/lumberJack.obj")
	m:setPosition(mat:val());
	scene.addModel(m)


	mat = Matrix.new()
	mat:translate(Glm_Vec3.new(0.5, -0.4, 0.5))
	mat:scale(0.1)

	m = ModelObj.new()
	m:init("assets/models/worker.obj")
	-- m:init("assets/models/lumberJack.obj")
	m:setPosition(mat:val());
	scene.addModel(m)

	mat = Matrix.new()
	mat:translate(Glm_Vec3.new(0.0, 0.5, -2))
	mat:scaleVec(Glm_Vec3.new(0.5, 0.5, 0.5))

	s = Sprite.new()
	s:init("assets/penguin.png", "triangle")
	s:setPosition(mat:val());
	scene.addSprite(s)

end


function setupCamera()
	-- log("setupCamera")
end
