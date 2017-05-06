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

#pragma once
#include <sol.hpp>
#include <functional>
#include <queue>
#include <vector>

namespace lua
{
	class Scheduler
	{
	public:
		Scheduler(sol::state& state, const double& time);
		~Scheduler();

	public:
		void Schedule(const sol::coroutine& rot);
		void Schedule(const std::function<void()>& rot);
		void RunQueue();

	private:
		struct ready_task
		{
			ready_task() = default;
			ready_task(const sol::coroutine& co);
			ready_task(const std::function<void()>& func);
			
			std::function<void()> func;
			sol::coroutine routine;
			bool isFunc;
		};

		struct sleeping_coroutine
		{
			double wake_time;
			sol::coroutine routine;
			bool operator<(const sleeping_coroutine& rhs)
			{
				return wake_time > rhs.wake_time;
			}
		};

		struct waiting_coroutine
		{
			sol::function wake_func;
			sol::coroutine routine;
		};

		std::queue<ready_task> m_ready;
		std::list<waiting_coroutine> m_waiting;
		std::vector<sleeping_coroutine> m_sleeping;
		sol::state& m_state;
		const double& m_time;
	};
}