#include "player.h"
// Qt
#include <QString>
// OSG
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ValueObject>
#ifdef WIN32
#include <osgViewer/config/SingleScreen>
#include <osgViewer/config/SingleWindow>
#endif
//troen
#include "constants.h"
#include "globals.h"
#include "gameeventhandler.h"
#include "sampleosgviewer.h"


#include "controller/levelcontroller.h"

#include "input/bikeinputstate.h"

#include "view/skydome.h"
#include "view/nodefollowcameramanipulator.h"

#include "model/bikemodel.h"

#include "navigation/routeparser.h"


using namespace troen;

// spawn point
// resource pool
Player::Player(
	TroenGame* game,
	const std::shared_ptr<GameConfig>& config,
	const int id) :
m_id(id),
m_name(config->playerNames[id].toStdString()),
m_health(BIKE_DEFAULT_HEALTH), //TODO: rename constants
m_points(0),
m_killCount(0),
m_deathCount(0),
m_hasGameView(config->ownView[id])
{

	m_troenGame = game;


	////////////////////////////////////////////////////////////////////////////////
	//
	// Color
	//
	////////////////////////////////////////////////////////////////////////////////

	m_color = osg::Vec3(config->playerColors[id].red(), config->playerColors[id].green(), config->playerColors[id].blue());



	////////////////////////////////////////////////////////////////////////////////
	//
	// Controller
	//
	////////////////////////////////////////////////////////////////////////////////
	std::shared_ptr<RouteParser> parseRoutes = std::make_shared<RouteParser>();
	m_routes = parseRoutes->routes();

	m_currentRoute = 0;
	btTransform initialTransform = m_routes.at(m_currentRoute).getTransform(18);
	initialTransform.setOrigin(initialTransform.getOrigin() + btVector3(0, 0, 10.0));

	m_bikeController = std::make_shared<BikeController>(
		this,
		(input::BikeInputState::InputDevice) config->playerInputTypes[m_id],
		initialTransform,
		game->resourcePool());


	game->levelController()->addBoundaries(m_routes.at(m_currentRoute).filePath);
	m_routeController = std::make_shared<RouteController>(this, initialTransform, m_routes[m_currentRoute]);

	// HUDController must be initialized later, because it
	// can only be created, once all Players are created

	////////////////////////////////////////////////////////////////////////////////
	//
	// View
	//
	////////////////////////////////////////////////////////////////////////////////
	m_cameras = std::make_shared<std::vector<osg::Camera*>>();
	if (config->ownView[m_id])
	{
		m_playerNode = new osg::Group();
		m_bikeController->addUniformsToNode(m_playerNode);

		m_gameView = new osgViewer::View();
		m_gameView->getCamera()->setCullMask(CAMERA_MASK_MAIN);
		m_gameView->setSceneData(m_playerNode);
		m_cameras->push_back(m_gameView->getCamera());

		osg::ref_ptr<NodeFollowCameraManipulator> manipulator
			= new NodeFollowCameraManipulator();

		m_bikeController->attachTrackingCamera(manipulator);
		m_bikeController->attachGameView(m_gameView);

		m_gameView->setCameraManipulator(manipulator.get());
		m_gameView->addEventHandler(game->gameEventHandler());
		m_gameView->addEventHandler(game->statsHandler());
		m_gameView->setUserValue("window_type", (int) MAIN_WINDOW);

		//second window with navigation infos (map/bended views)
		m_navigationWindow = std::make_shared<NavigationWindow>(m_bikeController, game->gameEventHandler());

		//must be called after all 3d cameras have been setup
		setCameraSpecificUniforms();
		

#ifdef WIN32
		if (config->fullscreen)
			m_gameView->apply(new osgViewer::SingleScreen(0));
		else
			m_gameView->apply(new osgViewer::SingleWindow(400, 200, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT));
#else
		if (config->fullscreen)
			m_gameView->setUpViewOnSingleScreen(0);
		else
			m_gameView->setUpViewInWindow(100, 100, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
#endif
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	// Viewer
	//
	////////////////////////////////////////////////////////////////////////////////

	if (config->ownView[m_id])
	{
		m_viewer = new SampleOSGViewer();
		m_viewer->addView(m_gameView);

#ifdef WIN32
		// turn of vSync (since we implement
		// an adaptive gameLoop that syncs itself)
		osg::ref_ptr<RealizeOperation> operation = new RealizeOperation;
		m_viewer->setRealizeOperation(operation);
		m_viewer->realize();

#endif
	}



}


void Player::createHUDController(const std::vector<std::shared_ptr<Player>>& players)
{
	if (!hasGameView()) return;

	m_HUDController = std::make_shared<HUDController>(m_id, players);
	m_bikeController->attachTrackingCamera(m_HUDController);
}

void Player::update(int g_gameTime)
{
	bikeController()->updateModel(g_gameTime);
}

Player::~Player()
{
	//
	// controllers
	//
	m_bikeController.reset();
	m_routeController.reset();
	m_HUDController.reset();

	//
	// osg elements
	//
	m_viewer = nullptr;
	m_gameView = nullptr;
	m_playerNode = nullptr;
}

float Player::increaseHealth(float diff)
{
	m_health = clamp(0, BIKE_DEFAULT_HEALTH, m_health + diff);
	return m_health;
}

float Player::increasePoints(float diff)
{
	m_points += diff;
	return m_points;
}

bool Player::isDead()
{
	return m_health <= 0 && bikeController()->state() == BikeController::BIKESTATE::DRIVING;
}


void Player::setBendingUniform(troen::windowType window, bool value)
{
	m_bendingActivatedUs[window]->set(value);
}

osg::ref_ptr<SampleOSGViewer> Player::navigationViewer()
{
	return m_navigationWindow->navViewer();
}

void  Player::setCameraSpecificUniforms()
{
	m_bendingActivatedUs = uniformVec();
	m_isReflectingUs = uniformVec();

	for (int i = 0; i < m_cameras->size(); i++)
	{
		osg::StateSet* state = m_cameras->at(i)->getOrCreateStateSet();
		
		m_bendingActivatedUs.push_back(new osg::Uniform("bendingActivated", false));
		m_isReflectingUs.push_back(new osg::Uniform("isReflecting", false));

		state->addUniform(m_bendingActivatedUs.back());
		state->addUniform(m_isReflectingUs.back());
	}
}


bool Player::hasNextTrack()
{
	if (m_currentRoute + 1 < m_routes.size())
		return true;
	else
		return false;
}


void Player::setOnNextTrack()
{
		m_routeController->removeAllFences();
		m_troenGame->levelController()->removeTemporaryBoundaries();
		
		m_currentRoute++;
		m_routeController->createTrack(m_routes[m_currentRoute]);
		m_troenGame->levelController()->addBoundaries(m_routes.at(m_currentRoute).filePath);

		btTransform position = m_routeController->getFirstWayPoint();
		position.setOrigin(position.getOrigin() + btVector3(0, 0, 10));


		m_bikeController->respawnAt(position);

}
