#pragma once
// STD
#include <chrono>
#include <vector>
#include <iostream>

namespace troen
{
namespace util
{
	typedef std::chrono::high_resolution_clock clock;
	typedef clock::time_point time_point;
	
    //Abstract proxy class, which can be subclassed to pass in objects and call member functions
    class  TaskExecutor
    {
    public:
        virtual void taskFunction() = 0; //pure virtual
    };

	struct task
	{
		time_point t0;
		double duration_millis;
		void(*fPtr)(void *);
		void *args;
        //if executor is anything else than nullptr, 
        //its taskFunction will be called instead of fPtr
        TaskExecutor *executor; 
	};


	class CountdownTimer
	{
	public:
		CountdownTimer();

		virtual ~CountdownTimer();
		
		void update();
		void addTimer(int millis, void(*fPtr)(void *), void *args = nullptr);
        void addTimer(int millis, TaskExecutor *taskE);
	protected:

	protected:
		std::vector<task>	m_tasks;

		typedef std::chrono::duration<long double, std::milli> milli;

		time_point m_t0;
	};
} // end namespace util
} // end namespace troen