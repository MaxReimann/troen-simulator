#pragma once
// OSG
#include <osg/ref_ptr>
#include <osg/Vec3d>
#include <osg/Group>
// troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"

#include <reflectionzeug/Object.h>
#include <scriptzeug/ScriptContext.h>
#include "carengine.h"
#include "customfriction.h"

#include "../util/scriptwatcher.h"
using namespace reflectionzeug;

namespace troen
{


	class VehiclePhysicSettings : public reflectionzeug::Object
	{
	public:

		int forwardAxis = 1;
		double	maxBreakingForce = 00.0;
		double	engineEfficiency = 00.0; //scale velocity with this parameter
		double	wheelRadius = 00.0;
		double	wheelWidth = 00.0;
		double	wheelFriction = 00.0;//BT_LARGE_double;
		double	suspensionStiffness = 00.0;
		double	suspensionDamping = 00.0;
		double	suspensionCompression = 00.0;
		double	rollInfluence = 00.0;//1.0f;
		double suspensionRestLength = 00.0;
		double cubeHalfExtents = 0.0;
		double connectionHeight = 0.0;
		double maxSpeed = 0.0;
		bool *changesPending;


		double EngineEfficiency() const { return engineEfficiency; }
		void setEngineEfficiency(const double & val) { engineEfficiency = val; }

		double MaxBreakingForce() const { return maxBreakingForce; }
		void setMaxBreakingForce(const double & val) { maxBreakingForce = val; }
		double WheelRadius() const { return wheelRadius; }
		void setWheelRadius(const double & val) { wheelRadius = val; }
		double WheelWidth() const { return wheelWidth; }
		void setWheelWidth(const double & val) { wheelWidth = val; }
		double WheelFriction() const { return wheelFriction; }
		void setWheelFriction(const double & val) { wheelFriction = val; }
		double SuspensionStiffness() const { return suspensionStiffness; }
		void setSuspensionStiffness(const double & val) { suspensionStiffness = val; }
		double SuspensionDamping() const { return suspensionDamping; }
		void setSuspensionDamping(const double & val) { suspensionDamping = val; }
		double SuspensionCompression() const { return suspensionCompression; }
		void setSuspensionCompression(const double & val) { suspensionCompression = val; }
		double RollInfluence() const { return rollInfluence; }
		void setRollInfluence(const double & val) { rollInfluence = val; }
		double SuspensionRestLength() const { return suspensionRestLength; }
		void setSuspensionRestLength(const double & val) { suspensionRestLength = val; }
		int ForwardAxis() const { return forwardAxis; }
		void setForwardAxis(const int &val) { forwardAxis = val; }
		double CubeHalfExtents() const { return cubeHalfExtents; }
		void setCubeHalfExtents(const double & val) { cubeHalfExtents = val; }
		double ConnectionHeight() const { return connectionHeight; }
		void setConnectionHeight(const double & val) { connectionHeight = val; }
		double MaxSpeed() const { return maxSpeed; }
		void setMaxSpeed(const double & val) { maxSpeed = val; }


		void log(std::string message) {
			std::cout << "script log:   " << message << std::endl;
		}



		void loadVehicleParameters();
		VehiclePhysicSettings();

	private:
		scriptzeug::ScriptContext m_scriptContext;
		ScriptWatcher m_scriptWatcher;

	};

	class BikeModel : public AbstractModel
	{
		friend class BikeController;
		friend class BikeInputState;
	public:
		BikeModel(
			btTransform initialTransform,
			osg::ref_ptr<osg::Group> node,
			Player * player,
			BikeController* bikeController);

		void setInputState(osg::ref_ptr<input::BikeInputState> bikeInputState);

		long double getTimeSinceLastUpdate();
		float updateState(const long double time);
		void resetState();
		void freeze();

		void rotate(const float angle);
		void accelerate(const float velocity);
		float getRotation();
		float getVelocity();
		float getSteering();
		osg::Vec3d getPositionOSG();
		btVector3 getPositionBt();
		btQuaternion getRotationQuat();
		btTransform getLastTransform();

		void moveBikeToPosition(btTransform position);
		float getTurboFactor();
		void updateTurboFactor(const float newVelocity, const float time);
		float getInputAcceleration();
		float getInputAngle();
		btVector3 getLinearVelocity();
		btVector3 getAngularVelocity();
		osg::Quat getTilt();
		btTransform getTransform();
		btVector3 BikeModel::getDirection();
		std::shared_ptr<btRigidBody> getRigidBody();
		void dampOut();
		void clearDamping();
		void constructVehicleBody(std::shared_ptr<PhysicsWorld> world);
		void resetBody();
		double getBrakeForce();
		void removeRaycastVehicle();
		btVector3 getEulerYPR();
		void moveBikeToLastPoint();
	protected:
		float m_steeringClamp;
		float m_steeringIncrement;
		float m_vehicleSteering;

	private:
		osg::ref_ptr<input::BikeInputState> m_bikeInputState;
		float m_oldVelocity;
		float m_rotation;
		float m_turboFactor;
		float m_timeOfLastTurboInitiation;
		long double m_lastUpdateTime;
		BikeController* m_bikeController;
		long double m_timeSinceLastUpdate;
		float m_currentSteeringTilt;
		float m_currentWheelyTilt;
		std::shared_ptr<btRigidBody> m_carChassis;
		int m_forwardAxis;

		btRaycastVehicle::btVehicleTuning	m_tuning;
		btVehicleRaycaster*	m_vehicleRayCaster;
		btRaycastVehicle*	m_vehicle;
		btCollisionShape*	m_wheelShape;
		float m_engineForce;
		float m_breakingForce;
		std::shared_ptr<PhysicsWorld> m_world;
		VehiclePhysicSettings m_vehicleParameters;
		std::shared_ptr<CarEngine> m_engine;
		CustomFrictionConstraint *mfrictionContraint;
		btTransform m_lastTransform;

		int m_skidCount;
	};



}