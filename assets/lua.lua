-- test.lua
function setupGui()
	log("setupGui")

	w = gui.rootWidget()

	b = Widget.new("")
	b:setRect(400, 250, 80, 150)
	b:setColor(100,100,100, 200)
	b:setLayout(1)
	b:setOverflow(2)
	b:setCenter(true)

	c = Widget.new("")
	c:setRect(100, 50, 20, 60)
	c:setColor(255, 0,0, 255)
	c:setLayout(2)
	c:setOverflow(1)

	d = Widget.new("")
	d:setRect(100, 50, 20, 120)
	d:setColor(0, 255, 0, 255)
	d:setLayout(2)
	d:setOverflow(1)


	e = Widget.new("")
	e:setRect(100, 50, 20, 100)
	e:setColor(0, 0, 255, 255)
	e:setLayout(2)
	e:setOverflow(1)

	b:addWidget(c)
	b:addWidget(d)
	b:addWidget(e)
	w:addWidget(b)

	-- b = Widget.new("")
	-- b:setRect(100, 30, 220, 100)
	-- b:setColor(100,100,100, 200)
	-- b:setLayout(2)
	-- d:setOverflow(0)

	l = Button.new("llaabbeell")
	l:setRect(50,50, 90, 50)
	l:onClickLua(function()
			log("onclick")
			gui.rootWidget():removeButton(l)
			l = nil
		end)
	w:addButton(l)
	-- w:addWidget(b)


	c = Widget.new("")
	c:setRect(100, 500, 320, 100)
	c:setColor(255, 0,0, 128)
	c:onClickLua(function()
			log("xxxxx");
			b:removeWidget(d)
			d = nil -- TODO Find how to delete it
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
