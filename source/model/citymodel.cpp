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
#include "../troengame.h"
#include "../model/bikemodel.h"

#include <string>
#include <iostream>
#include <fstream>
#include "qimage.h"

using namespace troen;

CityModel::CityModel(const LevelController* levelController, std::string levelName) : 
LevelModel(levelController,levelName)
{
	m_collisionImage = QImage("data/textures/berlin_binary.png");
	//m_collisionImage.load()
	m_count = 0;
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


btPoint CityModel::getLevelSize()
{
	return btPoint(14065.44, 12721.44); //from blender
}

void CityModel::physicsUpdate()
{
	int w = m_collisionImage.width();
	int h = m_collisionImage.height();

	double lv_w = getLevelSize().first;
	double lv_h = getLevelSize().second;

	double x_offset = -50.0;
	double y_offset = -10.0;
	
	btVector3 pos = m_levelController->m_troenGame->activeBikeModel()->getPositionBt();

	btCollisionShape* shape = m_levelController->m_troenGame->activeBikeModel()->getRigidBody()->getCollisionShape();
	btVector3 halfExtents = ((btBoxShape*)shape)->getHalfExtentsWithMargin();
	int count = 0;
	double xChecks[]{ pos.x() - halfExtents.x(), pos.x(), pos.x() + halfExtents.x()}; 

	for (double x: xChecks)
	{

		btScalar xrel = (x + x_offset+ lv_w / 2.0) / lv_w;
		btScalar yrel = (pos.y() + y_offset + lv_h / 2.0) / lv_h;

		int x_pix = xrel * (double)w;
		int y_pix = yrel * (double)h;

		QRgb pixel = m_collisionImage.pixel(x_pix, y_pix);
		QColor color(pixel);
		if (color.red() + color.green() + color.blue() < 20.0)
			std::cout << ++m_count << std::endl;
	}
}

// static wrapper-function to be able to callback the member function
void CityModel::callbackWrapper(void* pObject)
{
	CityModel* mySelf = (CityModel*)pObject;

	// call member
	mySelf->physicsUpdate();
}