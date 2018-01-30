-- buffer for holding shared data changes between class handler and Lua loop
--  Used for enabling yields when handling changes
global_sharedDataBuffer = {};

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

global_co = coroutine.create(function()
		while(true) do
			s, res = xpcall(function()
					for i=1,#global_sharedDataBuffer do
						local l = table.remove(global_sharedDataBuffer,1);
						sharedDataChange(l);
					end
					loop();
				end, debug.traceback);

			if(not s) then
				log(res);
			end
			coroutine.yield(); -- Give control to app to render next frame
		end
	end)

function main_loop()
	coroutine.resume(global_co);
end

-- To enable yield when handling shared data changes, push onto a list and process in loop
function sharedDataUpdated(line)
	global_sharedDataBuffer[#global_sharedDataBuffer+1] = line;
end

