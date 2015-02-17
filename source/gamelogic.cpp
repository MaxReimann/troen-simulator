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
#include "util/countdowntimer.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"



using namespace troen;

GameLogic::GameLogic(TroenGame* game,const int timeLimit) :
m_troenGame(game),
m_gameState(GAMESTATE::GAME_START),
m_timeLimit(timeLimit*1000*60),
m_gameStartTime(-1),
m_limitedFenceMode(true)
{
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
		checkForTossedPlayers();
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
		std::cout << "[GameLogic::collisionEvent] RigidBody invalid, but pointer was not 0xfeeefeeefeeefeee: " << e << std::endl;
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
		case ENDZONETYPE:
			handleEndZoneCollision(dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]));
			break;
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
		case LEVELGROUNDTYPE:
			break;
		default:
			break;
		}
	}
}


void GameLogic::triggerEvent(btCollisionObject *obj1, btCollisionObject *obj2)
{

	ObjectInfo* objectInfos[2];
	objectInfos[0] = static_cast<ObjectInfo *>(obj1->getUserPointer());
	objectInfos[1] = static_cast<ObjectInfo *>(obj2->getUserPointer());

	std::array<COLLISIONTYPE, 2> collisionTypes;
	collisionTypes[0] = static_cast<COLLISIONTYPE>(objectInfos[0]->getUserIndex());
	collisionTypes[1] = static_cast<COLLISIONTYPE>(objectInfos[1]->getUserIndex());

	AbstractController* collisionBodyControllers[2];
	collisionBodyControllers[0] = objectInfos[0]->getUserPointer();
	collisionBodyControllers[1] = objectInfos[1]->getUserPointer();
	// handle colision events object specific
	auto bikeIterator = std::find(collisionTypes.cbegin(), collisionTypes.cend(), BIKETYPE);

	if (bikeIterator != collisionTypes.cend())
	{
		int bikeIndex = bikeIterator - collisionTypes.cbegin();
		int otherIndex = bikeIndex == 0 ? 1 : 0;
		switch (collisionTypes[otherIndex])
		{
		case SPEEDZONETYPE:
			handleSpeedZone(dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]),
				dynamic_cast<LevelController*>(collisionBodyControllers[otherIndex]),
				dynamic_cast<btGhostObject*>(otherIndex == 0 ? obj1 : obj2));
			break;
		case NAVIGATION_BOUNDARY:
			handleNavigationBoundaryCollision(dynamic_cast<BikeController*>(collisionBodyControllers[bikeIndex]));
			break;

		case WAYPOINTTYPE:
			handleWaypoint(dynamic_cast<RouteController*>(collisionBodyControllers[otherIndex]), dynamic_cast<btGhostObject*>(otherIndex == 0 ? obj1 : obj2));
			break;
		default:
			std::cout << "[GameLogic::triggerEvent] unknown zone" << std::endl;
			break;
		}
	}

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


	bike->registerCollision(impulse);
	m_timeOfWallCollision = g_gameLoopTime;
	//
	// player death
	//
	if (bike->player()->isDead())
	{
		handlePlayerDeath(bike);
	}
}

void GameLogic::handlePlayerDeath(
	BikeController* bike)
{
	bike->player()->increaseDeathCount();
	bike->player()->hudController()->addCrashedMessage();
	m_troenGame->bikeTracker()->recordCrash();
	//handled in bikecontroller::updateModel
	bike->setState(BikeController::BIKESTATE::RESPAWN, g_gameTime);
}


void GameLogic::handleEndZoneCollision(BikeController* bike)
{
	m_troenGame->bikeTracker()->exportTaskStats(g_gameTime);
	bike->player()->routeController()->removeAllFencesFromModel();
	std::cout << "[GameLogic] in endzone" << std::endl;


	bike->setState(BikeController::BIKESTATE::RESPAWN_NEWTRACK, g_gameTime);

	if (!bike->player()->hasNextTrack())
	{
		m_troenGame->bikeTracker()->writeTrajectoryCSV();
		bike->player()->hudController()->addAllRoutesFinishedMessage();
		m_troenGame->countdownTimer()->addTimer(4000, endGame);
	}

}

void GameLogic::handleNavigationBoundaryCollision(BikeController* bike)
{
	m_troenGame->bikeTracker()->recordWrongTurn();
	//bike->getModel()->moveBikeToLastPoint();
	bike->player()->hudController()->addNavigationErrorMessage();
	bike->setState(BikeController::BIKESTATE::RESPAWN, g_gameTime);
}

void GameLogic::handleSpeedZone(BikeController* bike, LevelController* levelController, btGhostObject *ghost)
{
	Speedzone zone = levelController->findSpeedZone(ghost);
	if (zone.maxSpeed == 0)
		printf("speedzone not valid\n");

	bike->player()->setCurrentSpeedLimit(zone.maxSpeed);

#ifdef VERBOSE
	printf("just entered speedzone with limit %d km/h\n", zone.maxSpeed);
#endif

}

void GameLogic::handleWaypoint(RouteController* route, btGhostObject *ghost)
{
	route->registerWaypointCollision(ghost);
#ifdef VERBOSE
	printf("just crossed waypoint");
#endif
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
		if (crossProduct.length() < 0.4)
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

void troen::endGame(void *)
{
	std::cout << "exiting now" << std::endl;
	//stop game loop in next iteration
	g_gameRunning = false;
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
		position.setOrigin(position.getOrigin() + btVector3(0, 0, 10));
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

void GameLogic::checkForTossedPlayers()
{
	for (auto player : m_troenGame->m_players)
	{
		//must have collided with wall in last two seconds, to count this
		if (player->bikeController()->state() == BikeController::DRIVING && g_gameLoopTime - m_timeOfWallCollision < 2000) 
		{
			BikeController* bike = player->bikeController().get();
			btVector3 ypr = bike->getModel()->getEulerYPR();
			if (abs(ypr.getZ()) > PI_2)
			{
				std::cout << "[GameLogic] vehicle crashed and rolled to its death" << std::endl;
				player->increaseHealth(-1 * 1000); //die
				handlePlayerDeath(bike);
			}

		}
	}
}



void GameLogic::checkForFallenPlayers()
{
	for (auto player : m_troenGame->m_players)
	{
		BikeController* bike = player->bikeController().get();
		if (bike->isFalling())
		{
			std::cout << "[GameLogic] player is falling, this should not happen.." << std::endl;
		}
	}
}

