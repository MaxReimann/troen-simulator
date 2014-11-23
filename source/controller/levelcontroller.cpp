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

#include "../model/citymodel.h"
#include "../view/cityview.h"

#include "itemcontroller.h"
#include "../troengame.h"


using namespace troen;

LevelController::LevelController(TroenGame* troenGame, std::string levelName) : m_levelName(levelName)
{
	AbstractController();


	m_levelType = BERLIN;
	initSpecifics();

	m_troenGame = troenGame;
	m_currentItemCount = 0;

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
	m_initialBikePositionTransforms.push_back(btTransform(btQuaternion(Z_AXIS, (float)PI * 3.f / 4.f), btVector3(20, 20, BIKE_DIMENSIONS.z() / 2 + 500)));
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
}

void LevelController::removeRigidBodiesFromWorld()
{
	m_world.lock()->removeRigidBodies(getRigidBodies());
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

void LevelController::addItemBox()
{
	float x = randf(0, LEVEL_SIZE) - LEVEL_SIZE / 2;
	float y = randf(0, LEVEL_SIZE) - LEVEL_SIZE / 2;
	btVector3 position(x, y, +0.5);

	// the item controller will remove itself
	new ItemController(position, m_world, m_troenGame, m_levelView.get());

	m_currentItemCount++;
}

void LevelController::update()
{
	if (m_currentItemCount >= m_targetItemCount) {
		return;
		m_targetItemCount = 0;
	}

	// this method is called in each frame, so the amount of items will be refreshed relatively quickly
	// creating all at once would cause a lag

	//! No items for Simulator 
	//addItemBox();
}

void LevelController::setBendingFactor(float bendingFactor)
{
	m_levelView->setBendingFactor(bendingFactor);
}


void LevelController::setBendingActive(bool active)
{
	m_levelView->setBendingActive(active);
}