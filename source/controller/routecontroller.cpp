#include <stack>
#include <osg/Vec3>
#include "routecontroller.h"
// troen
#include "../constants.h"
#include "bikecontroller.h"
#include "../view/routeview.h"
#include "../model/routemodel.h"
#include "../model/physicsworld.h"
#include "../model/levelmodel.h"
#include <assert.h>


using namespace troen;

RouteController::RouteController(
	Player * player,
	Route route) 
{
	m_player = player;
	m_model = m_routeModel = std::make_shared<RouteModel>(this);
	m_view = m_routeView = std::shared_ptr<RouteView>(new RouteView(this, player->color(), m_model));

	createTrack(route);
	
}

void RouteController::createTrack(Route route)
{
	m_Route = route;
	m_nextPointIndex = 0;
	m_subdividedPoints = m_routeView->subdivide(m_Route.waypoints, 8);
	m_routeView->setupStrips(m_subdividedPoints.size());

	m_lastTouchedWaypoint = osgToBtVec3((m_Route.waypoints[0]));
	for (int i = 0; i < m_Route.waypoints.size(); i++)
	{
		m_routeModel->addWaypointGhost(m_Route.getTransform(i));
	}

	for (int i = 1; i < m_subdividedPoints.size(); i++)
	{
		m_routeView->addFencePart(m_subdividedPoints[i - 1], m_subdividedPoints[i], false);
	}

	addEndZone();

	if (m_world.use_count()>0) //world already attached
	{
		std::cout << "adding rigidbodies" << std::endl;
		m_routeModel->addSensorsToWorld();
	}

	osg::Vec3 p = m_subdividedPoints[100];
	btTransform t;
	t.setIdentity(); t.setOrigin(osgToBtVec3(p));
}


void RouteController::update(btVector3 position, btQuaternion rotation)
{
	//trackRouteProgress(position);
	m_routeView->m_playerPositionUniform->set(btToOSGVec3(position));
}

double RouteController::getDistanceToRouteNormalAt(int curPointIndex, osg::Vec3 playerPosition)
{

	osg::Vec3 pre, post;
	osg::Vec3 next = m_subdividedPoints[curPointIndex];
	if (curPointIndex > 0)
		pre = m_subdividedPoints[curPointIndex - 1];
	else
		pre = next;

	if (curPointIndex < m_subdividedPoints.size() - 1)
		post = m_subdividedPoints[curPointIndex + 1];
	else
		post = next;

	osg::Vec3 normal = (post - pre) ^ osg::Vec3(0, 0, 1);
	//point on normal line through m_nextPointIndex point
	osg::Vec3 p1 = next + normal;
	//dist: (| (x_2 - x_1)x(x_1 - x_0) | ) / (| x_2 - x_1 | )
	double distanceToLine = (normal ^ (next - playerPosition)).length() / normal.length();

	return distanceToLine;
}

//find index of point in subdivided points nearest to findpoint
int RouteController::findNearestPointIndex(btVector3 findPoint)
{
	float minDist = 10000000000000000;
	int findIndex = -1;
	
	for (int i = 0; i< m_subdividedPoints.size(); i++)
	{
		float dist = (m_subdividedPoints[i] - btToOSGVec3(findPoint)).length();
		if (dist < minDist)
		{
			findIndex = i;
			minDist = dist;
		}
	}

	return findIndex;
}

void RouteController::registerWaypointCollision(btGhostObject *waypointZone)
{
	m_lastTouchedWaypoint = waypointZone->getWorldTransform().getOrigin();
}


void RouteController::attachWorld(std::shared_ptr<PhysicsWorld> &world)
{
	m_world = world;
	m_routeModel->attachWorld(world);
}

void RouteController::removeAllFences()
{
	m_routeModel->removeAllFences();
	m_routeView->removeAllFences();
}

void RouteController::removeAllFencesFromModel()
{
	m_routeModel->removeAllFences();
}

void RouteController::setLimitFence(bool boolean)
{
	m_fenceLimitActivated = boolean;
}

int RouteController::getFenceLimit() {
	if (m_fenceLimitActivated)
		return m_player->points();
	else
		return 0;
}




void RouteController::showFencesInRadarForPlayer(const int id)
{
	m_routeView->showFencesInRadarForPlayer(id);
}

void RouteController::hideFencesInRadarForPlayer(const int id)
{
	m_routeView->hideFencesInRadarForPlayer(id);
}

osg::ref_ptr<osg::Group> RouteController::getViewNode()
{
	osg::ref_ptr<osg::Group> group = m_routeView->getNode();
	// TODO (dw) try not to disable culling, by resizing the childrens bounding boxes
	group->setCullingActive(false);
	return group;
}

void RouteController::updateFadeOutFactor(float fadeOutFactor)
{
	m_routeView->updateFadeOutFactor(fadeOutFactor);
}

btTransform RouteController::getFirstWayPoint()
{
	return m_Route.getTransform(0);
}


void RouteController::addEndZone()
{
	double radius = 50.0;
	double height = 100.0;
	osg::Vec3 origin = m_subdividedPoints.back();
	m_routeView->addEndZoneCylinder(origin, radius, height);
	m_routeModel->addEndZoneCylinder(osgToBtVec3((origin + osg::Vec3(0, 0, height / 2.0))), radius, height);
}


btTransform RouteController::getLastWayPoint()
{
	btTransform trans;
	osg::Vec3 vec;
	int index = findNearestPointIndex(m_lastTouchedWaypoint);
	assert(index != -1);

	trans.setOrigin(osgToBtVec3((m_subdividedPoints[index])));

	if (index != m_subdividedPoints.size() - 1)
		vec = m_subdividedPoints[index + 1] - m_subdividedPoints[index];
	else
		vec = m_subdividedPoints[index] - m_subdividedPoints[index - 1];

	vec.normalize();
	btQuaternion rotation;
	double rotAroundZ = atan2(vec.y(),vec.x()) - PI/2;
	rotation.setRotation(btVector3(0, 0, 1), rotAroundZ);
	//auto rotation = fromTwoVectors(btVector3(0, 1, 0), osgToBtVec3(vec));
	//rotation.setRotation(btVector3(0, 0, 1), rotation.getAngle());
	//std::cout << vecToString(vec) << std::endl;
	//btQuaternion rotation = generateRotationFromDirectionVector(osgToBtVec3((vec)));



	//btQuaternion rotation;
	//rotation.setRotation(btVector3(0, 0, 1), rotAroundZ);

	trans.setRotation(rotation);

	return trans;
}
