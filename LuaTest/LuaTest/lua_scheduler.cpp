/*
	Copyright (c) 2017 Ian Diaz

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "lua_scheduler.h"

lua::Scheduler::Scheduler(sol::state & state, const double& time) : m_state(state), m_time(time){
	sol::table schedtab = m_state.create_named_table("_InternalTasks");
	m_state["_InternalTasks"]["InternalQueueTask"] = [this](sol::coroutine co)
	{
		this->Schedule(co);
	};

	m_state["_InternalTasks"]["InternalProcessQueue"] = [this]()
	{
		this->RunQueue();
	};
}

lua::Scheduler::~Scheduler()
{
	RunQueue();
}

void lua::Scheduler::Schedule(const sol::coroutine& coroutine)
{
	m_ready.push(ready_task(coroutine));
}

void lua::Scheduler::Schedule(const std::function<void()>& func)
{
	m_ready.push(ready_task(func));
}

void lua::Scheduler::RunQueue()
{
	// Check sleeping tasks -- these have priority over waiting tasks
	while (m_sleeping.size() > 0 && m_sleeping.back().wake_time < m_time)
	{
		m_ready.push(m_sleeping.back().routine);
		m_sleeping.pop_back();
	}

	// Check if any waiting tasks should return
	for (auto it = m_waiting.begin(); it != m_waiting.end();)
	{
		bool pass = false;
		if ((pass = it->wake_func()))
		{
			m_ready.push(it->routine);
			auto rem = it;
			++it;
			m_waiting.erase(rem);
		}
		else
			++it;
	}

	// There are tasks ready to run
	while (m_ready.size() > 0)
	{
		auto task = m_ready.front();
		if (task.isFunc)
		{
			// Run it, it's a one-off
			task.func();
		}
		else
		{
			std::pair<bool, sol::table> ret = m_state["coroutine"]["resume"](task.routine);
			m_ready.pop();
			std::string st = m_state["coroutine"]["status"](task.routine);
			if (st == "dead")
				continue;

			sol::table result = ret.second;
			if (result.valid())
			{
				double time = result.get_or("wake_time", 0);
				if (time > 0)
				{
					sleeping_coroutine sleep;
					sleep.routine = task.routine;
					sleep.wake_time = m_time + time;

					m_sleeping.push_back(sleep);
					std::sort(m_sleeping.begin(), m_sleeping.end());
				}
				else if (result["wake_func"].valid())
				{
					sol::function wake_func = result["wake_func"];
					waiting_coroutine sleep;
					sleep.routine = task.routine;
					sleep.wake_func = wake_func;

					m_waiting.push_back(sleep);
				}
				else
					Schedule(task.routine);
			}
			else
				Schedule(task.routine);
		}
	}
}

lua::Scheduler::ready_task::ready_task(const sol::coroutine & co) : routine(co), isFunc(false){}
lua::Scheduler::ready_task::ready_task(const std::function<void()>& f) : func(f), isFunc(true){}
