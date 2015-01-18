#include "GameLogic.h"
// STD
#include <array>
#include <random>
#include <chrono>
// bullet
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btHashMap.h"
//troen
#include "troengame.h"
#include "constants.h"
#include "globals.h"
#include "player.h"
#include "model/abstractmodel.h"
#include "controller/abstractcontroller.h"
#include "controller/bikecontroller.h"
#include "controller/levelcontroller.h"
#include "controller/bikecontroller.h"
#include "controller/routecontroller.h"
#include "sound/audiomanager.h"
#include "model/objectinfo.h"
#include "model/bikemodel.h"
#include "tracking/trackbike.h"


using namespace troen;

GameLogic::GameLogic(TroenGame* game,const int timeLimit) :
m_troenGame(game),
m_gameState(GAMESTATE::GAME_START),
m_timeLimit(timeLimit*1000*60),
m_gameStartTime(-1),
m_limitedFenceMode(true)
{
	if (m_troenGame->isNetworking())
		m_receivedGameMessages = m_troenGame->getNetworkManager()->m_receivedGameStatusMessages;
}

////////////////////////////////////////////////////////////////////////////////
//
// Stepping
//
////////////////////////////////////////////////////////////////////////////////

void GameLogic::step(const long double gameloopTime, const long double gameTime)
{
	
	switch (m_gameState)
	{
	case GAME_START:
		stepGameStart(gameloopTime, gameTime);
		break;
	case GAME_RUNNING:
		stepGameRunning(gameloopTime, gameTime);
		checkForFallenPlayers();
		break;
	case GAME_OVER:
		stepGameOver(gameloopTime, gameTime);
		break;
	default:
		break;
	}
	
}

void GameLogic::stepGameStart(const long double gameloopTime, const long double gameTime)
{
	if (m_gameStartTime == -1)
	{
		m_gameStartTime = gameloopTime + GAME_START_COUNTDOWN_DURATION;
		for (auto player : m_troenGame->m_players)
			player->bikeController()->setState(BikeController::BIKESTATE::WAITING_FOR_GAMESTART,gameloopTime);
	}

	if (gameloopTime > m_gameStartTime)
	{
		for (auto player : m_troenGame->m_players)
			player->bikeController()->setState(BikeController::BIKESTATE::DRIVING);
		m_gameState = GAMESTATE::GAME_RUNNING;
		m_gameStartTime = -1;
		m_troenGame->unpauseSimulation();
	}
}

void GameLogic::stepGameRunning(const long double gameloopTime, const long double gameTime)
{

	if (gameTime >= m_timeLimit && m_timeLimit != 0)
	{
		m_gameState = GAMESTATE::GAME_OVER;
		m_troenGame->pauseSimulation();
	}
}

void GameLogic::stepGameOver(const long double gameloopTime, const long double gameTime)
{
	;
}

////////////////////////////////////////////////////////////////////////////////
//
// Collision Event Handling
//
////////////////////////////////////////////////////////////////////////////////

void GameLogic::collisionEvent(btRigidBody * pBody0, btRigidBody * pBody1, btPersistentManifold* contactManifold)
{
	if (!pBody0->isInWorld() || !pBody1->isInWorld()) {
		return;
	}

	btRigidBody * collidingBodies[2];
	collidingBodies[0] = pBody0;
	collidingBodies[1] = pBody1;

	// get the controllers of the colliding objects
	ObjectInfo* objectInfos[2];
	objectInfos[0] = static_cast<ObjectInfo *>(pBody0->getUserPointer());
	objectInfos[1] = static_cast<ObjectInfo *>(pBody1->getUserPointer());

	// try to recognize invalid pointers - workaround for debugmode
	//if (objectInfos[0] == (void*)0xfeeefeeefeeefeee || objectInfos[1] == (void*)0xfeeefeeefeeefeee) return;

	AbstractController* collisionBodyControllers[2];
	try {
		collisionBodyControllers[0] = objectInfos[0]->getUserPointer();
		collisionBodyControllers[1] = objectInfos[1]->getUserPointer();
	}
	catch (int e) {
		std::cout << "RigidBody invalid, but pointer was not 0xfeeefeeefeeefeee: " << e << std::endl;
		return;
	}

	// exit if either controller was not found
	if (!collisionBodyControllers[0] || !collisionBodyControllers[1]) return;

	std::array<COLLISIONTYPE,2> collisionTypes;
	collisionTypes[0] = static_cast<COLLISIONTYPE>(objectInfos[0]->getUserIndex());
	collisionTypes[1] = static_cast<COLLISIONTYPE>(objectInfos[1]->getUserIndex());


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
			handleCollisionOfBikeAndNonmovingObject(
				dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]),
				collisionBodyControllers[otherIndex],
				collisionTypes[otherIndex],
				contactManifold);
			break;

		case FENCETYPE:
			break;

		case BIKETYPE:
			handleCollisionOfTwoBikes(
				dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]),
				dynamic_cast<BikeController*>(collisionBodyControllers[otherIndex]),
				contactManifold);
			break;

		case ZONETYPE:
			handleEndZoneCollision(dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]));
			break;

		case LEVELGROUNDTYPE:
			break;
		case NAVIGATION_BOUNDARY:
			break;
		default:
			break;
		}
	}
}


void GameLogic::handleCollisionOfTwoBikes(
	BikeController* bike1,
	BikeController* bike2,
	btPersistentManifold* contactManifold)
{
	std::cout << "[GameLogic::handleCollisionOfTwoBikes]" << std::endl;
	// TODO
	// set different thredsholds of collisions between bikes
	// they dont have as much impact ?
	if (m_troenGame->isNetworking() && !m_troenGame->getNetworkManager()->isServer())
		return; //only server has authority over collision

	handleCollisionOfBikeAndNonmovingObject(bike1, bike2, BIKETYPE, contactManifold);
}


void GameLogic::handleCollisionOfBikeAndNonmovingObject(
	BikeController* bike,
	AbstractController* object,
	const int objectType,
	btPersistentManifold* contactManifold)
{
	btScalar impulse = impulseFromContactManifold(contactManifold, bike);
	if (bike->player()->hasGameView())
		playCollisionSound(impulse);


	if (bike->player()->isRemote())
		return; //let the remote player handle the collsions himself

	bike->registerCollision(impulse);


	//
	// player death
	//
	if (bike->player()->isDead())
	{
		handlePlayerDeath(bike);
		handlePlayerDeathNonFence(bike);
		


	}
}

void GameLogic::handlePlayerDeath(
	BikeController* bike)
{
	bike->player()->increaseDeathCount();
	//handled in bikecontroller::updateModel
	bike->setState(BikeController::BIKESTATE::RESPAWN, g_gameTime);
}


void GameLogic::handleEndZoneCollision(BikeController* bike)
{
	m_troenGame->getBikeTracker()->exportTaskStats(g_gameTime);
	std::cout << "in endzone" << std::endl;



	if (bike->player()->hasNextTrack())
	{
		bike->player()->routeController()->removeAllFencesFromModel();
		bike->setState(BikeController::BIKESTATE::RESPAWN_NEWTRACK, g_gameTime);

	}
	else
	{
		std::cout << "finished all routes" << std::endl;
	}


}


void GameLogic::handlePlayerDeathNonFence(BikeController* deadBike)
{

	Player* deadPlayer = deadBike->player();

	for (auto player : m_troenGame->m_players)
	{
		if (player.get() != deadPlayer)
		{
			player->increaseKillCount();
			if (player->hasGameView())
			{
				player->hudController()->addDiedMessage(deadPlayer);
			}
		}
	}
}

void GameLogic::handlePlayerFall(BikeController* deadBike)
{
	handlePlayerDeath(deadBike);

	Player* deadPlayer = deadBike->player();

	for (auto player : m_troenGame->m_players)
	{
		if (player.get() != deadPlayer)
		{
			player->increaseKillCount();
			if (player->hasGameView())
			{
				player->hudController()->addDiedOnFallMessage(deadPlayer);
			}
		}
	}
}

btScalar GameLogic::impulseFromContactManifold(btPersistentManifold* contactManifold, BikeController* bike)
{
	btScalar impulse = 0;
	int numContacts = contactManifold->getNumContacts();

	for (int i = 0; i < numContacts; i++)
	{
		btManifoldPoint& pt = contactManifold->getContactPoint(i);

		// ignore collisions with xy-plane (so the player doesn't lose health if he drives on a surface)
		if (abs(pt.m_normalWorldOnB.z()) < 0.5)
		{
			impulse = impulse + pt.getAppliedImpulse();
		}
		// let the player die instantly if he hits a wall head-on frontal
		btVector3 bikeDirection = bike->getModel()->getDirection();
		btVector3 crossProduct = bikeDirection.normalized().cross(pt.m_normalWorldOnB);
		if (crossProduct.length() < 0.2 && impulse > 5)
		{
			impulse = BIKE_FENCE_IMPACT_THRESHOLD_HIGH * 5;
		}
	}
	
	return impulse;
}

////////////////////////////////////////////////////////////////////////////////
//
// logic methods
//
////////////////////////////////////////////////////////////////////////////////

void GameLogic::removeAllFences()
{
	for (auto player : m_troenGame->m_players)
	{
		player->routeController()->removeAllFences();
	}
}


void GameLogic::toggleFencePartsLimit()
{
	m_limitedFenceMode = !m_limitedFenceMode;
	if (m_limitedFenceMode){
		std::cout << "[GameLogic::toggleFencePartsLimitEvent] turning fenceParsLimit ON ..." << std::endl;
	}
	else
	{
		std::cout << "[GameLogic::toggleFencePartsLimitEvent] turning fenceParsLimit OFF ..." << std::endl;
	}

	for (auto player : m_troenGame->m_players)
	{
		player->routeController()->setLimitFence(m_limitedFenceMode);
	}
}

void GameLogic::resetBike(BikeController *bikeController)
{
	bikeController->reset();


}

void GameLogic::resetBikePositions()
{
	for (auto player : m_troenGame->m_players)
	{
		btTransform position = player->routeController()->getFirstWayPoint();
		player->bikeController()->moveBikeToPosition(position);
	}
}

void GameLogic::restartLevel()
{
	removeAllFences();
	resetBikePositions();
}

void GameLogic::playCollisionSound(float impulse)
{
	if (impulse > BIKE_FENCE_IMPACT_THRESHOLD_LOW)
	{
		m_troenGame->m_audioManager->PlaySFX("data/sound/explosion.wav",
			impulse / BIKE_FENCE_IMPACT_THRESHOLD_HIGH,
			impulse / (BIKE_FENCE_IMPACT_THRESHOLD_HIGH - BIKE_FENCE_IMPACT_THRESHOLD_LOW),
			1, 1);
	}
}

void GameLogic::hideFencesInRadarForPlayer(int id)
{
	for (auto player : m_troenGame->m_players)
	{
		player->routeController()->hideFencesInRadarForPlayer(id);
	}
}

void GameLogic::showFencesInRadarForPlayer(int id)
{
	for (auto player : m_troenGame->m_players)
	{
		player->routeController()->showFencesInRadarForPlayer(id);
	}
}

Player* GameLogic::getPlayerWithID(int bikeID)
{
	for (auto player : m_troenGame->players())
	{
		if (player->getNetworkID() == bikeID)
			return player.get();
	}
	return NULL;
}

void GameLogic::checkForFallenPlayers()
{
	for (auto player : m_troenGame->m_players)
	{
		BikeController* bike = player->bikeController().get();
		if (bike->isFalling() && !bike->player()->isRemote())
		{
			handlePlayerFall(bike);
		}
	}
}

