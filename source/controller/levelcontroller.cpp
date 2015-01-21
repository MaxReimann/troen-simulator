#include "levelcontroller.h"
#include <typeinfo>
// OSG
#include <osg/Group>
// bullet
#include <btBulletDynamicsCommon.h>
// troen
#include "../constants.h"
#include "../model/LevelModel.h"
#include "../view/LevelView.h"
#include "../model/physicsworld.h"


#include "../view/cityview.h"

#include "../troengame.h"
#include "../model/objectinfo.h"


using namespace troen;

LevelController::LevelController(TroenGame* troenGame, std::string levelName) : m_levelName(levelName)
{
	AbstractController();


	m_levelType = BERLIN;
	initSpecifics();

	m_troenGame = troenGame;

	initializeSpawnPoints();
}

void LevelController::initSpecifics()
{
	if (m_levelType == BERLIN)
	{
		m_model = m_levelModel = std::make_shared<CityModel>(this, m_levelName);
		m_view = m_levelView = std::make_shared<CityView>(m_levelModel, m_levelName);

	}
	else
	{
		m_model = m_levelModel = std::make_shared<LevelModel>(this, m_levelName);
		m_view = m_levelView = std::make_shared<LevelView>(m_levelModel, m_levelName);
	}

	m_levelModel->initSpecifics(); //loading of model specific data
	m_levelView->initSpecifics(m_levelModel);


}

void LevelController::reload()
{
	removeRigidBodiesFromWorld();
	m_levelModel->reload(m_levelName);
	addRigidBodiesToWorld();

	m_levelView->reload(m_levelName);
}

btTransform LevelController::getSpawnPointForBikeWithIndex(int index)
{
	return m_initialBikePositionTransforms[index];
}

btTransform LevelController::getRandomSpawnPoint()
{
	int index = randf(0, m_initialBikePositionTransforms.size() - 1);
	return m_initialBikePositionTransforms[index];
}

void LevelController::initializeSpawnPoints()
{
	btVector3 Z_AXIS(0, 0, 1);
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS, (float)PI * 3.f / 4.f), btVector3(-181, 379, BIKE_DIMENSIONS.z() / 2 + 100)));
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS, (float)PI * 1.f / 4.f), btVector3(20, -20, BIKE_DIMENSIONS.z() / 2 + 500)));
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS, (float)-PI * 1.f / 4.f), btVector3(-20, -20, BIKE_DIMENSIONS.z() / 2 + 500)));
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS, (float)-PI * 3.f / 4.f), btVector3(-20, 20, BIKE_DIMENSIONS.z() / 2 + 500)));
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS,0), btVector3(100, 100, BIKE_DIMENSIONS.z() / 2 + 500)));
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS,0), btVector3(-100, -100, BIKE_DIMENSIONS.z() / 2 + 500)));
}

osg::ref_ptr<osg::Group>  LevelController::getFloorView()
{
	return m_levelView->getFloor();
}

void LevelController::attachWorld(std::shared_ptr<PhysicsWorld> &world)
{
	m_world = world;
	getAsLevelModel()->attachWorld(world);
}

void LevelController::addBoundaries(std::string path)
{
	if (path.find("bounds") == std::string::npos)
	{
		//change ending
		int i = path.length() - 1;
		while (path.at(i) != '.' && i >= 0) i--;
		path = path.substr(0, i+1)+"bounds";
		std::cout << "path: " << path << std::endl;

	}
	m_levelModel->addObstaclesFromFile("", path);
}

void LevelController::removeTemporaries(bool walls, bool boundaries)
{
	if (walls)
		getAsCityModel()->clearTemporaryWalls();
	if (boundaries)
		removeBoundaries();
}

void LevelController::removeBoundaries()
{
	auto bodies = getRigidBodies();
	auto bodyIter = std::begin(bodies);
	while (bodyIter != std::end(bodies))
	{
		ObjectInfo *info = static_cast<ObjectInfo *>(bodyIter->get()->getUserPointer());
		if (static_cast<COLLISIONTYPE>(info->getUserIndex()) != LEVELGROUNDTYPE)
		{
			m_world.lock()->removeRigidBody(bodyIter->get());
			bodyIter = bodies.erase(bodyIter);
		}
		else
			++bodyIter;
	}
}

void LevelController::removeRigidBodiesFromWorld()
{
	m_world.lock()->removeRigidBodies(getRigidBodies());
	getRigidBodies().clear();
	
}

void LevelController::addRigidBodiesToWorld()
{
	m_world.lock()->addRigidBodies(getRigidBodies(), COLGROUP_LEVEL, COLMASK_LEVEL);

	if (m_levelType == BERLIN)
	{
		m_world.lock()->registerCustomCallback(
			(void*)m_levelModel.get(), CityModel::callbackWrapper);
	}
}




void LevelController::setBendingFactor(float bendingFactor)
{
	m_levelView->setBendingFactor(bendingFactor);
}


void LevelController::setBendingActive(bool active)
{
	m_levelView->setBendingActive(active);
}

void LevelController::debugSnapShot()
{
	if (m_levelType == BERLIN)
	{
		getAsCityModel()->m_key_event = true;
	}
}

std::shared_ptr<LevelModel> LevelController::getAsLevelModel()
{
	return std::dynamic_pointer_cast<LevelModel>(m_model);
}

std::shared_ptr<CityModel> LevelController::getAsCityModel()
{
	return std::dynamic_pointer_cast<CityModel>(m_model);
}


