#pragma once
// bullet
#include <btBulletDynamicsCommon.h>
//troen
#include "../forwarddeclarations.h"
#include "abstractcontroller.h"
#include "../player.h"

#include <osg/Vec3>
#include "../navigation/routeparser.h"
#include "../model/levelmodel.h"

namespace troen
{

	class RouteController : public AbstractController
	{
	public:
		RouteController(Player * player, Route route);
		void update(btVector3 position, btQuaternion rotation);
		void attachWorld(std::shared_ptr<PhysicsWorld>& world);

		osg::ref_ptr<osg::Group> getViewNode() override;

		Player* player() { return m_player; };

		// fence manipulation
		void removeAllFences();
		void removeAllFencesFromModel();
		void setLimitFence(bool boolean);
		int getFenceLimit();

		// radar visibility
		void showFencesInRadarForPlayer(const int id);
		void hideFencesInRadarForPlayer(const int id);

		void updateFadeOutFactor(float fadeOutFactor);
		void setBendingActive(bool active);
		btTransform getFirstWayPoint();
		btTransform getLastWayPoint();
		void createTrack(Route route);
		void registerWaypointCollision(btGhostObject *waypointZone);


	private:
		std::shared_ptr<RouteView> m_routeView;
		std::shared_ptr<RouteModel> m_routeModel;

		double getDistanceToRouteNormalAt(int curPointIndex, osg::Vec3 playerPosition);
		int findNearestPointIndex(btVector3 findPoint);
		void addEndZone();
		bool m_fenceLimitActivated;

		Player * m_player;
		std::weak_ptr<PhysicsWorld> m_world;
		Route m_Route;
		std::vector<osg::Vec3> m_subdividedPoints;
		int m_nextPointIndex;
		btVector3 m_lastTouchedWaypoint;
	};
}