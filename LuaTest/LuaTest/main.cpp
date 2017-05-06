#include <iostream>
//#define SOL_USING_CXX_LUA
#include <lua.hpp>
#include <sol.hpp>
#include "lua_scheduler.h"

static bool* running;
void exitfunc()
{
	*running = false;
	std::cout << "Press return to exit program\n";
}

void print_error(lua_State* state) {
	// The error message is on top of the stack.
	// Fetch it, print it and then pop it off the stack.
	const char* message = lua_tostring(state, -1);
	std::cout << message << std::endl;
	lua_pop(state, 1);
}

void reload(sol::state& lua)
{
	try
	{
		lua.script_file("../scripts/main.lua", sol::default_on_error);
	}
	catch (sol::error& err)
	{
		std::cerr << err.what() << std::endl;
	}
}

#include <thread>
#include <mutex>
#include <chrono>
static std::mutex script_mutex;
static std::queue<std::string> scriptsToRun;
void run_thread()
{
	char buf[512];
	while (*running)
	{
		std::cin.getline(buf, 512);
		if (!running)
			return;

		std::lock_guard<std::mutex> lock(script_mutex);
		scriptsToRun.push(buf);
	}
}

void main(int argc, char** argv)
{
	using namespace std::chrono;
	high_resolution_clock::time_point start = high_resolution_clock::now();
	
	bool m_running=true;
	running = &m_running;

	std::thread input(run_thread);
	double time = 0;
	double deltaTime = 0.016;
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::jit, sol::lib::package);
	
	// Add our scripts folder to the path
	{
		std::string path = lua["package"]["path"];
		path = path + ";../scripts/?.lua";
		lua["package"]["path"] = path;
	}

	lua::Scheduler sched(lua, time);
	lua.set_function("exit", exitfunc);
	lua.set_function("reload", [&lua]() {
		reload(lua);
	});

	lua.set_function("CurTime", [&time]
	{
		return time;
	});

	lua.set_function("FrameTime", [&deltaTime]
	{
		return deltaTime;
	});
	
	// Load main.lua
	reload(lua);

	high_resolution_clock::time_point last;
	while (m_running)
	{
		high_resolution_clock::time_point cur = high_resolution_clock::now();
		time = ((double)duration_cast<nanoseconds>((cur - start)).count()) / 1000000000.0;
		deltaTime = ((double)duration_cast<nanoseconds>((cur - last)).count()) / 1000000000.0;
		last = cur;

		if (scriptsToRun.size() > 0)
		{
			std::lock_guard<std::mutex> lock(script_mutex);
			while (scriptsToRun.size() > 0 && m_running)
			{
				try
				{
					auto code = scriptsToRun.front();
					scriptsToRun.pop();
					lua.script(code, sol::default_on_error);
				}
				catch (sol::error& err)
				{
					std::cerr << "Lua error: " << err.what() << std::endl;
				}
			}
		}
		sched.RunQueue();
		lua.collect_garbage();
	}

	if (input.joinable())
		input.join();
}