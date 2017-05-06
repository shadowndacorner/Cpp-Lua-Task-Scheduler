# C++/Lua Task Scheduler using Sol2

This is a basic task scheduling library for lua using coroutines.  The scheduler itself is written in C++, but exposes a nice interface to lua to schedule function calls, which internally become coroutines.

## Dependencies

Requires sol2 and some version of lua.  The example application uses LuaJIT.

## Building for Windows
### Visual Studio 2017:
Run build_dependencies_win.bat under x86/x64 Native Tools Command Prompt for VS 2017 and open the solution in LuaTest.

## Example usage
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