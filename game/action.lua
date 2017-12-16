Action = class(function(self, name)
		if(self:fileExists(name)) then
			log('Loading action: '..name);
			self:load(name);
		else
			log("Empty action: "..name);
			self.applyFnc = function() return false; end;
			self.executeFnc = function() end;
		end
		self.name = name;
	end)

function Action:fileExists(name)
	local list = require('game/actions/list');
	return inList(list, name);
end

function Action:load(name)
	local c = require('game/actions/'..name);
	for k,v in pairs(c) do
		self[k] = v;
	end
end
function Action:doesApply()
	return self.applyFnc(self);
end

function Action:execute()
	return self.executeFnc(self);
end

return Action

