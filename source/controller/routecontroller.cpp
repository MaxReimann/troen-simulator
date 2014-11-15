#include "routecontroller.h"
// troen
#include "../constants.h"
#include "bikecontroller.h"
#include "../view/routeview.h"
#include "../model/routemodel.h"
#include "../model/physicsworld.h"

#include <osg/Vec3>

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
	Route route) : RouteController(player, initialTransform) //c++11 delegation
{

	m_Route = route;
	m_subdividedPoints = m_routeView->subdivide(m_Route.waypoints, 8);
	m_routeView->setupStrips(m_subdividedPoints.size());

	for (int i = 1; i < m_subdividedPoints.size(); i++)
	{
		m_routeView->addFencePart(m_subdividedPoints[i - 1], m_subdividedPoints[i], false);
	}
}


void RouteController::update(btVector3 position, btQuaternion rotation)
{
	return;
	adjustPositionUsingFenceOffset(rotation, position);
	osg::Vec3 osgPosition = osg::Vec3(position.x(), position.y(), position.z());
	osg::Vec3 osgLastPosition = osg::Vec3(m_lastPosition.x(), m_lastPosition.y(), m_lastPosition.z());
	// add new fence part
	if ((position - m_lastPosition).length() > FENCE_PART_LENGTH)
	{
		//m_routeModel->addFencePart(m_lastPosition, position);
		m_routeView->addFencePart(osgLastPosition,osgPosition);
		m_lastPosition = position;
	}

	// update fence gap
	m_routeView->updateFenceGap(osgLastPosition, osgPosition);
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
