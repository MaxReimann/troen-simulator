#include "citymodel.h"
//bullet
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btHashMap.h"
//troen
#include "../controller/itemcontroller.h"
#include "objectinfo.h"
#include "../constants.h"
#include "../controller/abstractcontroller.h"
#include "../controller/levelcontroller.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace troen;

CityModel::CityModel(const LevelController* levelController, std::string levelName) : 
LevelModel(levelController,levelName)
{
}

void CityModel::initSpecifics()
{
	addFloor(-10);
}

void CityModel::reload(std::string levelName)
{
	m_rigidBodies.clear();
	m_motionStates.clear();
	m_collisionShapes.clear();
	m_floors.clear();

	addFloor(-10);
}


btScalar CityModel::getLevelSize()
{
	return btScalar(13000);
}

void CityModel::physicsUpdate()
{

}

// static wrapper-function to be able to callback the member function
void CityModel::callbackWrapper(void* pObject)
{
	CityModel* mySelf = (CityModel*)pObject;

	// call member
	mySelf->physicsUpdate();
}