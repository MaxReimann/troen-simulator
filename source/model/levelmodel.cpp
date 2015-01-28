#include "levelmodel.h"
//bullet
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btHashMap.h"
//troen
#include "objectinfo.h"
#include "../constants.h"
#include "../controller/abstractcontroller.h"
#include "../controller/levelcontroller.h"
//#include <btHeightfieldTerrainShape.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace troen;
extern char MyHeightfield[];

LevelModel::LevelModel(LevelController* levelController, std::string levelName)
{
	AbstractModel();
	m_levelController = levelController;
	m_levelName = levelName;
	m_rigidBodies = std::vector<std::shared_ptr<btRigidBody>>();
}

void LevelModel::initSpecifics()
{
	addFloor(-10);
	addObstaclesFromFile(m_levelName);
}

void LevelModel::reload(std::string levelName)
{
	m_rigidBodies.clear();
	m_motionStates.clear();
	m_collisionShapes.clear();
	m_obstacles.clear();
	m_floors.clear();

	addFloor(-10);
	addObstaclesFromFile(levelName);
}

void LevelModel::addObstaclesFromFile(std::string levelName)
{
	addObstaclesFromFile(levelName, "data/levels/" + levelName + ".level");
}

void LevelModel::attachWorld(std::shared_ptr<PhysicsWorld> &world)
{
	m_world = world;
}


void LevelModel::addObstaclesFromFile(std::string levelName, std::string filePath)
{

	std::vector<BoxModel> newObstacles = parseLevelFile(filePath, false);
	m_obstacles.insert(m_obstacles.end(), newObstacles.begin(), newObstacles.end());
	addBoxes(m_obstacles);
}


std::vector<BoxModel> LevelModel::parseLevelFile(std::string filePath, bool useSpeedLimit)
{

	std::string line;
	std::ifstream input(filePath);
	std::vector<BoxModel> newObstacles;

	btVector3 center, dimensions;
	btQuaternion rotation;
	std::string name, collisionTypeString;
	

	while (std::getline(input, line)) {

		double x, y, z, w;

		QString qLine;

		int speed;

		// center

		qLine = QString::fromStdString(line);
		x = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		y = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		z = qLine.toDouble();

		center = btVector3(x, y, z);


		// dimensions

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		x = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		y = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		z = qLine.toDouble();

		dimensions = btVector3(x, y, z);


		// rotation

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		x = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		y = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		z = qLine.toDouble();

		std::getline(input, line);
		qLine = QString::fromStdString(line);
		w = qLine.toDouble();

		rotation = btQuaternion(x, y, z, w);


		// name, collisionType

		std::getline(input, line);
		name = line;

		std::getline(input, line);
		collisionTypeString = line;

		const std::string collisionTypes[10] = { "ABSTRACTTYPE", "BIKETYPE", "LEVELTYPE", "LEVELWALLTYPE", "LEVELGROUNDTYPE", "LEVELOBSTACLETYPE", "FENCETYPE", "ITEMTYPE", "ZONETYPE", "NAVIGATION_BOUNDARY" };
		int index = 0;
		for (auto type : collisionTypes) {
			if (type == collisionTypeString)
				break;
			index++;
		}

		troen::COLLISIONTYPE collisionType = static_cast<troen::COLLISIONTYPE>(index);

		BoxModel newBox(center, dimensions, rotation, name, collisionType);

		if (useSpeedLimit)
		{
			std::getline(input, line);
			qLine = QString::fromStdString(line);
			speed = qLine.toInt();
			newBox.speedLimit = speed;
		}

		newObstacles.push_back(newBox);
	}

	return newObstacles;
}

void LevelModel::addFloor(float yPosition)
{
	btPoint size = getLevelSize();
	m_floors.push_back({
		btVector3(0, 0, yPosition-10),
		btVector3(size.first, size.second, 20)
	});

	addBoxes(m_floors, LEVELGROUNDTYPE);


	btScalar maxHeight = 100;

	bool useFloatDatam = false;
	bool flipQuadEdges = false;

	std::shared_ptr<btHeightfieldTerrainShape> heightFieldShape = std::make_shared<btHeightfieldTerrainShape>(128, 128, MyHeightfield, maxHeight, 2, useFloatDatam, flipQuadEdges);;
	btVector3 mmin, mmax;
	heightFieldShape->getAabb(btTransform::getIdentity(), mmin, mmax);


	heightFieldShape->setUseDiamondSubdivision(true);

	btVector3 localScaling(size.first / 128.0, size.second / 128.0, 1);
	heightFieldShape->setLocalScaling(localScaling);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(0, 0, 50));
	std::shared_ptr<btDefaultMotionState> groundMotionState
		= std::make_shared<btDefaultMotionState>(trans);

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(btScalar(0), groundMotionState.get(), heightFieldShape.get(), btVector3(0, 0, 0));

	std::shared_ptr<btRigidBody> groundRigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);

	//create ground object

	ObjectInfo*  info = new ObjectInfo(const_cast<LevelController*>(m_levelController), LEVELGROUNDTYPE);

	groundRigidBody->setUserPointer(info);

	m_collisionShapes.push_back(heightFieldShape);
	m_motionStates.push_back(groundMotionState);
	m_rigidBodies.push_back(groundRigidBody);
}


void LevelModel::addBoxes(std::vector<BoxModel> &boxes, COLLISIONTYPE type)
{
	for (int i = 0; i < boxes.size(); ++i)
	{
		std::shared_ptr<btBoxShape> wallShape = std::make_shared<btBoxShape>(boxes[i].dimensions / 2);
		std::shared_ptr<btDefaultMotionState> wallMotionState
			= std::make_shared<btDefaultMotionState>(btTransform(boxes[i].rotation, boxes[i].center));

		btRigidBody::btRigidBodyConstructionInfo
			wallRigidBodyCI(btScalar(0), wallMotionState.get(), wallShape.get(), btVector3(0, 0, 0));

		std::shared_ptr<btRigidBody> wallRigidBody = std::make_shared<btRigidBody>(wallRigidBodyCI);

		ObjectInfo* info;
		if (type == ABSTRACTTYPE) {
			info = new ObjectInfo(const_cast<LevelController*>(m_levelController), boxes[i].collisionType);
		}
		else {
			info = new ObjectInfo(const_cast<LevelController*>(m_levelController), type);
		}

		if (type == NAVIGATION_BOUNDARY)
		{
			printf("cf nocont\n");
			wallRigidBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		wallRigidBody->setUserPointer(info);

		m_collisionShapes.push_back(wallShape);
		m_motionStates.push_back(wallMotionState);
		m_rigidBodies.push_back(wallRigidBody);
	}
}

const btPoint LevelModel::getLevelSize()
{
	return btPoint(LEVEL_SIZE, LEVEL_SIZE);
}
