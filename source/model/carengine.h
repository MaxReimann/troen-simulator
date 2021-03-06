#ifndef _CARENGINE_H_
#define _CARENGINE_H_

#include "physicsworld.h"
#include "../forwarddeclarations.h"

namespace troen
{
	class CarEngine
	{
		friend class CarSoundData;
	public:
		CarEngine(btRaycastVehicle *_mVehicle);
		CarEngine(btRaycastVehicle *_mVehicle, float effiency);
		~CarEngine();

		void update(float deltat)				{ _computeAxisTorque(deltat); }

		btScalar getThrottle()				{ return Throttle; }
		void	 setThrottle(btScalar th)	{ Throttle = th; }

		btScalar getClutch()				{ return Clutch; }
		void	 setClutch(btScalar cl)		{ Clutch = cl; }

		void setBrake(bool br)				{ Brake = br; }

		void setMaxSpeed(float speed)	{ MaxSpeed = speed; }


		btScalar getRPM()					{ return RPM; }
		btScalar getEngineForce()			{ return EngineForce; }
		int		 getGear()					{ return CurGear; }

		void	 upGear()					{ CurGear++; }
		void	 downGear()					{ CurGear--; }
		unsigned int getCurGear()			{ return CurGear; }

	protected:

		btScalar Clutch;
		btScalar Throttle;

		void _computeAxisTorque(float time);
		btScalar getTorque(btScalar _RPM);
		btScalar computeRpmFromWheels(float time);
		btScalar computeMotorRpm(btScalar rpm);
		int changeGears();

		btRaycastVehicle* mVehicle;
		btScalar RPM;
		float EngineForce;
		btScalar Efficiency;

		unsigned int CurGear;
		bool Brake;

		btScalar Gears[8];
		btScalar MainGear;
		btScalar IdleRPM;
		btScalar MaxRPM;

		btScalar Torque[10][2];
		btScalar MaxSpeed;
	};
}
#endif