#pragma once
#include <sol.hpp>
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
		void Schedule(sol::coroutine);
		void RunQueue();

	private:
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

		std::queue<sol::coroutine> m_ready;
		std::list<waiting_coroutine> m_waiting;
		std::vector<sleeping_coroutine> m_sleeping;
		sol::state& m_state;
		const double& m_time;
	};
}