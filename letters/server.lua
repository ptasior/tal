Server = class(function(self)
		self.pendingTransactions = {};
		self.sw = sharedData.root():at('server');
		self.onConnect = {};
		self._connected = false;
	end)


function Server:update(line)
	-- On connection success
	if(startsWith(line, 'server\1me\1no')) then
		self._connected = true;
		self._meNo = self.sw:at('me'):at('no'):get();

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
		self.sw:at('transaction'):set('stop');
	end
end


function Server:addOnConnect(handler)
	if(self._connected) then error('Already connected'); end

	self.onConnect[#self.onConnect+1] = handler;
end


function Server:transaction(handle)
	self.sw:at('transaction'):set('start');
	self.pendingTransactions[#self.pendingTransactions+1] = handle;
end


function getClients()
	local ret = {};

	local cliNo = self.sw:at('clients'):get();
	if(cliNo == '') then return ret; end

	for i = 0, cliNo-1 do
		local addr = sharedData.root():at('server'):at('clients'):at(i):at('addr'):get();
		if(addr ~= '') then
			ret[cliNo] = addr;
		end
	end

	return ret;
end


function Server:meNo()
	return self._meNo;
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
			sharedData.print();
		end);
	self.widget['box']:addButton(self.widget['print']);

	self.widget['tstart'] = Button.new('Start transaction');
	self.widget['tstart']:onClickLua(function()
			sharedData.root():at('server'):at('transaction'):set('start');
		end);
	self.widget['box']:addButton(self.widget['tstart']);

	self.widget['tstop'] = Button.new('Finish transaction');
	self.widget['tstop']:onClickLua(function()
			sharedData.root():at('server'):at('transaction'):set('stop');
		end);
	self.widget['box']:addButton(self.widget['tstop']);

	self.widget['write'] = Button.new('write Something');
	self.widget['write']:onClickLua(function()
			local v = sharedData.root():at('test'):at('variable'):get();
			sharedData.root():at('test'):at('variable'):set(v+1);
		end);
	self.widget['box']:addButton(self.widget['write']);

	gui.rootWidget():addBox(self.widget['box']);
end

return Server

