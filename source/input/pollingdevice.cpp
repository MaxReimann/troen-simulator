#include "pollingdevice.h"
// troen
#include "bikeinputstate.h"

using namespace troen::input;

PollingDevice::PollingDevice(osg::ref_ptr<BikeInputState> bikeInputState) : mPause(false)
{
	m_bikeInputState = bikeInputState;
}

void PollingDevice::run()
{
	// subclass responsibility
}

void PollingDevice::stop(){
	m_pollingEnabled = false;
}

void PollingDevice::resume()
{
	mSync.lock();
	mPause = false;
	mSync.unlock();
	mPauseCond.wakeAll();
}

void PollingDevice::pause()
{
	mSync.lock();
	mPause = true;
	mSync.unlock();
}

void PollingDevice::waitIfPaused()
{
	mSync.lock();
	if (mPause)
	{
		mPauseCond.wait(&mSync);
		std::cout << "exited wait in polling device" << std::endl;

	}
	mSync.unlock();
}

void PollingDevice::setBikeInputState(osg::ref_ptr<BikeInputState> bikeInputState)
{
	m_bikeInputState.release();
	m_bikeInputState = bikeInputState;
}