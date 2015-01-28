#include "bikecontroller.h"
// OSG
#include <osg/PositionAttitudeTransform>
#include <osgViewer/View>
//troen
#include "../constants.h"
#include "../globals.h"
#include "../player.h"

#include "../controller/routecontroller.h"
#include "../controller/hudcontroller.h"
#include "../controller/levelcontroller.h"

#include "../model/physicsworld.h"
#include "../model/bikemodel.h"

#include "../view/bikeview.h"
#include "../view/nodefollowcameramanipulator.h"

#include "../input/keyboard.h"
#include "../input/gamepad.h"
#include "../input/gamepadps4.h"
#include "../input/pollingdevice.h"
#include "../input/ffbwheel.h"

#include "../resourcepool.h"
#include "../sound/audiomanager.h"
#include "../model/bikemotionstate.h"

#include "../util/filteredrayresultcallback.h"
#include "../troengame.h"


using namespace troen;

BikeController::BikeController(
	Player* player,
	const input::BikeInputState::InputDevice& inputDevice,
	const btTransform initialPosition,
	ResourcePool* resourcePool) :
	AbstractController(),
	m_player(player),
	m_keyboardHandler(nullptr),
	m_pollingThread(nullptr),
	m_initialTransform(initialPosition),
	m_state(BIKESTATE::WAITING),
	m_speed(0),
	m_turboInitiated(false),
	m_timeOfLastCollision(-1),
	m_respawnTime(-1),
	m_lastFenceCollision(std::make_pair<float, RouteController*>(0, nullptr))
{
	m_view = m_bikeView = std::make_shared<BikeView>(player->color(), resourcePool);

	osg::ref_ptr<osg::Group> viewNode = m_bikeView->getNode();
	m_model = m_bikeModel = std::make_shared<BikeModel>(m_initialTransform, viewNode, m_player, this);
	initializeInput(inputDevice);
}

void BikeController::reset()
{
	if (m_pollingThread != nullptr)
		m_pollingThread->setVibration(false);

	m_player->setHealth(BIKE_DEFAULT_HEALTH);
	m_timeOfLastCollision = -1;
	btTransform position = player()->routeController()->getLastWayPoint();
	position.setOrigin(position.getOrigin() + btVector3(0, 0, 10.0));
	m_bikeModel->removeRaycastVehicle();
	m_bikeModel->constructVehicleBody(m_world);
	moveBikeToPosition(position);
}

void BikeController::respawnAt(btTransform respawnPoint)
{
	if (m_pollingThread != nullptr)
		m_pollingThread->setVibration(false);
	
	m_initialTransform = respawnPoint;
	moveBikeToPosition(respawnPoint);
}


void BikeController::reloadVehicle()
{
	m_bikeModel->removeRaycastVehicle();
	m_bikeModel->constructVehicleBody(m_world);
	btTransform trans;
	trans.setOrigin(m_initialTransform.getOrigin() + btVector3(0.0, 0.0, 1.0));
	trans.setRotation(m_initialTransform.getRotation());
	respawnAt(trans);
}


void BikeController::registerCollision(const btScalar impulse)
{
	if (impulse > 0) {
		m_timeOfLastCollision = g_gameTime;
	}
	m_player->increaseHealth(-1 * impulse);
}


BikeController::~BikeController()
{
	if (m_pollingThread != nullptr)
	{
		m_pollingThread->stop();
		m_pollingThread->wait();
	}
	if (m_keyboardHandler == nullptr)
	{
		delete m_pollingThread;
	}
}

void BikeController::killThread()
{
	if (m_pollingThread != nullptr)
	{
		m_pollingThread->stop();
		m_pollingThread->wait();
		m_pollingThread = nullptr;
		delete m_pollingThread;
	}
}

void BikeController::initializeInput(input::BikeInputState::InputDevice inputDevice)
{
	osg::ref_ptr<input::BikeInputState> bikeInputState = new input::BikeInputState();
	setInputState(bikeInputState);

	switch (inputDevice)
	{
	case input::BikeInputState::KEYBOARD_wasd:
		initializeWASD(bikeInputState);
		break;
	case input::BikeInputState::KEYBOARD_arrows:
		initializeArrows(bikeInputState);
		break;
#ifdef WIN32
	case input::BikeInputState::GAMEPAD:
		initializeGamepad(bikeInputState);
		break;
	case input::BikeInputState::FFBWHEEL:
		initializeFFBWheel(bikeInputState);
		break;
#endif
	case input::BikeInputState::GAMEPADPS4:
		initializeGamepadPS4(bikeInputState);
		break;
	default:
		break;
	}
}

void BikeController::initializeWASD(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	m_keyboardHandler = new input::Keyboard(bikeInputState, std::vector<osgGA::GUIEventAdapter::KeySymbol>{
		osgGA::GUIEventAdapter::KEY_W,
			osgGA::GUIEventAdapter::KEY_A,
			osgGA::GUIEventAdapter::KEY_S,
			osgGA::GUIEventAdapter::KEY_D,
			osgGA::GUIEventAdapter::KEY_Space,
			osgGA::GUIEventAdapter::KEY_G
	});

	m_pollingThread = m_keyboardHandler.get();
	m_pollingThread->start();
}

void BikeController::initializeArrows(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	m_keyboardHandler = new input::Keyboard(bikeInputState, std::vector<osgGA::GUIEventAdapter::KeySymbol>{
		osgGA::GUIEventAdapter::KEY_Up,
			osgGA::GUIEventAdapter::KEY_Left,
			osgGA::GUIEventAdapter::KEY_Down,
			osgGA::GUIEventAdapter::KEY_Right,
			osgGA::GUIEventAdapter::KEY_Control_R,
			osgGA::GUIEventAdapter::KEY_M,
	});

	m_pollingThread = m_keyboardHandler.get();
	m_pollingThread->start();
}

#ifdef WIN32
void BikeController::initializeGamepad(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	input::Gamepad* gamepad = new input::Gamepad(bikeInputState);

	if (gamepad->checkConnection())
	{
		std::cout << "[TroenGame::initializeInput] Gamepad connected on port " << gamepad->getPort() << std::endl;
	}
	else
	{
		std::cout << "[TroenGame::initializeInput] No gamepad connected!" << std::endl;
	}
	m_pollingThread = gamepad;
	m_pollingThread->start();
}


void BikeController::initializeFFBWheel(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	input::FFBWheel* gamepad = new input::FFBWheel(bikeInputState);

	if (gamepad->checkConnection())
	{
		std::cout << "[TroenGame::initializeInput] FFBWheel connected on port " << gamepad->getPort() << std::endl;
	}
	else
	{
		std::cout << "[TroenGame::initializeInput] No FFBWheel connected!" << std::endl;
	}
	m_pollingThread = gamepad;
	m_pollingThread->start();
}

#endif

void BikeController::initializeGamepadPS4(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	input::GamepadPS4* gamepad = new input::GamepadPS4(bikeInputState, m_player->color());

	if (gamepad->checkConnection())
	{
		std::cout << "[TroenGame::initializeInput] PS4 Controller connected" << std::endl;
	}
	else
	{
		std::cout << "[TroenGame::initializeInput] No PS4 Controller connected!" << std::endl;
	}
	m_pollingThread = gamepad;
	m_pollingThread->start();
}


void BikeController::setInputState(osg::ref_ptr<input::BikeInputState> bikeInputState)
{
	m_bikeInputState = bikeInputState;
	m_bikeModel->setInputState(bikeInputState);
}

void BikeController::attachTrackingCamera(
	std::shared_ptr<HUDController>& hudController)
{
	osg::ref_ptr<osg::Group> viewNode = m_bikeView->getNode();
	osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (viewNode->getChild(0));

	// set the actual node as the track node, not the pat
	if (hudController != nullptr)
		hudController->setTrackNode(pat->getChild(0));
}

void BikeController::attachTrackingCamera(osg::ref_ptr<NodeFollowCameraManipulator>& manipulator)
{
	osg::ref_ptr<osg::Geode> lookat = m_bikeView->getLookatGeode();
	//osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*> (viewNode->getChild(0));

	// set the actual node as the track node, not the pat
	manipulator->setTrackNode(lookat);

	// set the bikeInputState
	manipulator->setBikeInputState(m_bikeInputState);

	// set camera position
	manipulator->setHomePosition(
		CAMERA_EYE_POSITION, // homeEye
		osg::Vec3f(), // homeCenter
		osg::Z_AXIS, // up
		false
		);
}

void BikeController::attachGameView(osg::ref_ptr<osgViewer::View> gameView)
{
	m_gameView = gameView;
}


void BikeController::setFovy(float newFovy)
{
	if (!m_gameView.valid()) return;
	double fovy, aspect, znear, zfar;
	newFovy = min(newFovy, FOVY_INITIAL + FOVY_ADDITION_MAX);
	m_gameView->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, znear, zfar);
	m_gameView->getCamera()->setProjectionMatrixAsPerspective(newFovy, aspect, znear, zfar);
}

float BikeController::getFovy()
{
	if (!m_gameView.valid())
		return 0;
	double fovy, aspect, znear, zfar;
	m_gameView->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, znear, zfar);
	return fovy;
}

float BikeController::computeFovyDelta(float speed, float currentFovy)
{
	long double timeFactor = getTimeFactor();

	m_speed = speed;

	float fovyFactor = (speed - BIKE_VELOCITY_MIN) / (BIKE_VELOCITY_MAX - BIKE_VELOCITY_MIN);
	fovyFactor = fovyFactor > 0 ? fovyFactor : 0;
	float newFovy = FOVY_INITIAL + interpolate(fovyFactor, InterpolateCubed) * FOVY_ADDITION_MAX;

	const float fovyDampening = 20.f;
	float fovyDelta = (newFovy - currentFovy) / fovyDampening * timeFactor;
	return clamp(-FOVY_DELTA_MAX, FOVY_DELTA_MAX, fovyDelta);
}

void BikeController::setState(BIKESTATE newState, double respawnTime /*=-1*/)
{
	if (m_state == newState)
		return;

	m_state = newState;
	m_respawnTime = respawnTime;
}



void BikeController::updateModel(const long double gameTime)
{
	switch (m_state)
	{
	case DRIVING:
	{

		double speed =  m_bikeModel->updateState(gameTime);
		updateFov(speed);
		break;
	}
	case RESPAWN:
	{
		m_bikeModel->freeze();
		updateFov(0);


		if (gameTime > m_respawnTime + RESPAWN_DURATION * 2.f / 3.f)
		{
			m_player->getTroenGame()->levelController()->removeTemporaries(true, false, false);
			reset();
			updateFov(0);
			m_state = RESPAWN_PART_2;
		}
		break;
	}
	case RESPAWN_PART_2:
	{
		if (gameTime > m_respawnTime + RESPAWN_DURATION)
		{
			m_state = DRIVING;
		}
		break;
	}
	case RESPAWN_NEWTRACK:
	{
		 updateFov(0);
		 m_bikeModel->dampOut();
		
		 // fades fence out
		 m_player->routeController()->updateFadeOutFactor(1 - (gameTime - m_respawnTime) / (RESPAWN_DURATION * 2.f / 3.f));

		 if (gameTime > m_respawnTime + RESPAWN_DURATION * 2.f / 3.f)
		 {
			 m_bikeModel->clearDamping();
			 m_player->routeController()->updateFadeOutFactor(1);
			 
			 if (m_player->hasNextTrack())
			 {
				m_state = RESPAWN_PART_2;
				m_player->setOnNextTrack();
			 }
			 else 
				 m_state = WAITING;
		 }
		 break;

	}
	case WAITING_FOR_GAMESTART:
	case WAITING:
		break;
	default:
		break;
	}

	m_player->increaseHealth(getTimeFactor() * 20.0);

	if (m_pollingThread != nullptr)
	{
		m_pollingThread->setVibration(m_timeOfLastCollision != -1 && g_gameTime - m_timeOfLastCollision < VIBRATION_TIME_MS);
	}

	updateUniforms();
}

void BikeController::updateView(const btTransform &worldTrans)
{
	btQuaternion rot = worldTrans.getRotation();
	btVector3 position = worldTrans.getOrigin();

	osg::Vec3 axis = osg::Vec3(rot.getAxis().x(), rot.getAxis().y(), rot.getAxis().z());
	osg::Quat rotationQuat(rot.getAngle(), axis);
	osg::Quat attitude = m_bikeModel->getTilt() * rotationQuat;


	m_bikeView->m_pat->setAttitude(attitude);
	m_bikeView->m_pat->setPosition(osg::Vec3(position.x(), position.y(), position.z()));


	m_player->routeController()->update(position, rot);

}

osg::ref_ptr<osg::Group> BikeController::getViewNode()
{
	osg::ref_ptr<osg::Group> group = m_bikeView->getNode();
	// TODO (dw) try not to disable culling, by resizing the childrens bounding boxes
	//group->setCullingActive(false);
	return group;
};


void BikeController::addUniformsToNode(osg::ref_ptr<osg::Group> group)
{
	// playerNode is the node which holds the rootNode of the entire scene
	// it is used to expose player specific information to the shaders
	// the following is only necessary if a gameView exists for this player

	m_timeOfCollisionUniform = new osg::Uniform("timeSinceLastHit", 100000.f);
	m_velocityUniform = new osg::Uniform("velocity", 0.f);
	m_timeFactorUniform = new osg::Uniform("timeFactor", 1.f);
	m_healthUniform = new osg::Uniform("healthNormalized", m_player->health() / BIKE_DEFAULT_HEALTH);



	osg::ref_ptr<osg::StateSet> stateset = group->getOrCreateStateSet();
	stateset->addUniform(m_timeOfCollisionUniform);
	stateset->addUniform(m_velocityUniform);
	stateset->addUniform(m_timeFactorUniform);
	stateset->addUniform(m_healthUniform);
}

void BikeController::attachWorld(std::shared_ptr<PhysicsWorld> world)
{
	m_world = world;
	m_world->addRigidBodies(getRigidBodies(), COLGROUP_BIKE, COLMASK_BIKE);
	m_bikeModel->constructVehicleBody(world);
}

long double BikeController::getTimeFactor()
{
	long double timeSinceLastUpdate = m_bikeModel->getTimeSinceLastUpdate();
	return timeSinceLastUpdate / 16.7f;
}

void BikeController::moveBikeToPosition(btTransform transform)
{
	m_bikeModel->moveBikeToPosition(transform);
}

void BikeController::updateUniforms()
{
	if (m_player->hasGameView()) {
		m_timeOfCollisionUniform->set((float) g_gameTime - m_timeOfLastCollision);
		m_velocityUniform->set(m_bikeModel->getVelocity());
		m_timeFactorUniform->set((float) getTimeFactor());
		m_healthUniform->set(m_player->health()/BIKE_DEFAULT_HEALTH);
	}

}

void BikeController::updateFov(double speed)
{
	if (m_player->gameView().valid()) {
		float currentFovy = getFovy();
		setFovy(currentFovy + computeFovyDelta(speed, currentFovy));
	}
}



std::shared_ptr<BikeModel>  BikeController::getModel()
{
	return std::static_pointer_cast<BikeModel>(m_model);
}


std::shared_ptr<BikeView>  BikeController::getView()
{
	return std::static_pointer_cast<BikeView>(m_view);
}


float BikeController::getDistanceToObstacle(double angle) {
	// angle specifies the deviation from the current direction the bike is heading

	float rayLength = 10000;

	if (m_world) { // && m_world->()) {

		btDiscreteDynamicsWorld* discreteWorld = m_world->getDiscreteWorld();
		std::shared_ptr<BikeModel> bikeModel = std::static_pointer_cast<BikeModel>(m_model);

		btVector3 from, to, direction;
		direction = bikeModel->getDirection().rotate(btVector3(0, 0, 1), angle);
		from = bikeModel->getPositionBt();
		to = from + direction * rayLength;

		FilteredRayResultCallback RayCallback(bikeModel->getRigidBody().get(), from, to);

		// Perform raycast
		m_world->getMutex()->lock();
		discreteWorld->rayTest(from, to, RayCallback);
		m_world->getMutex()->unlock();

		if (RayCallback.hasHit()) {
			btVector3 collisionPoint;
			collisionPoint = RayCallback.m_hitPointWorld;

			if (RayCallback.m_hitNormalWorld.z() > 0.4){
				// ignore collisions with tilted objects
				return rayLength;
			}

			return (collisionPoint - from).length();
		}
		else {
			return rayLength;
		}
		//g_bulletMutex.unlock();
	}

	return rayLength;
}

bool BikeController::isFalling()
{
	const int fallThreshold = -100;
	return m_bikeModel->getPositionBt().z() < fallThreshold && state() == BikeController::BIKESTATE::DRIVING;
}

