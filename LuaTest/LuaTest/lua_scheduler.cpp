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

void lua::Scheduler::Schedule(sol::coroutine coroutine)
{
	m_ready.push(coroutine);
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
		std::pair<bool, sol::table> ret = m_state["coroutine"]["resume"](task);
		m_ready.pop();
		std::string st = m_state["coroutine"]["status"](task);
		if (st == "dead")
			continue;

		sol::table result = ret.second;
		if (result.valid())
		{
			double time = result.get_or("wake_time", 0);
			if (time > 0)
			{
				sleeping_coroutine sleep;
				sleep.routine = task;
				sleep.wake_time = m_time + time;

				m_sleeping.push_back(sleep);
				std::sort(m_sleeping.begin(), m_sleeping.end());
			}
			else if (result["wake_func"].valid())
			{
				sol::function wake_func = result["wake_func"];
				waiting_coroutine sleep;
				sleep.routine = task;
				sleep.wake_func = wake_func;

				m_waiting.push_back(sleep);
			}
			else
				Schedule(task);
		}
		else
			Schedule(task);
	}
}
