#pragma once

#ifdef WIN32

// OSG
#include <osg/ref_ptr>
// troen
#include "pollingdevice.h"
#include "../forwarddeclarations.h"
//other
#define _AMD64_
#include <xinput.h>
#include <vector>


namespace troen
{
namespace input
{
	/*! The FFBWheel Class takes input from a forcefeedback wheel. Because xinput only supports the x360 controller, we need to 
	use the x360ce to emulate the controls. Troen has a strange issue of not loading the supplied x360ce xinput dll, so we load
	the dll explicitly and call the approriate functions. The values of the controller can be adjusted in the x360ce.ini (with the x360ce tool)
	TO prevent a invalid read in shutdown issue with the thrustmaster t100, the dll code has been patched to disable shutdown code for the thrustmaster.
	I dont know, if this will affect other programs or even corrupt memory. It is the only way to prevent the error though*/
	class FFBWheel : public PollingDevice
	{
	public:
		FFBWheel(osg::ref_ptr<BikeInputState> bikeInputState) : PollingDevice(bikeInputState) {
			m_deadzoneX = 0.001f;
			m_deadzoneY = 0.2f;
			m_isConnected = false;
			m_controllerId = -1;
			loadCustomXInput();
		};
		~FFBWheel();
		XINPUT_GAMEPAD* getState();
		bool checkConnection();
		void run() override;
		bool isPressed(const unsigned short button);
		int loadCustomXInput();

		void vibrate();
		int getPort();
		static std::vector <int>* getFreePorts();
		static std::vector <int> freePorts;
		static void clearPorts();

	private:
		int m_controllerId, m_isConnected;
		XINPUT_STATE m_state;
		float m_deadzoneX, m_deadzoneY;
		float m_leftStickX, m_leftStickY, m_rightStickX, m_rightStickY;
		float m_brake, m_throttle;
		HINSTANCE hGetProcIDDLL;

		template<typename T>
		void GetProcAddress(const char* funcname, T* ppfunc);
	};
}
}

#endif