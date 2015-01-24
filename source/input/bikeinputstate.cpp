#include "bikeinputstate.h"
// troen
#include "pollingdevice.h"
#include "../constants.h"
#include "LocklessTypes.h"


using namespace troen::input;

BikeInputState::BikeInputState()
{
	m_acceleration = 0.0;
	m_brakeforce = 0.0;
	m_angle = 0.0;
	m_turboPressed = false;
	m_viewingAngle = 0.0;
	//only used for network
	m_position = btVector3(0.0, 0.0, 0.0);
	m_isNewPosition = true;
	m_vehicleSteering = 0;

}

float BikeInputState::getAngle()
{
	return m_angle * BIKE_ROTATION_VALUE;
}

float BikeInputState::getSteering()
{
	return m_vehicleSteering;
}

float BikeInputState::getAcceleration()
{
	// deceleration is stronger than acceleration
	return m_acceleration < 0 ? m_acceleration * BIKE_DECELERATION_FACTOR : m_acceleration;
}

void BikeInputState::steerLeft(float factor)
{
	m_vehicleSteering += BIKE_STEERING_INCREMENT;
	if (m_vehicleSteering > BIKE_STEERINGCLAMP)
		m_vehicleSteering = BIKE_STEERINGCLAMP;
}

void BikeInputState::steerRight(float factor)
{
	m_vehicleSteering += BIKE_STEERING_INCREMENT;
	if (m_vehicleSteering < -BIKE_STEERINGCLAMP)
		m_vehicleSteering = -BIKE_STEERINGCLAMP;
}


void BikeInputState::setAngle(float angle)
{
	// angle should be smaller for small values to avoid overly responsive steering
	m_angle = abs(angle) < 1.f ? sign(angle) * pow(angle, 2) : angle;
}

void BikeInputState::setAcceleration(float acceleration)
{
	m_acceleration = acceleration;
}

void BikeInputState::setBrakeForce(float brakeforce)
{
	m_brakeforce = brakeforce;
}

double BikeInputState::getBrakeForce()
{
	return m_brakeforce;
}


bool BikeInputState::getTurboPressed()
{
	return m_turboPressed;
}

void BikeInputState::setTurboPressed(bool pressed)
{
	//! no turbo for simulator
	//m_turboPressed = pressed;
	m_turboPressed = false;
}

float BikeInputState::getViewingAngle()
{
	return m_viewingAngle;
}

void BikeInputState::setViewingAngle(float angle)
{
	m_viewingAngle = angle;
}


btVector3 BikeInputState::getPosition()
{
	return m_position;
}

void BikeInputState::setPosition(btVector3 position)
{
	m_position = position;
}

btQuaternion BikeInputState::getRotation() 
{ 
	return m_rotation; 
}

void BikeInputState::setRotation(btQuaternion val) 
{ 
	m_rotation = val;
}

btVector3 BikeInputState::getLinearVelocity() 
{
	return m_linearVeloctiy;
}

void BikeInputState::setLinearVeloctiy(btVector3 val)
{
	m_linearVeloctiy = val;
}

float BikeInputState::getAngularVelocity() 
{
	return m_angularVelocity;
}

void BikeInputState::setAngularVelocityZ(float val) 
{ 
	m_angularVelocity = val; 
}

bool BikeInputState::isNewPosition()
{
	return m_isNewPosition;
}

void BikeInputState::setIsNewPosition(bool value)
{
	m_isNewPosition = value;
}

void BikeInputState::setReceivementTimestamp(RakNet::Time time)
{
	m_receivementTime = time;
}

RakNet::Time BikeInputState::getReceivementTime()
{
	return m_receivementTime;
}
