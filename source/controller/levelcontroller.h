#pragma once
// OSG
#include <osg/ref_ptr>
// troen
#include "abstractcontroller.h"
#include "../forwarddeclarations.h"

namespace troen
{
	enum levelModelType
	{
		TROEN,
		BERLIN
	};
	class LevelController : public AbstractController
	{
		friend class LevelModel;
	public:
		LevelController(TroenGame* troenGame, std::string levelName);

		btTransform getSpawnPointForBikeWithIndex(const int index);
		void attachWorld(std::shared_ptr<PhysicsWorld> &world);

		void setBendingFactor(float bendingFactor);
		void setBendingActive(bool active);
		btTransform getRandomSpawnPoint();
		osg::ref_ptr<osg::Group>  getFloorView();
		std::shared_ptr<LevelModel> getAsLevelModel();
		std::shared_ptr<CityModel> getAsCityModel();

		void reload();
		void addBoundaries(std::string path);
		void debugSnapShot();

		void addRigidBodiesToWorld();
		void removeRigidBodiesFromWorld();
		void removeTemporaries(bool walls, bool boundaries);
		void removeBoundaries();
		TroenGame* m_troenGame;
	protected:
		std::shared_ptr<LevelView> m_levelView;
		std::shared_ptr<LevelModel> m_levelModel;
		std::vector<btTransform> m_initialBikePositionTransforms;
		void initializeSpawnPoints();
		void initSpecifics();
		std::weak_ptr<PhysicsWorld> m_world;

		std::string m_levelName;
		levelModelType m_levelType;
	};

}