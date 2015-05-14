#include "countdowntimer.h"

using namespace troen::util;

CountdownTimer::CountdownTimer()
: m_t0(clock::now())
{
	m_tasks = std::vector<task>();
}

CountdownTimer::~CountdownTimer()
{
}

void CountdownTimer::update()
{
	auto taskIter = std::begin(m_tasks);
	while (taskIter != std::end(m_tasks))
	{
		const auto delta = clock::now() - taskIter->t0;
		auto elapsed = milli(delta).count();
		
		if (elapsed >= taskIter->duration_millis)
		{
            if (taskIter->executor == nullptr)
			    taskIter->fPtr(taskIter->args);
            else
                taskIter->executor->taskFunction();

			taskIter = m_tasks.erase(taskIter);
		}
		else
			++taskIter;
	}



}

//add a single shot timer, which will execute the supplied function with the arguments in a struct as void pointer (pthreads style)
void CountdownTimer::addTimer(int millis, void (*fPtr)(void *), void *args/*=nullptr*/)
{
	m_tasks.push_back(task{ clock::now(), (double)millis, fPtr, args, nullptr});
}

void CountdownTimer::addTimer(int millis, TaskExecutor *taskE)
{
    m_tasks.push_back(task{ clock::now(), (double)millis, nullptr, nullptr, taskE });
}