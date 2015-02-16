#pragma once
// OSG
#include <osg/ref_ptr>
// troen
#include "abstractcontroller.h"
#include "../forwarddeclarations.h"
#include "../model/levelmodel.h"

namespace troen
{
	enum levelModelType
	{
		TROEN,
		BERLIN
	};

	struct Speedzone
	{
		int speedZoneIndex;
		int maxSpeed;
		btCollisionObject *sensorObject;
	};

	class LevelController : public AbstractController
	{
		friend class LevelModel;
	public:
		LevelController(TroenGame* troenGame, std::string levelName, bool texturedModel);

		btTransform getSpawnPointForBikeWithIndex(const int index);
		void attachWorld(std::shared_ptr<PhysicsWorld> &world);

		void setBendingFactor(float bendingFactor);
		void setBendingActive(bool active);
		btTransform getRandomSpawnPoint();
		osg::ref_ptr<osg::Group>  getFloorView();
		std::shared_ptr<LevelModel> getAsLevelModel();
		std::shared_ptr<CityModel> getAsCityModel();
		std::shared_ptr<CityView> getAsCityView();
		std::vector<BoxModel> getSpeedZones(std::string path);
		osg::ref_ptr<osg::Group> getNaviView();

		void addSpeedZones(std::string filePath);
		Speedzone findSpeedZone(btGhostObject *obj);

		void reload();
		void addBoundaries(std::string path);
		void debugSnapShot();

		void addRigidBodiesToWorld();
		void removeRigidBodiesFromWorld();
		void removeTemporaries(bool walls=true, bool boundaries=true, bool speedZones = true);
		TroenGame* m_troenGame;
	protected:
		std::shared_ptr<LevelView> m_levelView;
		std::shared_ptr<LevelModel> m_levelModel;
		std::vector<btTransform> m_initialBikePositionTransforms;
		void initializeSpawnPoints();
		void initSpecifics(bool texturedModel);
		std::weak_ptr<PhysicsWorld> m_world;

		std::string m_levelName;
		levelModelType m_levelType;
	};

}