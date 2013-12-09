#include "bikecontroller.h"
// OSG
#include "osg/PositionAttitudeTransform"
//troen
#include "../view/bikeview.h"
#include "../model/bikemodel.h"
#include "../controller/fencecontroller.h"
#include "../model/physicsworld.h"
#include "../sound/audiomanager.h"
#include "../input/keyboard.h"
#include "../input/gamepad.h"
#include "../input/ai.h"

#include <cstdlib>

using namespace troen;

BikeController::BikeController(input::BikeInputState::InputDevice inputDevice)
{
	// TODO change player color here

	m_playerColor = osg::Vec3(
		rand() > RAND_MAX / 2 ? 1 : 0,
		rand() > RAND_MAX / 2 ? 1 : 0,
		rand() > RAND_MAX / 2 ? 1 : 0
	);

	m_view = std::make_shared<BikeView>(m_playerColor);
	m_fenceController = std::make_shared<FenceController>(m_playerColor);

	osg::ref_ptr<osg::Group> viewNode = std::static_pointer_cast<BikeView>(m_view)->getNode();
	m_model = std::make_shared<BikeModel>(viewNode, m_fenceController, this);

	initializeInput(inputDevice);
}

void BikeController::initializeInput(input::BikeInputState::InputDevice inputDevice)
{
	osg::ref_ptr<input::BikeInputState> bikeInputState = new input::BikeInputState();
	setInputState(bikeInputState);

	switch (inputDevice)
	{
	case input::BikeInputState::KEYBOARD:
	{
		m_keyboardHandler = new input::Keyboard(bikeInputState);
		break;
	}
	case input::BikeInputState::GAMEPAD:
	{
		std::shared_ptr<input::Gamepad> gamepad = std::make_shared<input::Gamepad>(bikeInputState);

		if (gamepad->checkConnection())
		{
			std::cout << "[TroenGame::initializeInput] Gamepad connected on port " << gamepad->getPort() << std::endl;
			bikeInputState->setPollingDevice(gamepad);
		}
		else
		{
			std::cout << "[TroenGame::initializeInput] USE_GAMEPAD true but no gamepad connected!" << std::endl;
		}
		break;
	}
	case input::BikeInputState::AI:
	{
		std::shared_ptr<input::AI> ai = std::make_shared<input::AI>(bikeInputState);
		bikeInputState->setPollingDevice(ai);
		break;
	}
	}
}

osg::ref_ptr<input::Keyboard> BikeController::getEventHandler()
{
	return m_keyboardHandler;
}

bool BikeController::hasEventHandler()
{
	return m_keyboardHandler != nullptr;
}

void BikeController::setInputState(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	std::static_pointer_cast<BikeModel>(m_model)->setInputState(bikeInputState);
}

void BikeController::attachTrackingCamera(osg::ref_ptr<osgGA::NodeTrackerManipulator>& manipulator)
{
	osg::Matrixd cameraOffset;

	int debugNormalizer = 1;
#ifdef _DEBUG
	debugNormalizer = -1;
#endif

	cameraOffset.makeTranslate(0, debugNormalizer * 100, -20);

	osg::ref_ptr<osg::Group> viewNode = std::static_pointer_cast<BikeView>(m_view)->getNode();
	osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (viewNode->getChild(0));
	// set the actual node as the track node, not the pat
	manipulator->setTrackNode(pat->getChild(0));
	manipulator->setHomePosition(pat->getPosition(), pat->getPosition() * cameraOffset, osg::Vec3d(0, debugNormalizer * 1, 0));
}

void BikeController::updateModel()
{
	std::static_pointer_cast<BikeModel>(m_model)->updateState();

}

osg::ref_ptr<osg::Group> BikeController::getViewNode()
{

	osg::ref_ptr<osg::Group> group = new osg::Group();
	// TODO (dw) try not to disable culling, by resizing the childrens bounding boxes
	group->setCullingActive(false);
	group->addChild(m_fenceController->getViewNode());
	group->addChild(std::static_pointer_cast<BikeView>(m_view)->getNode());
	return group;
};

void BikeController::attachWorld(std::weak_ptr<PhysicsWorld> &world) {
	world.lock()->addRigidBodies(getRigidBodies());
	m_fenceController->attachWorld(world);
}

void BikeController::removeAllFences()
{
	m_fenceController->removeAllFences();
}

void BikeController::enforceFencePartsLimit(int maxFenceParts)
{
	m_fenceController->enforceFencePartsLimit(maxFenceParts);
}
