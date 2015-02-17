
#include "carengine.h"
#include <algorithm>
using namespace troen;

#define SPEEDTOREALRATIO  2.0

CarEngine::CarEngine(btRaycastVehicle *_mVehicle, float effiency)
{
	mVehicle = _mVehicle;

	MainGear = 3.85f;
	CurGear = 1;

	Gears[0] = -3.68;
	Gears[1] = 0.;
	Gears[2] = 4.06;
	Gears[3] = 2.4;
	Gears[4] = 1.58;
	Gears[5] = 1.19;
	Gears[6] = 1.;
	Gears[7] = 0.87;


	//Torque[0][0] = 500.f;
	//Torque[0][1] = 150.0f;

	//Torque[1][0] = 1000.f;
	//Torque[1][1] = 280.f;

	//Torque[2][0] = 1800.f;
	//Torque[2][1] = 350.0f;

	//Torque[3][0] = 2200.f;
	//Torque[3][1] = 380.f;

	//Torque[4][0] = 2500.f;
	//Torque[4][1] = 300.f;

	//Torque[4][0] = 2700.f;
	//Torque[4][1] = 260.f;



	Torque[0][0] = 1000.f;
	Torque[0][1] = 260.f;

	Torque[1][0] = 2000.f;
	Torque[1][1] = 340.f;

	Torque[2][0] = 3500.f;
	Torque[2][1] = 400.f;

	Torque[3][0] = 7500.f;
	Torque[3][1] = 380.f;

	Torque[4][0] = 8500.f;
	Torque[4][1] = 340.f;

	Torque[4][0] = 9500.f;
	Torque[4][1] = 300.f;

	IdleRPM = 1000.f;
	MaxRPM = 8300.f;

	RPM = 0.f;

	Efficiency = effiency;

	Clutch = 0;
	Throttle = 0;

	Brake = false;

	MaxSpeed = 80;
	EngineForce = 0;
}
CarEngine::~CarEngine()
{
	mVehicle = NULL;
}
void CarEngine::_computeAxisTorque(float time)
{
	btScalar speed = mVehicle->getCurrentSpeedKmHour() / SPEEDTOREALRATIO;
	EngineForce = 0.0;
	if (CurGear > 1)
	{
		RPM = computeMotorRpm(computeRpmFromWheels(btScalar(time)) * Gears[CurGear] * MainGear / SPEEDTOREALRATIO);
		btScalar torque = Throttle * getTorque(RPM) / mVehicle->m_wheelInfo[0].m_wheelsRadius;
		EngineForce = torque * Efficiency * Gears[CurGear] * MainGear;

		btAssert(!std::isnan(EngineForce));
		
		if (speed > MaxSpeed)
			EngineForce = 0.0;
	
	}

	if (CurGear == 0)
	{
		if (Brake)
		{
			EngineForce = Torque[0][0] / mVehicle->m_wheelInfo[0].m_wheelsRadius * Efficiency * Gears[0];
			if (EngineForce > 0) //sanity
				EngineForce = -EngineForce;

		}
		else
		{
			CurGear = 1;
		}


	}

	if (speed > 5)
		EngineForce -= 2000; //damping


	for (int i = 0; i < 4; i++)
		if (mVehicle->getWheelInfo(i).m_bIsFrontWheel == false)
			mVehicle->applyEngineForce(EngineForce * SPEEDTOREALRATIO, i);


	if (CurGear == 1 && Throttle > 0.1)
		CurGear = 2;
}

btScalar CarEngine::getTorque(btScalar _RPM)
{
	int i = 0;
	while (i < 4 && Torque[i + 1][0] < _RPM)
		i++;
	if (i == 4)
		return Torque[4][1];
	return Torque[i][1] + (Torque[i + 1][1] - Torque[i][1]) * (_RPM - Torque[i][0]) / (Torque[i + 1][0] - Torque[i][0]);
}

btScalar CarEngine::computeRpmFromWheels(float time)
{
	btScalar wheelRpms = 0;
	btScalar nbWheels = 0;
	btScalar wheel_angular_velocity = 0;
	btScalar btswheel_rpm = 0;

	for (int i = 0; i < mVehicle->m_wheelInfo.size(); i++)
	{
		btWheelInfo& wheel = mVehicle->m_wheelInfo[i];
		btVector3 relpos = wheel.m_raycastInfo.m_hardPointWS - mVehicle->getRigidBody()->getCenterOfMassPosition();
		btVector3 vel = mVehicle->getRigidBody()->getVelocityInLocalPoint(relpos);

		const btTransform&   chassisWorldTransform = mVehicle->getChassisWorldTransform();

		btVector3 fwd(
			chassisWorldTransform.getBasis()[0][mVehicle->getForwardAxis()],
			chassisWorldTransform.getBasis()[1][mVehicle->getForwardAxis()],
			chassisWorldTransform.getBasis()[2][mVehicle->getForwardAxis()]);

		btScalar proj = fwd.dot(wheel.m_raycastInfo.m_contactNormalWS);
		fwd -= wheel.m_raycastInfo.m_contactNormalWS * proj;

		btScalar proj2 = fwd.dot(vel);

		//if (wheel.m_raycastInfo.m_isInContact)
		//{
		//	wheel.m_deltaRotation = (proj2 * time) / (wheel.m_wheelsRadius);
		//	wheel.m_rotation += wheel.m_deltaRotation;
		//}
		//else
		//{
		//	wheel.m_rotation += wheel.m_deltaRotation;
		//}

		wheel.m_deltaRotation *= btScalar(0.99);//damping of rotation when not in contact

		wheel_angular_velocity = proj2 / (wheel.m_wheelsRadius); // rad/s
		btswheel_rpm += wheel_angular_velocity*60. / SIMD_2_PI;
	}
	if (time > 0)
		return btswheel_rpm / 4;
	else
		return btScalar(0);
}

btScalar CarEngine::computeMotorRpm(btScalar rpm)
{
	if (int change = changeGears())
		if (change == 1)
			CurGear++;
		else if (change == -1)
			CurGear--;

	if (rpm < IdleRPM)
		if (Brake)
			CurGear = 0;
		else
			return IdleRPM;

	return rpm;
}

int CarEngine::changeGears()
{
	if (Throttle == 1 && CurGear < 2)
		CurGear = 2;
	if (RPM > 7100 && CurGear < 7)
		return 1;
	else if (RPM < 3000 && CurGear > 2)
		return -1;
	else
		return 0;
}