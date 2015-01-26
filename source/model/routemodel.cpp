#include "routemodel.h"
// troen
#include "../constants.h"
#include "physicsworld.h"
#include "objectinfo.h"
#include "../controller/routecontroller.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


using namespace troen;

RouteModel::RouteModel(RouteController* routeController)
{
	AbstractModel();
	m_routeController = routeController;
	m_rigidBodyDeque = std::deque<std::shared_ptr<btRigidBody>>();
}

void RouteModel::attachWorld(std::shared_ptr<PhysicsWorld>& world)
{
	m_world = world;
	addRigidBodiesToWorld();
}

void RouteModel::addRigidBodiesToWorld()
{
	for (auto boundary : m_rigidBodyDeque)
		m_world.lock()->addRigidBody(boundary.get());

}



void RouteModel::removeAllFences()
{
	for (auto rigidBody : m_rigidBodyDeque)
		m_world.lock()->removeRigidBody(rigidBody.get());
	m_rigidBodyDeque.clear();
	m_motionStateDeque.clear();
	m_collisionShapeDeque.clear();


}


void RouteModel::removeFirstFencePart()
{
	m_world.lock()->removeRigidBody(m_rigidBodyDeque.front().get());
	m_rigidBodyDeque.pop_front();
	m_motionStateDeque.pop_front();
	m_collisionShapeDeque.pop_front();
}


void RouteModel::addEndZoneCylinder(btVector3 origin, double radius, double height)
{
	btVector3 dimensions(radius, height / 2.0, radius);
	btQuaternion rot;
	rot.setRotation(btVector3(0, 0, 1), PI_2);

	std::shared_ptr<btCylinderShapeZ> zoneShape = std::make_shared<btCylinderShapeZ>(dimensions);
	std::shared_ptr<btDefaultMotionState> zoneMotionState
		= std::make_shared<btDefaultMotionState>(btTransform(rot, origin));

	btRigidBody::btRigidBodyConstructionInfo
		zoneRigidBodyCI(btScalar(0), zoneMotionState.get(), zoneShape.get(), btVector3(0, 0, 0));

	std::shared_ptr<btRigidBody> zoneRigidBody = std::make_shared<btRigidBody>(zoneRigidBodyCI);


	ObjectInfo* info = new ObjectInfo(m_routeController, ENDZONETYPE);
	zoneRigidBody->setUserPointer(info);

	m_collisionShapeDeque.push_back(zoneShape);
	m_motionStateDeque.push_back(zoneMotionState);
	m_rigidBodyDeque.push_back(zoneRigidBody);

}


void RouteModel::addFencePart(btVector3 a, btVector3 b)
{
	btVector3 fenceVector = b - a;

	const btVector3 yAxis = btVector3(0, 1, 0);
	btScalar angle = fenceVector.angle(-yAxis);
	const btScalar inverseAngle = fenceVector.angle(yAxis);

	btQuaternion rotationQuatXY;
	btVector3 axis;
	if (angle != 0 && inverseAngle != 0) {
		// we need to make sure the angle is lower than PI_2
		if (angle < PI_2)
			axis = fenceVector.cross(yAxis).normalized();
		else {
			angle = fenceVector.angle(yAxis);
			axis = fenceVector.cross(-yAxis).normalized();
		}
		rotationQuatXY = btQuaternion(axis, angle);
	}
	else {
		rotationQuatXY = btQuaternion(0, 0, 0, 1);
	}

	std::shared_ptr<btBoxShape> fenceShape = std::make_shared<btBoxShape>(btVector3(FENCE_PART_WIDTH / 2, fenceVector.length() / 2, ROUTE_WIDTH_MODEL / 2));
	std::shared_ptr<btDefaultMotionState> fenceMotionState = std::make_shared<btDefaultMotionState>(btTransform(rotationQuatXY, (a + b) / 2 + btVector3(0, 0, ROUTE_WIDTH_MODEL / 2)));

	const btScalar mass = 0;
	const btVector3 fenceInertia(0, 0, 0);

	btRigidBody::btRigidBodyConstructionInfo m_fenceRigidBodyCI(mass, fenceMotionState.get(), fenceShape.get(), fenceInertia);

	std::shared_ptr<btRigidBody> fenceRigidBody = std::make_shared<btRigidBody>(m_fenceRigidBodyCI);

	ObjectInfo* info = new ObjectInfo(m_routeController, FENCETYPE);
	fenceRigidBody->setUserPointer(info);

	m_collisionShapeDeque.push_back(fenceShape);
	m_motionStateDeque.push_back(fenceMotionState);
	m_rigidBodyDeque.push_back(fenceRigidBody);

	m_world.lock()->addRigidBody(fenceRigidBody.get());
}