#pragma once
// OSG
#include <osg/ref_ptr>
// troen
#include "abstractcontroller.h"
#include "../forwarddeclarations.h"
#include "itemcontroller.h"

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
		void addItemBox();
		void update();

		void setBendingFactor(float bendingFactor);
		void setBendingActive(bool active);
		btTransform getRandomSpawnPoint();
		osg::ref_ptr<osg::Group>  getFloorView();

		void addRigidBodiesToWorld();
		void removeRigidBodiesFromWorld();
		void reload();

		TroenGame* m_troenGame;
		void debugSnapShot();
	protected:
		std::shared_ptr<LevelView> m_levelView;
		std::shared_ptr<LevelModel> m_levelModel;
		std::vector<btTransform> m_initialBikePositionTransforms;
		void initializeSpawnPoints();
		void initSpecifics();
		std::weak_ptr<PhysicsWorld> m_world;

		std::string m_levelName;
		int m_currentItemCount;
		int m_targetItemCount = 100;
		levelModelType m_levelType;
	};

}