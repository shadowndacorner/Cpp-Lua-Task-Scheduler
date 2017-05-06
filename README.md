# C++/Lua Task Scheduler using Sol2

This is a basic task scheduling library for lua using coroutines.  The scheduler itself is written in C++, but exposes a nice interface to lua to schedule function calls, which internally become coroutines.