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

global_co = coroutine.create(function()
		while(true) do
			s, res = xpcall(loop, debug.traceback);
			if(not s) then
				log(res);
			end
			coroutine.yield(); -- Give control to app to render next frame
		end
	end)

function main_loop()
	coroutine.resume(global_co);
end

