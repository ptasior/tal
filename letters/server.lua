Server = class(function(self)
		self.pendingTransactions = {};
		self.sw = sharedData:root():at('server');
		self.onConnect = {};
		self._connected = false;
		self.onBroadcast = {};
	end)


function Server:update(line)
	-- On connection success
	if(startsWith(line, 'server\1me\1no')) then
		self._connected = true;
		self._meNo = tonumber(self.sw:at('me'):at('no'):get());

		for k,v in pairs(self.onConnect) do
			v(); -- Call all handelrs
		end
	end

	-- On transaction granted
	if(startsWith(line, 'server\1me\1transaction\1true')) then
		print('Transaction granted')
		-- Execute all handlers
		for k,v in pairs(self.pendingTransactions) do
			v();
		end
		self.pendingTransactions = {}; -- Remove handlers
		self.sw:at('transaction'):set('stop');
	end


	if(startsWith(line, 'server\1broadcast')) then
		local msg = self.sw:at('broadcast'):get();

		if(#self.onBroadcast > 0) then
			for i=1,#self.onBroadcast do
				self.onBroadcast[i](msg);
			end
		else
			GuiHelpers:message(msg);
		end
	end
end


function Server:addOnConnect(handler)
	if(self._connected) then error('Already connected'); end

	self.onConnect[#self.onConnect+1] = handler;
end


function Server:transaction(handle)
	if(self.sw:at('me'):at('transaction'):get() ~= 'true') then
		self.sw:at('transaction'):set('start');
		-- error('A transaction is already started');
	end
	self.sw:at('transaction'):set('start');
	self.pendingTransactions[#self.pendingTransactions+1] = handle;
end


function Server:getClients()
	local ret = {};

	local cliNo = tonumber(self.sw:at('clients'):get());
	if(cliNo == '') then return ret; end

	for i = 0, cliNo-1 do
		local addr = self.sw:at('clients'):at(tostring(i)):at('addr'):get();
		if(addr ~= '') then
			ret[i] = addr;
		end
	end

	return ret;
end


function Server:meNo()
	return self._meNo;
end

function Server:addOnBroadcast(handler)
	self.onBroadcast[#self.onBroadcast+1] = handler;
end

function Server:broadcast(msg)
	self.sw:at('broadcast'):set(msg);
end


function Server:showWindow()
	self.widget = {};

	self.widget['box'] = Box.new("Server");
	self.widget['box']:setPosition(40,140);
	-- self.widget['box']:setLayout(0);
	-- self.widget['box']:setCenter(false);
	self.widget['box']:setPadding(3,3);
	self.widget['box']:setSize(306,220);


	self.widget['print'] = Button.new('Print tree');
	self.widget['print']:onClickLua(function()
			sharedData:print();
		end);
	self.widget['box']:addButton(self.widget['print']);

	self.widget['tstart'] = Button.new('Start transaction');
	self.widget['tstart']:onClickLua(function()
			sharedData:root():at('server'):at('transaction'):set('start');
		end);
	self.widget['box']:addButton(self.widget['tstart']);

	self.widget['tstop'] = Button.new('Finish transaction');
	self.widget['tstop']:onClickLua(function()
			sharedData:root():at('server'):at('transaction'):set('stop');
		end);
	self.widget['box']:addButton(self.widget['tstop']);

	self.widget['write'] = Button.new('Message');
	self.widget['write']:onClickLua(function()
			-- local v = sharedData:root():at('test'):at('variable'):get();
			-- sharedData:root():at('test'):at('variable'):set(v);
			execute(function() GuiHelpers:message('qqrq'); end);
		end);
	self.widget['box']:addButton(self.widget['write']);

	gui:rootWidget():addBox(self.widget['box']);
end

return Server

