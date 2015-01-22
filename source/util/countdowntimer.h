#pragma once
// STD
#include <chrono>
#include <vector>

namespace troen
{
namespace util
{
	typedef std::chrono::high_resolution_clock clock;
	typedef clock::time_point time_point;
	
	struct task
	{
		time_point t0;
		double duration_millis;
		void(*fPtr)(void *);
		void *args;
	};

	class CountdownTimer
	{
	public:
		CountdownTimer();

		virtual ~CountdownTimer();
		
		void update();
		void addTimer(int millis, void(*fPtr)(void *), void *args = nullptr);
	protected:

	protected:
		std::vector<task>	m_tasks;

		typedef std::chrono::duration<long double, std::milli> milli;

		time_point m_t0;
	};
} // end namespace util
} // end namespace troen