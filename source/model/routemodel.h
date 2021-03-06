#pragma once
// troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../controller/routecontroller.h"

namespace troen
{


	class RouteModel : public AbstractModel
	{
	public:
		RouteModel(RouteController* routeController);
		void attachWorld(std::shared_ptr<PhysicsWorld>& world);

		void addFencePart(const btVector3 a, const btVector3 b);
		void removeFirstFencePart();
		void removeAllFences();
		void addEndZoneCylinder(btVector3 origin, double radius, double height);
		void addSensorsToWorld();
		void addWaypointGhost(btTransform position);
	private:
		RouteController* m_routeController;
		std::weak_ptr<PhysicsWorld> m_world;

		std::deque<std::shared_ptr<btRigidBody>>		m_rigidBodyDeque;
		std::deque<std::shared_ptr<btMotionState>>		m_motionStateDeque;
		std::deque<std::shared_ptr<btCollisionShape>>	m_collisionShapeDeque;

		std::vector<std::shared_ptr<btGhostObject>>		m_ghostObjectList;


	};
}