#ifdef WIN32

#include "ffbwheel.h"
// troen
#include "bikeinputstate.h"
#include "../constants.h"
// other
#include <WinBase.h>
#include <numeric>

DWORD(WINAPI* Custom_XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
DWORD(WINAPI* Custom_XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
VOID(WINAPI* Custom_XInputEnable)(BOOL enable);

using namespace troen::input;

std::vector <int> FFBWheel::freePorts(XUSER_MAX_COUNT);

std::vector <int>* FFBWheel::getFreePorts()
{
	// initialize freePorts
	if (FFBWheel::freePorts.back() == 0){
		std::iota(FFBWheel::freePorts.begin(), FFBWheel::freePorts.end(), 0);
	}
	return &FFBWheel::freePorts;
}

void FFBWheel::clearPorts()
{
	FFBWheel::freePorts.clear();
	FFBWheel::freePorts.resize(XUSER_MAX_COUNT, 0);
}

FFBWheel::~FFBWheel() {
	if (m_controllerId >= 0) {
	//this led to corrupt files on a win7, not sure why, disable for now
	//will lead to error on exit
#ifdef _WIN32_WINNT_WIN8 
		std::cout << "shutting down xinput emulation .. this might lead to errors.." << std::endl;
		setVibration(false);
		Custom_XInputEnable(false);
#endif
	}
}




XINPUT_GAMEPAD* FFBWheel::getState()
{
	return &m_state.Gamepad;
}

int FFBWheel::getPort()
{
	return m_controllerId;
}

bool FFBWheel::checkConnection()
{
	std::vector <int>* freePorts = FFBWheel::getFreePorts();
	m_isConnected = false;

	if (m_controllerId == -1) {
		for (auto i : *freePorts)
		{
			
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			if (Custom_XInputGetState(i, &state) == ERROR_SUCCESS) {
				std::cout << "succesfull ffb connection no port " << i << std::endl;
				m_controllerId = i;
				m_isConnected = true;
				// remove element with value i from vector
				freePorts->erase(std::remove(freePorts->begin(), freePorts->end(), i), freePorts->end());
				break;
			}
		}
	}
	else {
		ZeroMemory(&m_state, sizeof(XINPUT_STATE));

		if (Custom_XInputGetState(m_controllerId, &m_state) == ERROR_SUCCESS) {
			m_isConnected = true;
		}
	}

	return m_isConnected;
}

double scale_between(double number, double	from_min, double from_max, double to_min, double to_max)
{
	return ((to_max - to_min) * (number - from_min)) / (from_max - from_min) + to_min;
}

// Returns false if the controller has been disconnected
void FFBWheel::run()
{
	m_pollingEnabled = true;

	while (m_pollingEnabled)
	{
		if (!m_isConnected)
			checkConnection();

		if (m_isConnected)
		{
			waitIfPaused();

			ZeroMemory(&m_state, sizeof(XINPUT_STATE));
			if (Custom_XInputGetState(m_controllerId, &m_state) != ERROR_SUCCESS)
			{
				m_bikeInputState->setAngle(0);
				m_bikeInputState->setAcceleration(0);
				m_bikeInputState->setTurboPressed(false);
				m_bikeInputState->setViewingAngle(0);
				m_isConnected = false;
				continue;
			}

			float normLX = fmaxf(-1, (float)m_state.Gamepad.sThumbLX / 32767);
			m_WheelX = (abs(normLX) < m_deadzoneX ? 0 : (abs(normLX) - m_deadzoneX) * (normLX / abs(normLX)));
			m_WheelX = scale_between(m_WheelX, -1, 1, -BIKE_STEERINGCLAMP, BIKE_STEERINGCLAMP);
			m_WheelX = -m_WheelX;

			float normLY = fminf(fmaxf(-1, (float)m_state.Gamepad.sThumbLY / 32767),1.0);
			//m_brake = (abs(normLY) < m_deadzoneY ? 0 : (abs(normLY) - m_deadzoneY) * (normLY / abs(normLY)));
			m_brake = scale_between(normLY, -1, 1, 0, 1);

			float normRY = fminf(fmaxf(-1, (float)m_state.Gamepad.sThumbRY / 32767), 1.0);
			//m_throttle = (abs(normRY) < m_deadzoneY ? 0 : (abs(normRY) - m_deadzoneY) * (normRY / abs(normRY)));
			m_throttle = scale_between(normRY, -1, 1, 0, 1);

			if (m_deadzoneX > 0) m_WheelX *= 1 / (1 - m_deadzoneX);
			if (m_deadzoneY > 0) m_throttle *= 1 / (1 - m_deadzoneY);

			float handbrakePressed = isPressed(XINPUT_GAMEPAD_X);
			bool turboPressed = isPressed(XINPUT_GAMEPAD_A);
			
			
			m_bikeInputState->m_vehicleSteering = m_WheelX;

			m_bikeInputState->setAcceleration(m_throttle);
			m_bikeInputState->setBrakeForce(m_brake);
			//m_bikeInputState->setAcceleration(m_throttle - m_brake);
			//m_bikeInputState->setAngle(-m_leftStickX);
			//m_bikeInputState->setTurboPressed(turboPressed);

			vibrate();
		}

		this->msleep(POLLING_DELAY_MS);
	}
}


void FFBWheel::vibrate() {
	bool b = m_vibrationEnabled;

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = b ? 65535 : 0; // use any value between 0-65535 here
	vibration.wRightMotorSpeed = b ? 32000 : 0; // use any value between 0-65535 here
	Custom_XInputSetState(m_controllerId, &vibration);
}

bool FFBWheel::isPressed(unsigned short button)
{
	return (m_state.Gamepad.wButtons & button) != 0;
}

std::string fullPath(std::string relativePath)
{
#define BUFSIZE 4096
	TCHAR  buffer[BUFSIZE] = TEXT("");
	TCHAR** lppPart = { NULL };

	DWORD retval = GetFullPathName(relativePath.c_str(),
		BUFSIZE,
		buffer,
		lppPart);

	if (retval == 0)
	{
		// Handle an error condition.
		printf("GetFullPathName failed (%d)\n", GetLastError());
		return "";
	}

	return std::string(buffer);
}

template<typename T>
void FFBWheel::GetProcAddress(const char* funcname, T* ppfunc)
{
	*ppfunc = reinterpret_cast<T>(::GetProcAddress(hGetProcIDDLL, funcname));
	if (!ppfunc) {
		std::cout << "could not locate the function " << funcname << std::endl;
	}
}



int FFBWheel::loadCustomXInput()
{
	std::cout << "loading custom xinput dll" << std::endl;
#ifdef _DEBUG
	hGetProcIDDLL = LoadLibrary(fullPath("data\\FFB_Wheel\\Debug\\XInput9_1_0.dll").c_str());
#else
	hGetProcIDDLL = LoadLibrary(fullPath("data\\FFB_Wheel\\Release\\XInput9_1_0.dll").c_str());
#endif // DEBUG

	if (!hGetProcIDDLL)
	{
		printf("Loading DLL failed (%d)\n", GetLastError());
		return EXIT_FAILURE;
	}

	GetProcAddress("XInputGetState", &Custom_XInputGetState);
	GetProcAddress("XInputSetState", &Custom_XInputSetState);
	GetProcAddress("XInputEnable", &Custom_XInputEnable);

	std::cout << " loading dll succeeded" << std::endl;

	return 1;

}

#endif