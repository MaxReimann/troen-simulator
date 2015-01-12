#include <stack>
#include <osg/Vec3>
#include "routecontroller.h"
// troen
#include "../constants.h"
#include "bikecontroller.h"
#include "../view/routeview.h"
#include "../model/routemodel.h"
#include "../model/physicsworld.h"


using namespace troen;

RouteController::RouteController(
	Player * player,
	btTransform initialTransform) :
AbstractController(),
m_fenceLimitActivated(true)
{
	m_player = player;
	m_model = m_routeModel = std::make_shared<RouteModel>(this);
	m_view = m_routeView = std::shared_ptr<RouteView>(new RouteView(this, player->color(), m_model));

	btQuaternion rotation = initialTransform.getRotation();
	btVector3 position = initialTransform.getOrigin();
	adjustPositionUsingFenceOffset(rotation, position);

	m_lastPosition = position;
}

RouteController::RouteController(
	Player * player,
	btTransform initialTransform,
	Route route) 
{
	m_player = player;
	m_model = m_routeModel = std::make_shared<RouteModel>(this);
	m_view = m_routeView = std::shared_ptr<RouteView>(new RouteView(this, player->color(), m_model));

	btQuaternion rotation = initialTransform.getRotation();
	btVector3 position = initialTransform.getOrigin();
	adjustPositionUsingFenceOffset(rotation, position);

	m_lastPosition = position;
	m_Route = route;
	m_nextPointIndex = 0;
	m_subdividedPoints = m_routeView->subdivide(m_Route.waypoints, 8);
	m_routeView->setupStrips(m_subdividedPoints.size());

	for (int i = 1; i < m_subdividedPoints.size(); i++)
	{
		m_routeView->addFencePart(m_subdividedPoints[i - 1], m_subdividedPoints[i], false);
	}

	
}


void RouteController::update(btVector3 position, btQuaternion rotation)
{
	trackRouteProgress(position);
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

int RouteController::findNearestPointIndex(osg::Vec3 playerPos, double &distance)
{
	int start = m_nextPointIndex;
	int end = m_subdividedPoints.size() - 1;

	const double threshold = 50.0;

	for (int res = 100; res >= 1; res /= 2)
	{
		for (int i = start; i <= end; i += res)
		{
			if (i % (res * 2) == 0) //already checked
				continue;

			distance = (m_subdividedPoints[i] - playerPos).length();
			if (distance < threshold)
			{
				return i;
			}
		}
	}
	return -1;
}


void RouteController::trackRouteProgress(btVector3 playerPosition)
{
	osg::Vec3 next = m_subdividedPoints[m_nextPointIndex];
	osg::Vec3 playerPos = btToOSGVec3(playerPosition);
	double distanceToLine = getDistanceToRouteNormalAt(m_nextPointIndex, playerPos );

	double distanceToPoint = (next - playerPos).length();

	if (distanceToPoint < 150.0 || (distanceToLine < 15.0 && distanceToPoint < 300))
	{
		if (m_nextPointIndex < m_subdividedPoints.size() - 1)
			m_nextPointIndex++;
		else
			std::cout << "finished this route!" << std::endl;
	}
	else{
		double dist;
		int i = findNearestPointIndex(playerPos, dist);
		if (i!=-1)
			m_nextPointIndex = i;

	}

}


void RouteController::attachWorld(std::shared_ptr<PhysicsWorld> &world)
{
	m_world = world;
	m_routeModel->attachWorld(world);
	addEndZone();
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

void RouteController::adjustPositionUsingFenceOffset(const btQuaternion& rotation, btVector3& position)
{
	btVector3 fenceOffset = btVector3(
		0,
		-BIKE_DIMENSIONS.y() / 2,
		BIKE_DIMENSIONS.z() / 2).rotate(rotation.getAxis(), rotation.getAngle()
		);

	position = position - fenceOffset;
}

void RouteController::setLastPosition(const btQuaternion rotation, btVector3 position)
{
	adjustPositionUsingFenceOffset(rotation, position);
	m_lastPosition = position;
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
	int index;
	if (m_nextPointIndex > 0)
		index = m_nextPointIndex - 1;
	else
		index = m_nextPointIndex;

	trans.setOrigin(osgToBtVec3((m_subdividedPoints[index])));

	if (index != m_subdividedPoints.size() - 1)
		vec = m_subdividedPoints[index + 1] - m_subdividedPoints[index];
	else
		vec = m_subdividedPoints[index] - m_subdividedPoints[index - 1];

	double rotAroundZ = PI / 2 - atan(vec.y() / vec.x());
	if (rotAroundZ < 0)
		rotAroundZ += PI;

	btQuaternion rotation;
	rotation.setRotation(btVector3(0, 0, 1), rotAroundZ);

	trans.setRotation(rotation);

	return trans;
}
