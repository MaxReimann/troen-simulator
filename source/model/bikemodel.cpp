#include "bikemodel.h"
// OSG
#include <osg/BoundingBox>
// STD
#include <exception>
#include <cmath>
#include <iostream>
// troen
#include <reflectionzeug/Object.h>
#include <scriptzeug/ScriptContext.h>
#include "../constants.h"
#include "../input/bikeinputstate.h"
#include "../controller/bikecontroller.h"
#include "bikemotionstate.h"
#include "objectinfo.h"
#include "GetTime.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#include "physicsworld.h"
#include "../scriptable/scriptwatcher.h"

#include "../view/bikeview.h"

using namespace troen;

int rightIndex = 0;
int upIndex = 2;
int forwardIndex = 1;
btVector3 wheelDirectionCS0(0, 0, -1);
btVector3 wheelAxleCS(1, 0, 0);

BikeModel::BikeModel(
	btTransform initialTransform,
	osg::ref_ptr<osg::Group> node,
	Player * player,
	BikeController* bikeController) :
AbstractModel(),
m_lastUpdateTime(0),
m_bikeController(bikeController),
m_currentSteeringTilt(0),
m_currentWheelyTilt(0),
m_skidCount(0)
{
	resetState();

	osg::BoundingBox bb;
	bb.expandBy(node->getBound());

	btTransform nullTrans;
	nullTrans.setIdentity();
	nullTrans.setOrigin(btVector3(0, 0, BIKE_DIMENSIONS.getZ()/2 + 0.01));

	std::shared_ptr<BikeMotionState> bikeMotionState = std::make_shared<BikeMotionState>(
		initialTransform,
		dynamic_cast<osg::PositionAttitudeTransform*> (node->getChild(0)),
		player,
		this
	);
	m_motionStates.push_back(bikeMotionState);


	std::shared_ptr<btBoxShape> chassisShape = std::make_shared<btBoxShape>(BIKE_DIMENSIONS/2);
	m_collisionShapes.push_back(chassisShape);
	std::shared_ptr<btCompoundShape> compoundShape = std::make_shared<btCompoundShape>();
	m_collisionShapes.push_back(compoundShape);

	btTransform localTrans;
	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(0, 0, 1));
	compoundShape->addChildShape(localTrans, chassisShape.get());
	btVector3 bikeInertia(0, 0, 0);
	compoundShape->calculateLocalInertia(BIKE_MASS, bikeInertia);


	btRigidBody::btRigidBodyConstructionInfo m_carRigidBodyCI(BIKE_MASS, bikeMotionState.get(), compoundShape.get(), bikeInertia);

	m_carChassis = std::make_shared<btRigidBody>(m_carRigidBodyCI);
	m_rigidBodies.push_back(m_carChassis);

	//m_carChassis->setCcdMotionThreshold(1 / BIKE_DIMENSIONS.y());
	//m_carChassis->setCcdSweptSphereRadius(BIKE_DIMENSIONS.x() * .5f - BIKE_DIMENSIONS.x() * 0.01);
	// this seems to be necessary so that we can move the object via setVelocity()
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);
	//m_carChassis->setAngularFactor(btVector3(0, 0, 1));


	// for collision event handling
	ObjectInfo* info = new ObjectInfo(bikeController, BIKETYPE);
	m_carChassis->setUserPointer(info);

	bikeMotionState->setRigidBody(m_carChassis);
	m_vehicleSteering = 0.f;
}


void BikeModel::constructVehicleBody(std::shared_ptr<PhysicsWorld> world)
{
	m_world = world;
	btDynamicsWorld *discreteWorld = world->getDiscreteWorld();

	m_forwardAxis = 1;

	m_engineForce = 0.f;
	m_vehicleSteering = 0.f;
	m_breakingForce = 0.f;

	//set vehicle parameters
	m_vehicleParameters.loadVehicleParameters();

	double wheelRadius = m_vehicleParameters.wheelRadius;
	double wheelWidth = m_vehicleParameters.wheelWidth;
	double wheelFriction = m_vehicleParameters.wheelFriction;
	double suspensionStiffness = m_vehicleParameters.suspensionStiffness;
	double suspensionDamping = m_vehicleParameters.suspensionDamping;
	double suspensionCompression = m_vehicleParameters.suspensionCompression;
	double rollInfluence = m_vehicleParameters.rollInfluence;
	double suspensionRestLength = m_vehicleParameters.suspensionRestLength;
	double CUBE_HALF_EXTENTS = m_vehicleParameters.cubeHalfExtents;
	double connectionHeight = m_vehicleParameters.connectionHeight;
	double engineEfficiency = m_vehicleParameters.engineEfficiency;


	//m_carChassis->setDamping(0.0,0.0);
	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	m_vehicleRayCaster = new btDefaultVehicleRaycaster(discreteWorld);
	m_vehicle =  new btRaycastVehicle(m_tuning, m_carChassis.get(), m_vehicleRayCaster);
	discreteWorld->addAction(m_vehicle);
	m_engine = std::make_shared<CarEngine>(m_vehicle, engineEfficiency);
	m_engine->setMaxSpeed(m_vehicleParameters.maxSpeed);




	//choose coordinate system
	m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
	btVector3 halfCH = btVector3(0, 0, connectionHeight / 2 );
	btVector3 viewOffset = wheelAxleCS / 2.0 + halfCH;

	bool isFrontWheel = true;
	btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3*wheelWidth), 2 * CUBE_HALF_EXTENTS - wheelRadius, connectionHeight);
	m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	m_bikeController->getView()->addWheel(wheelRadius, btToOSGVec3((connectionPointCS0 - viewOffset)), btToOSGVec3((connectionPointCS0 + wheelAxleCS / 2 - halfCH)));
	
	connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3*wheelWidth), 2 * CUBE_HALF_EXTENTS - wheelRadius, connectionHeight);
	m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	m_bikeController->getView()->addWheel(wheelRadius, btToOSGVec3((connectionPointCS0 - viewOffset)), btToOSGVec3((connectionPointCS0 + wheelAxleCS / 2 - halfCH)));

	
	
	isFrontWheel = false;
	connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3*wheelWidth), -2 * CUBE_HALF_EXTENTS + wheelRadius, connectionHeight);
	m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	m_bikeController->getView()->addWheel(wheelRadius, btToOSGVec3((connectionPointCS0 - viewOffset)), btToOSGVec3((connectionPointCS0 + wheelAxleCS / 2 - halfCH)));
	
	connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3*wheelWidth), -2 * CUBE_HALF_EXTENTS + wheelRadius, connectionHeight);
	m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
	m_bikeController->getView()->addWheel(wheelRadius, btToOSGVec3((connectionPointCS0 - viewOffset)), btToOSGVec3((connectionPointCS0 + wheelAxleCS / 2 - halfCH)));

	
	btVector3 wheelColor(1, 0, 0);
	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
		wheel.m_maxSuspensionForce = 50000.f;
	}
	world->drawVehicle = m_vehicle;

	m_vehicle->resetSuspension();
	
}

void BikeModel::removeRaycastVehicle()
{
	m_world->getDrawShapes().clear();

	m_world->getDiscreteWorld()->removeVehicle(m_vehicle);
	m_engine.reset();
	delete m_vehicle;
	delete m_vehicleRayCaster;
	delete m_wheelShape;

	m_bikeController->getView()->removeWheels();
}


void BikeModel::resetBody()
{
	btDynamicsWorld *world = m_world->getDiscreteWorld();
	m_vehicleSteering = 0.f;
	//m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
	m_carChassis->setLinearVelocity(btVector3(0, 0, 0));
	m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
	world->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(), world->getDispatcher());
	if (m_vehicle)
	{
		//m_vehicleParameters.loadVehicleParameters();
		m_vehicle->resetSuspension();
		for (int i = 0; i < m_vehicle->getNumWheels(); i++)
		{
			btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = m_vehicleParameters.suspensionStiffness;
			wheel.m_wheelsDampingRelaxation = m_vehicleParameters.suspensionDamping;
			wheel.m_wheelsDampingCompression = m_vehicleParameters.suspensionCompression;
			wheel.m_frictionSlip = m_vehicleParameters.wheelFriction;
			wheel.m_rollInfluence = m_vehicleParameters.rollInfluence;
			wheel.m_maxSuspensionForce = 50000.f;
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle->updateWheelTransform(i, true);
		}
	}

}

std::shared_ptr<btRigidBody> BikeModel::getRigidBody() {
	return m_carChassis;
}

void BikeModel::setInputState(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	m_bikeInputState = bikeInputState;
}

void BikeModel::resetState()
{
	m_oldVelocity = 0.0;
	m_rotation = 0.0;
}

float BikeModel::getSteering()
{
	return m_vehicleSteering;
}


float BikeModel::getTurboFactor()
{
	// return value will either be between 0 and 1 or it is -1
	// it indicates if the "turbo phase" has just started or if it is already over or if it was ended abruptly
	// this can be used to compute the wheelyTilt of the bike
	return m_turboFactor;
}


long double BikeModel::getTimeSinceLastUpdate()
{
	return m_timeSinceLastUpdate;
}


float BikeModel::updateState(long double time)
{
	m_timeSinceLastUpdate = time - m_lastUpdateTime;
	m_lastUpdateTime = time;
	float timeFactor = m_timeSinceLastUpdate / 1000.0;

	btVector3 positionSmoothed = m_carChassis->getWorldTransform().getOrigin() * 0.4 + m_lastTransform.getOrigin() * 0.6;
	btQuaternion rotationSmoothed = m_carChassis->getWorldTransform().getRotation() * 0.4 + m_lastTransform.getRotation() * 0.6;
	m_lastTransform.setOrigin(positionSmoothed);
	m_lastTransform.setRotation(rotationSmoothed);

	if (m_vehicleParameters.changed())
		m_vehicleParameters.loadVehicleParameters();

	m_vehicleSteering = m_bikeInputState->getSteering();

	m_breakingForce = m_bikeInputState->getBrakeForce() * m_vehicleParameters.maxBreakingForce;
	float throttle = m_bikeInputState->getThrottle();// *2.0;
	m_engine->setThrottle(throttle);

	{
		m_vehicle->setSteeringValue(m_vehicleSteering, 0);
		m_vehicle->setSteeringValue(m_vehicleSteering, 1);

		m_vehicle->setBrake(m_breakingForce, 0);
		m_vehicle->setBrake(m_breakingForce, 1);
		m_vehicle->setBrake(m_breakingForce, 2);
		m_vehicle->setBrake(m_breakingForce, 3);

		if (m_bikeInputState->getBrakeForce() > 0.5)
			m_engine->setBrake(true);
		else
			m_engine->setBrake(false);
	}

	m_engine->update(timeFactor);




	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i, true);


		if (m_vehicle->m_wheelInfo[i].m_skidInfo == 0)
			m_skidCount++;


		btTransform localTrans = m_carChassis->getWorldTransform().inverse() * m_vehicle->getWheelInfo(i).m_worldTransform;

		//localTrans.setOrigin(m_carChassis->getCenterOfMassPosition() + localTrans.getOrigin());
		//float xRot = m_vehicle->getWheelInfo(i).m_rotation;
		//btMatrix3x3 m(localTrans.getRotation());
		//btScalar yaw, pitch, roll;
		//m.getEulerYPR(yaw, pitch, roll);

		m_bikeController->getView()->setWheelRotation(i, localTrans);
	}

	float speed = m_vehicle->getCurrentSpeedKmHour() / 2.0;
	m_oldVelocity = speed;


	return speed;
}


osg::Quat BikeModel::getTilt()
{

	float desiredSteeringTilt = getSteering() / BIKE_TILT_MAX;

	// timeFactor is 1 for 60 frames, 0.5 for 30 frames etc..
	long double timeFactor = 16.7f / getTimeSinceLastUpdate();
	// sanity check for very large delays
	if (timeFactor < 1 / BIKE_TILT_DAMPENING)
		timeFactor = 1 / BIKE_TILT_DAMPENING;

	m_currentSteeringTilt = m_currentSteeringTilt + (desiredSteeringTilt - m_currentSteeringTilt) / (BIKE_TILT_DAMPENING * timeFactor);

	float turboFactor = getTurboFactor();

	if (turboFactor < 0) {
		// no interpolation on abrupt speed change
		m_currentWheelyTilt = 0;
	}
	else{
		const float desiredWheelyTilt = getTurboFactor() / BIKE_WHEELY_TILT_MAX;
		const float tiltDifference = desiredWheelyTilt - m_currentWheelyTilt;
		m_currentWheelyTilt = m_currentWheelyTilt + tiltDifference / (BIKE_TILT_DAMPENING * timeFactor);
	}


	osg::Quat tiltSteeringQuat, tiltWheelyQuat;
	tiltSteeringQuat.makeRotate(m_currentSteeringTilt, osg::Vec3(0, 1, 0));
	tiltWheelyQuat.makeRotate(m_currentWheelyTilt, osg::Vec3(-1, 0, 0));

	return tiltSteeringQuat * tiltWheelyQuat;
}

float BikeModel::getRotation()
{
	return m_rotation;
}

float BikeModel::getVelocity()
{
	return m_oldVelocity;
}

float BikeModel::getInputAcceleration()
{
	return m_bikeInputState->getThrottle();
}

float BikeModel::getInputAngle()
{
	return m_bikeInputState->getAngle();
}

osg::Vec3d BikeModel::getPositionOSG()
{
	btTransform trans;
	trans = m_carChassis->getWorldTransform();
	return osg::Vec3d(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

btVector3 BikeModel::getPositionBt()
 {
	btTransform trans;
	trans = m_carChassis->getWorldTransform();

	return trans.getOrigin();
}

btVector3 BikeModel::getEulerYPR()
{
	btQuaternion rot = m_carChassis->getWorldTransform().getRotation();
	float yaw, pitch, roll;
	btMatrix3x3 mat(rot);
	mat.getEulerYPR(yaw, pitch, roll);
	
	return btVector3(yaw, pitch, roll);
}


btQuaternion BikeModel::getRotationQuat()
{
	btTransform trans;
	trans = m_carChassis->getWorldTransform();

	return trans.getRotation();
}

btTransform BikeModel::getTransform()
{
	return  m_carChassis->getWorldTransform();
}

btVector3 BikeModel::getLinearVelocity()
{
	return m_carChassis->getLinearVelocity();
}

btVector3 BikeModel::getAngularVelocity()
{
	return m_carChassis->getAngularVelocity();
}

btVector3 BikeModel::getDirection()
{
	float angle = m_carChassis->getOrientation().getAngle();
	btVector3 axis = m_carChassis->getOrientation().getAxis();
	const btVector3 front = btVector3(0, 1, 0);
	return front.rotate(axis, angle);
}
void BikeModel::moveBikeToLastPoint()
{
	moveBikeToPosition(m_lastTransform);
}

void BikeModel::moveBikeToPosition(btTransform position)
{
	
	m_carChassis->setWorldTransform(position);
	m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
	m_carChassis->setLinearVelocity(btVector3(0, 0, 0));
}

void BikeModel::freeze()
{
	m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
	m_carChassis->setLinearVelocity(btVector3(0, 0, 0));

}

void BikeModel::dampOut()
{
	m_carChassis->setLinearVelocity(m_carChassis->getLinearVelocity() / 1.08f);
	m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
}
void BikeModel::clearDamping()
{
	m_carChassis->setDamping(0, 0);
}

btTransform BikeModel::getLastTransform()
{
	return m_lastTransform;
}


VehiclePhysicSettings::VehiclePhysicSettings() : AbstractScript("vehicle")
{
	typedef VehiclePhysicSettings VPS;

	addProperty<double>("engineEfficiency", *this, &VPS::EngineEfficiency, &VPS::setEngineEfficiency);
	addProperty<double>("maxBreakingForce", *this, &VPS::MaxBreakingForce, &VPS::setMaxBreakingForce);
	addProperty<double>("wheelRadius", *this, &VPS::WheelRadius, &VPS::setWheelRadius);
	addProperty<double>("wheelWidth", *this, &VPS::WheelWidth, &VPS::setWheelWidth);
	addProperty<double>("wheelFriction", *this, &VPS::WheelFriction, &VPS::setWheelFriction);
	addProperty<double>("suspensionStiffness", *this, &VPS::SuspensionStiffness, &VPS::setSuspensionStiffness);
	addProperty<double>("suspensionDamping", *this, &VPS::SuspensionDamping, &VPS::setSuspensionDamping);
	addProperty<double>("suspensionCompression", *this, &VPS::SuspensionCompression, &VPS::setSuspensionCompression);
	addProperty<double>("rollInfluence", *this, &VPS::RollInfluence, &VPS::setRollInfluence);
	addProperty<double>("suspensionRestLength", *this, &VPS::SuspensionRestLength, &VPS::setSuspensionRestLength);
	addProperty<double>("suspensionCompression", *this, &VPS::SuspensionCompression, &VPS::setSuspensionCompression);
	addProperty<double>("cubeHalfExtents", *this, &VPS::CubeHalfExtents, &VPS::setCubeHalfExtents);
	addProperty<double>("maxSpeed", *this, &VPS::MaxSpeed, &VPS::setMaxSpeed);
	//addProperty<int>("forwardAxis", *this, &VPS::ForwardAxis, &VPS::setForwardAxis);
	addProperty<double>("connectionHeight", *this, &VPS::ConnectionHeight, &VPS::setConnectionHeight);

	changesPending = new bool;
	*changesPending = false;
	
	m_scriptContext->registerObject(this);
	m_scriptWatcher.watchAndLoad("scripts/vehiclephysics.js", this, changesPending);
}

void VehiclePhysicSettings::loadVehicleParameters()
{
	try
	{
		m_scriptContext->evaluate(m_scriptWatcher.getContent());
		*changesPending = false;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}
