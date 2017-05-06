--[[
	Lua wrapper for C++ task scheduler functionality
]]--

local Tasks = {}
local task_internal = _InternalTasks

_InternalTasks=nil
function Tasks.Schedule(func)
	if func and type(func)=="function" then
		task_internal.InternalQueueTask(coroutine.create(func))
	end
end

function Tasks.Sleep(t)
	local ret = {}
	ret.wake_time = t;
	coroutine.yield(ret)
end

function Tasks.Yield()
	coroutine.yield()
end

function Tasks.WaitFor(f)
	local ret = {}
	ret.wake_func = f;
	coroutine.yield(ret)
end
return Tasks;