#pragma once
// OSG
#include <osg/ref_ptr>
// Qt
#include <QThread>
// troen
#include "../forwarddeclarations.h"

namespace troen
{
	namespace input
	{

		enum class InputDevice : unsigned int
		{
			KEYBOARD_wasd, KEYBOARD_arrows, GAMEPAD, GAMEPADPS4, FFBWHEEL
		};

		/*! The PollingDevice is an abstract class which inherits from QThread. It is used as the base class for controller input and AI.*/
		class PollingDevice : public QThread
		{
		public:
			PollingDevice(osg::ref_ptr<BikeInputState> bikeInputState);
			virtual void run();
			void stop();
			virtual void setVibration(const bool b) { m_vibrationEnabled = b; };
			virtual InputDevice getType() = 0; //pure virtual, all methods must override
		protected:
			osg::ref_ptr<BikeInputState> m_bikeInputState;

			bool m_pollingEnabled = false;
			bool m_vibrationEnabled = false;
		};
	}
}