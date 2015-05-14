#include "gameeventhandler.h"

#include <osg/ValueObject>
// troen
#include "troengame.h"
#include "gamelogic.h"
#include "view/shaders.h"
#include "BendedViews/src/SplineDeformationRendering.h"
#include "controller/levelcontroller.h"
#include "player.h"

using namespace troen;

GameEventHandler::GameEventHandler(TroenGame * game) :
osgGA::GUIEventHandler(),
m_troenGame(game)
{}

void GameEventHandler::attachGameLogic(std::shared_ptr<GameLogic>& gamelogic)
{
	m_gameLogic = gamelogic;
}

bool GameEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::RESIZE)
	{
		// re setup textures to new size
		int window_type;
		aa.asView()->getUserValue("window_type", window_type);

		m_troenGame->resize(ea.getWindowWidth(), ea.getWindowHeight(), window_type);
		return false;
	}


	if (ea.getEventType() != osgGA::GUIEventAdapter::KEYDOWN) {
		return false;
	}

	switch (ea.getKey()) {
	case osgGA::GUIEventAdapter::KEY_V:
		m_troenGame->switchSoundVolumeEvent();
		break;
	case osgGA::GUIEventAdapter::KEY_C:
		m_gameLogic.lock()->removeAllFences();
		break;
	case osgGA::GUIEventAdapter::KEY_F:

        //osg::ref_ptr<const osgGA::GUIEventAdapter> emptyAdapter = new osgGA::GUIEventAdapter();
        m_troenGame->players()[0]->cameraManipulator()->home(ea, aa);
		break;
	case osgGA::GUIEventAdapter::KEY_L:
		m_troenGame->reloadLevel();
		break;
	case osgGA::GUIEventAdapter::KEY_R:
		std::cout << "Reloading shaders" << std::endl;
		shaders::reloadShaders();
		if (m_troenGame->useBendedViews())
			m_troenGame->getBendedViews()->reloadShaders();
		break;
	case osgGA::GUIEventAdapter::KEY_Shift_R:
	case osgGA::GUIEventAdapter::KEY_Shift_L:
		m_troenGame->pauseEvent();
		break;
	case osgGA::GUIEventAdapter::KEY_Delete:
		m_gameLogic.lock()->restartLevel();
		break;
	case osgGA::GUIEventAdapter::KEY_U:
		m_troenGame->players()[0]->bikeController()->reloadVehicle();
		break;
	case osgGA::GUIEventAdapter::KEY_1:
		if (m_troenGame->useBendedViews())
		{
			m_troenGame->getBendedViews()->setPreset(0);
			m_troenGame->players()[0]->setBendingUniform(NAVIGATION_WINDOW, false);

		}
		break;
	case osgGA::GUIEventAdapter::KEY_2:
		if (m_troenGame->useBendedViews())
		{
			m_troenGame->getBendedViews()->setPreset(1);
			m_troenGame->players()[0]->setBendingUniform(NAVIGATION_WINDOW, true);

		}
		break;
	case osgGA::GUIEventAdapter::KEY_3:
		if (m_troenGame->useBendedViews())
		{
			m_troenGame->getBendedViews()->setPreset(2);
			m_troenGame->players()[0]->setBendingUniform(NAVIGATION_WINDOW, true);
		}
		break;
	case osgGA::GUIEventAdapter::KEY_4:
		if (m_troenGame->useBendedViews())
		{
			m_troenGame->getBendedViews()->setPreset(3);
			m_troenGame->players()[0]->setBendingUniform(NAVIGATION_WINDOW, true);
		}

		break;
	case osgGA::GUIEventAdapter::KEY_Tab:
		m_troenGame->toggleHUDVisibility();
		break;
	case osgGA::GUIEventAdapter::KEY_Q:
		m_troenGame->levelController()->debugSnapShot();
		break;
	case osgGA::GUIEventAdapter::KEY_M:
		m_troenGame->switchSoundVolumeEvent();
		return true;
	default:
		return false;
	}

	return true;
}