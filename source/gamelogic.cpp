#include "GameLogic.h"
// STD
#include <array>
// bullet
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btHashMap.h"
//troen
#include "troengame.h"
#include "constants.h"
#include "model/abstractmodel.h"
#include "controller/bikecontroller.h"
#include "controller/levelcontroller.h"
#include "sound/audiomanager.h"

using namespace troen;

GameLogic::GameLogic(
	TroenGame* game,
	std::shared_ptr<sound::AudioManager>& audioManager,
	std::shared_ptr<LevelController> levelController,
	std::vector<std::shared_ptr<BikeController>> bikeControllers) :
m_troenGame(game),
m_maxFenceParts(0),
m_audioManager(audioManager),
m_levelController(levelController),
m_bikeControllers(bikeControllers)
{}

void GameLogic::attachPhysicsWorld(std::shared_ptr<PhysicsWorld>& physicsWorld)
{
	m_physicsWorld = physicsWorld;
}


void GameLogic::collisionEvent(btRigidBody * pBody0, btRigidBody * pBody1, btPersistentManifold* contactManifold)
{
	//std::cout << "[PhysicsWorld::collisionEvent] collision detected" << std::endl;
	btRigidBody * collidingBodies[2];
	collidingBodies[0] = pBody0;
	collidingBodies[1] = pBody1;

	// get the controllers of the colliding objects
	AbstractController* collisionBodyControllers[2];
	collisionBodyControllers[0] = static_cast<AbstractController *>(pBody0->getUserPointer());
	collisionBodyControllers[1] = static_cast<AbstractController *>(pBody1->getUserPointer());
	
	// exit either controlles was not found
	if (!collisionBodyControllers[0] || !collisionBodyControllers[1]) return;

	std::array<COLLISIONTYPE,2> collisionTypes;
	collisionTypes[0] = static_cast<COLLISIONTYPE>(collidingBodies[0]->getUserIndex());
	collisionTypes[1] = static_cast<COLLISIONTYPE>(collidingBodies[1]->getUserIndex());
	
	// handle colision events object specific
	auto bikeIterator = std::find(collisionTypes.cbegin(), collisionTypes.cend(), BIKETYPE);
	if (bikeIterator != collisionTypes.cend())
	{
		int bikeIndex = bikeIterator - collisionTypes.cbegin();
		int otherIndex = bikeIndex == 0 ? 1 : 0;
		switch (collisionTypes[otherIndex])
		{
		case LEVELWALLTYPE:
		case LEVELOBSTACLETYPE:
		case FENCETYPE:
			handleCollisionOfBikeAndNonmovingObject(
				static_cast<BikeController*>(collisionBodyControllers[bikeIndex]),
				collisionBodyControllers[otherIndex],
				contactManifold);
			break;
		case BIKETYPE:
			handleCollisionOfTwoBikes(
				static_cast<BikeController*>(collisionBodyControllers[bikeIndex]),
				static_cast<BikeController*>(collisionBodyControllers[otherIndex]),
				contactManifold);
			break;
		case LEVELGROUNDTYPE:
			//std::cout << "collision with ground" << std::endl;
		default:
			break;
		}
	}
}

void GameLogic::separationEvent(btRigidBody * pBody0, btRigidBody * pBody1)
{
	//std::cout << "[PhysicsWorld::seperationEvent] seperation detected" << std::endl;
	btRigidBody * collidingBodies[2];
	collidingBodies[0] = pBody0;
	collidingBodies[1] = pBody1;

	// get the controllers of the separating objects
	AbstractController* collisionBodyControllers[2];
	collisionBodyControllers[0] = static_cast<AbstractController *>(pBody0->getUserPointer());
	collisionBodyControllers[1] = static_cast<AbstractController *>(pBody1->getUserPointer());

	// exit either controlles was not found
	if (!collisionBodyControllers[0] || !collisionBodyControllers[1]) return;

	std::array<COLLISIONTYPE, 2> collisionTypes;
	collisionTypes[0] = static_cast<COLLISIONTYPE>(collidingBodies[0]->getUserIndex());
	collisionTypes[1] = static_cast<COLLISIONTYPE>(collidingBodies[1]->getUserIndex());

	// handle separation events object specific
	auto bikeIterator = std::find(collisionTypes.cbegin(), collisionTypes.cend(), BIKETYPE);
	if (bikeIterator != collisionTypes.cend())
	{
		int bikeIndex = bikeIterator - collisionTypes.cbegin();
		int otherIndex = bikeIndex == 0 ? 1 : 0;
		switch (collisionTypes[otherIndex])
		{
		case FENCETYPE:
		case LEVELTYPE:
		case LEVELOBSTACLETYPE:
		case LEVELGROUNDTYPE:
		case LEVELWALLTYPE:
		case BIKETYPE:
			break;
		default:
			break;
		}
	}
}

void GameLogic::handleCollisionOfBikeAndNonmovingObject(
	BikeController* bike,
	AbstractController* object,
	btPersistentManifold* contactManifold)
{
	btScalar impulse = 0;
	int numContacts = contactManifold->getNumContacts();
	//std::cout << numContacts << " - ";
	for (int i = 0; i < numContacts; i++)
	{
		btManifoldPoint& pt = contactManifold->getContactPoint(i);
		impulse = impulse + pt.getAppliedImpulse();
	}
	//std::cout << "total impulse: " << impulse << std::endl;
	if (impulse > BIKE_FENCE_IMPACT_THRESHOLD_LOW)
		m_audioManager->PlaySFX("data/sound/explosion.wav",
			impulse / BIKE_FENCE_IMPACT_THRESHOLD_HIGH,
			impulse / (BIKE_FENCE_IMPACT_THRESHOLD_HIGH - BIKE_FENCE_IMPACT_THRESHOLD_LOW),
			1, 1);

	if (impulse > BIKE_FENCE_IMPACT_THRESHOLD_HIGH)
	{
		m_troenGame->pauseSimulation();
		restartLevel();
	}
}

void GameLogic::handleCollisionOfTwoBikes(
	BikeController* bike1,
	BikeController* bike2,
	btPersistentManifold* contactManifold)
{
	std::cout << "[GameLogic::handleCollisionOfTwoBikes]" << std::endl;
	//TODO
	// set different thredsholds of collisions between bikes
	// they dont have as much impact ?
	handleCollisionOfBikeAndNonmovingObject(bike1, bike2, contactManifold);
}

void GameLogic::removeAllFences()
{
	for (auto bikeController : m_bikeControllers)
	{
		bikeController->removeAllFences();
	}
}

void GameLogic::toggleFencePartsLimit()
{
	if (m_maxFenceParts == 0){
		m_maxFenceParts = DEFAULT_MAX_FENCE_PARTS;
		std::cout << "[GameLogic::toggleFencePartsLimitEvent] turning fenceParsLimit ON ..." << std::endl;
	}
	else
	{
		m_maxFenceParts = 0;
		std::cout << "[GameLogic::toggleFencePartsLimitEvent] turning fenceParsLimit OFF ..." << std::endl;
	}

	for (auto bikeController : m_bikeControllers)
	{
		bikeController->enforceFencePartsLimit(m_maxFenceParts);
	}
}

void GameLogic::resetBikePositions()
{
	for (int i = 0; i < m_bikeControllers.size(); i++)
	{
		btTransform position = m_levelController->initialPositionTransformForBikeWithIndex(i);
		m_bikeControllers[i]->moveBikeToPosition(position);
	}
}

void GameLogic::restartLevel()
{
	removeAllFences();
	resetBikePositions();
}