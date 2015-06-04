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
#include "model/levelmodel.h"

#include "navigation/routeparser.h"
#include "osgGA/GUIEventAdapter"
#include "util/countdowntimer.h"


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
m_currentSpeedLimit(30),
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

	m_currentRoute =0;
	m_routeController = std::make_shared<RouteController>(this, m_routes[m_currentRoute]);
	
	btTransform initialTransform = m_routeController->getFirstWayPoint();
	initialTransform.setOrigin(initialTransform.getOrigin() + btVector3(0, 0, 11));

	m_bikeController = std::make_shared<BikeController>(
		this,
		(input::InputDevice) config->playerInputTypes[m_id],
		initialTransform,
		game->resourcePool());


	std::string routesPath = m_routes.at(m_currentRoute).filePath;
	game->levelController()->addBoundaries(routesPath);
	game->levelController()->addSpeedZones(routesPath);

	// HUDController must be initialized later, because it
	// can only be created, once all Players are created

	////////////////////////////////////////////////////////////////////////////////
	//
	// Viewer
	//
	////////////////////////////////////////////////////////////////////////////////

	m_viewer = new SampleOSGViewer();

	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if (!wsi)
	{
		osg::notify(osg::NOTICE) << "Error, no WindowSystemInterface available, cannot create windows." << std::endl;
	}
	unsigned int width, height;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);


	////////////////////////////////////////////////////////////////////////////////
	//
	// View
	//
	////////////////////////////////////////////////////////////////////////////////
	m_cameras = std::make_shared<std::vector<osg::Camera*>>();
	m_cameras->resize(2);
	m_playerNode = new osg::Group();
	m_bikeController->addUniformsToNode(m_playerNode);

	m_gameView = new osgViewer::View();
	m_viewer->addView(m_gameView);

	m_gameView->getCamera()->setCullMask(CAMERA_MASK_MAIN);

	if (!USE_CULLING)
		m_gameView->getCamera()->setCullingMode(osg::CullStack::NO_CULLING); //culling results in strong framerate loss at sudden peaks..

	m_gameView->setSceneData(m_playerNode);
	m_cameras->at(MAIN_WINDOW) = m_gameView->getCamera();

	m_cameraManipulator = new NodeFollowCameraManipulator();

	m_bikeController->attachTrackingCamera(m_cameraManipulator);
	m_bikeController->attachGameView(m_gameView);

	m_gameView->setCameraManipulator(m_cameraManipulator.get());
	m_gameView->addEventHandler(game->gameEventHandler());
	m_gameView->addEventHandler(game->statsHandler());
	m_gameView->setUserValue("window_type", (int) MAIN_WINDOW);
    
	//second window with navigation infos (map/bended views)
	auto viewport = new osg::Viewport(width * WINDOW_RATIO_FULLSCREEN, height / 2.0 - height / 6.0, width * (1.0 - WINDOW_RATIO_FULLSCREEN), height / 3.0);
	m_navigationWindow = std::make_shared<NavigationWindow>(m_bikeController, game->gameEventHandler(), m_viewer, viewport, config->fullscreen);
	m_cameras->at(NAVIGATION_WINDOW) = m_navigationWindow->mapView()->getCamera();

	//must be called after all 3d cameras have been setup
	setCameraSpecificUniforms();

	if (config->fullscreen)
	{

		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->windowDecoration = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		traits->x = 20; // In screen space, so it's the top-left corner 
		traits->y = 50;
		traits->width = width-20;
		traits->height = height-50;
		traits->samples = 2; //multi samples
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

		m_gameView->getCamera()->setViewport(new osg::Viewport(0, 0, width * WINDOW_RATIO_FULLSCREEN, height));
		m_gameView->getCamera()->setGraphicsContext(gc.get());
		//m_gameView->apply(osgViewer::)

		m_navigationWindow->setGraphicsContext(gc.get());
	}
	else
		m_gameView->apply(new osgViewer::SingleWindow(200, 200, DEFAULT_MAINWINDOW_WIDTH, DEFAULT_MAINWINDOW_HEIGHT));

	// turn of vSync (since we implement
	// an adaptive gameLoop that syncs itself)
	osg::ref_ptr<RealizeOperation> operation = new RealizeOperation;
	m_viewer->setRealizeOperation(operation);
	m_viewer->realize();

    // proxy class to set camera to home in a timer
    class ResetCamExecutor : public util::TaskExecutor
    {
    public:
        Player *m_player;
        ResetCamExecutor(Player* player) :m_player(player){}

        virtual void taskFunction()
        {
            m_player->setCameraToHome();
        }
    };
    ResetCamExecutor *resetCE = new ResetCamExecutor(this);

    //call method after 1s, if set directly the settings will be overwritten at game start
    m_viewAdjustTimer = new util::CountdownTimer();
    m_viewAdjustTimer->addTimer(1000, resetCE);

}

void Player::setCameraToHome()
{
    std::cout << "setting camera to home" << std::endl;
    osg::ref_ptr<const osgGA::GUIEventAdapter> emptyAdapter = new osgGA::GUIEventAdapter();
    m_cameraManipulator->home(*emptyAdapter.get(), *m_gameView.get());
}

void Player::createHUDController(const std::vector<std::shared_ptr<Player>>& players)
{
	if (!hasGameView()) return;

	m_HUDController = std::make_shared<HUDController>(m_id, players);
	m_bikeController->attachTrackingCamera(m_HUDController);
}

void Player::update(int g_gameTime)
{
    m_viewAdjustTimer->update();
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
	m_bendingActivatedUs[window]->dirty();
}

osg::ref_ptr<SampleOSGViewer> Player::navigationViewer()
{
	return m_navigationWindow->navViewer();
}

void  Player::setCameraSpecificUniforms()
{
	m_bendingActivatedUs = uniformVec();
	m_isReflectingUs = uniformVec();

	for (int i : {MAIN_WINDOW, NAVIGATION_WINDOW})
	{
		osg::StateSet* state = m_cameras->at(i)->getOrCreateStateSet();

		osg::Uniform *bending = new osg::Uniform("bendingActivated", false);
		osg::Uniform *reflecting = new osg::Uniform("isReflecting", false);
		
		m_bendingActivatedUs.push_back(bending);
		m_isReflectingUs.push_back(reflecting);

		state->addUniform(bending);
		state->addUniform(reflecting);
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
		m_troenGame->levelController()->removeTemporaries(true,true);

		setHealth(BIKE_DEFAULT_HEALTH);
		
		m_currentRoute++;
		m_routeController->createTrack(m_routes[m_currentRoute]);
		m_troenGame->levelController()->addBoundaries(m_routes.at(m_currentRoute).filePath);
		m_troenGame->levelController()->addSpeedZones(m_routes.at(m_currentRoute).filePath);

		btTransform position = m_routeController->getFirstWayPoint();
		position.setOrigin(position.getOrigin() + btVector3(0, 0, 10));


		m_bikeController->respawnAt(position);
        setCameraToHome();

}
