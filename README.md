# C++/Lua Task Scheduler using Sol2

This is a basic task scheduling library for lua using coroutines.  The scheduler itself is written in C++, but exposes a nice interface to lua to schedule function calls, which internally become coroutines.

## Dependencies

Requires sol2 and some version of lua.  The example application uses LuaJIT.

## Building for Windows
### Visual Studio 2017:
Run build_dependencies_win.bat under x86/x64 Native Tools Command Prompt for VS 2017 and open the solution in LuaTest.

## Example usage
#### C++ application
```c++
// game time double, for sleep
double time = 0;

sol::state lua;
lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::jit, sol::lib::package);

lua::Scheduler sched(lua, time);

...
// In game loop
sched.RunQueue();

```

#### Lua script
```lua
local Tasks = require("task_scheduler")

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
```

Output:
```
Sleeping for 5 seconds
Sleeping for 5 seconds at       0.003233071
Coroutine finished sleeping from        0.003233071
Coroutine finished sleeping, time is now        5.00325006
```
