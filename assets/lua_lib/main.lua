-- buffer for holding shared data changes between class handler and Lua loop
--  Used for enabling yields when handling changes
global_sharedDataBuffer = {};
global_sharedDataListeners = {};

function appContinue()
	setWait(0); -- C++, wsRun
end

function appRefresh()
	setWait(2); -- C++, wsRefresh
	coroutine.yield();
end

function appWait()
	setWait(1); -- C++, wsWait
	coroutine.yield();
end

function appSleep(ms)
	setTimeout(ms);
	setWait(3); -- C++, wsSleep
	coroutine.yield();
end

function waitFor(s_data)
	local v = s_data:get();
	while(v == '') do
		appWait(100);
		v = s_data:get();
	end
	return v;
end


function main_addOnSharedDataUpdate(handler)
	global_sharedDataListeners[#global_sharedDataListeners+1] = handler;
end


function main_sharedDataUpdate(line)
	for i=1,#global_sharedDataListeners do
		global_sharedDataListeners[i](line);
	end

	global_sharedDataListeners = {};
end

function main_executeAwaiting()
	updateAwaitingExecution();

	for i=1,#global_toExecuteStrings do
		local c = global_toExecuteStrings[i]:gsub("\n","\\n");
		load(c)();
	end

	for i=1,#global_toExecuteLua do
		global_toExecuteLua[i]();
	end

	for i=1,#global_toExecute do
		global_toExecute[i]();
	end

	global_toExecuteStrings = {};
	global_toExecuteLua = {};
	global_toExecute = {};
end

main_coroutine = coroutine.create(function()
		while(true) do
			s, res = xpcall(function()
					main_executeAwaiting();
					loop();
				end, debug.traceback);

			if(not s) then
				log(res);
			end
			coroutine.yield(); -- Give control to app to render next frame
		end
	end)

function main_loop()
	coroutine.resume(main_coroutine);
end

