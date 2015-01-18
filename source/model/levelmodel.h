#pragma once
//troen
#include "../forwarddeclarations.h"
#include "../constants.h"
#include "abstractmodel.h"
#include <btBulletDynamicsCommon.h>

namespace troen
{
	// helper class to abstract boxes from bullet and osg

	class BoxModel {
	public:
		BoxModel(btVector3 centerVec, btVector3 dimensionsVec) {
			center = centerVec;
			dimensions = dimensionsVec;
			rotation = btQuaternion(0, 0, 0, 1);
			name = "default";
			collisionType = LEVELOBSTACLETYPE;

		};

		BoxModel(btVector3 centerVec, btVector3 dimensionsVec, btQuaternion rotationQuat) {
			center = centerVec;
			dimensions = dimensionsVec;
			rotation = rotationQuat;
			name = "default";
			collisionType = LEVELOBSTACLETYPE;
		}

		BoxModel(btVector3 centerVec, btVector3 dimensionsVec, btQuaternion rotationQuat, std::string modelName, troen::COLLISIONTYPE collision_type) {
			center = centerVec;
			dimensions = dimensionsVec;
			rotation = rotationQuat;
			name = modelName;
			collisionType = collision_type;
		}

		btVector3 center;
		btVector3 dimensions;
		btQuaternion rotation;
		std::string name;
		troen::COLLISIONTYPE collisionType;
	};


	class LevelModel : public AbstractModel
	{
		friend LevelController;
	public:
		LevelModel(const LevelController* levelController, std::string levelName);
		virtual ~LevelModel(){};
		virtual const btPoint getLevelSize();
		
		virtual void reload(std::string levelName);
		virtual std::vector<BoxModel>& getFloors() { return m_floors; };
		virtual std::vector<BoxModel>& getObstacles() { return m_obstacles; };
		virtual void initSpecifics();


	protected:
		void attachWorld(std::shared_ptr<PhysicsWorld> &world);
		virtual void addBoxes(std::vector<BoxModel> &boxes, const COLLISIONTYPE type = ABSTRACTTYPE);
		virtual void addFloor(const float yPosition);

		void addObstaclesFromFile(std::string levelName);
		void addObstaclesFromFile(std::string levelName, std::string filePath);
		const LevelController* m_levelController;

		std::vector<BoxModel> m_floors;
		std::vector<BoxModel> m_obstacles;
		std::string m_levelName;
		std::shared_ptr<PhysicsWorld> m_world;
	};

}