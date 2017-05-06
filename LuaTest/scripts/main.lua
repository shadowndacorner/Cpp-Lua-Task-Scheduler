print("Loading main script")
local Tasks = require("include/task_scheduler.lua")

function sleep()
	local start = CurTime()
	print("Sleeping for 5 seconds")
	Tasks.Sleep(5)
	print("Coroutine finished sleeping from ", start)
end

function waitsleep()
	print("Sleeping for 5 seconds at ", CurTime())
	local endtime = CurTime() + 5
	Tasks.WaitFor(function()
		return CurTime() > endtime
	end)
	print("Coroutine finished sleeping, time is now ", CurTime())
end

Tasks.Schedule(sleep)
Tasks.Schedule(waitsleep)
