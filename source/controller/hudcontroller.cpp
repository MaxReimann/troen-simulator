#include "hudcontroller.h"
// std
#include <chrono>
#include <regex>
// troen
#include "../constants.h"
#include "../globals.h"
#include "../gamelogic.h"
#include "../player.h"
#include "../controller/bikecontroller.h"
#include "../view/hudview.h"

namespace
{
	const std::vector<std::string> diedMessages
	{
		"PLAYER_X\njust hit a wall",
		"PLAYER_X\njust crashed!"
	};
	const std::string diedOnNaviBoundary("Sorry, wrong turn! Resetting to route..");
	const std::string diedOnWall("You crashed. Resetting to route..");
	const std::string allRoutesCompleteMessage("Congrats, you completed all routes!");
}

using namespace troen;

HUDController::HUDController(const int id,
	const std::vector<std::shared_ptr<Player>>& players) :
AbstractController(),
m_player(players[id]),
m_nextRandNumStart(8000),
m_lastRandNumStart(0)
{
	m_view = m_HUDView = std::make_shared<HUDView>(id, players);

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	m_randomGenerator.seed(seed);

	m_ingameMessages = std::deque<std::shared_ptr<IngameMessage>>();
}

void HUDController::resize(const int width, const int height)
{
	m_HUDView->resize(width, height);
}

void HUDController::attachSceneToRadarCamera(osg::Group* scene)
{
	//m_HUDView->attachSceneToRadarCamera(scene);
}

void HUDController::update(
	const long double currentGameloopTime,
	const long double currentGameTime,
	const int timeLimit,
	const GameLogic::GAMESTATE gameState,
	const std::vector<std::shared_ptr<Player>>& players)
{
	std::shared_ptr<HUDView> hudview = m_HUDView;
	std::shared_ptr<Player> player = m_player.lock();

	hudview->setSpeedText(player->bikeController()->speed());

	//hudview->setTimeText(currentGameTime, timeLimit);

	//
	// Countdown
	//
	BikeController::BIKESTATE bikeState = player->bikeController()->state();
	if (gameState == GameLogic::GAMESTATE::GAME_OVER)
	{
		hudview->setCountdownText("GameOver");
	}
	else if (bikeState == BikeController::BIKESTATE::RESPAWN || bikeState == BikeController::BIKESTATE::RESPAWN_PART_2)
	{
		double respawnTime = player->bikeController()->respawnTime();
		hudview->setCountdownText((int)(respawnTime - currentGameTime + RESPAWN_DURATION) / 1000 + 1);
	}
	else if (bikeState == BikeController::BIKESTATE::WAITING_FOR_GAMESTART)
	{
		double respawnTime = player->bikeController()->respawnTime();
		hudview->setCountdownText((int)(respawnTime - currentGameloopTime + GAME_START_COUNTDOWN_DURATION) / 1000 + 1);
	}
	else
	{
		hudview->setCountdownText(-1);
	}

	//
	// ingame messages
	//
	while (!m_ingameMessages.empty() && m_ingameMessages.front()->endTime < currentGameTime)
	{
		m_ingameMessages.pop_front();
	}
	hudview->updateIngameMessageTexts(m_ingameMessages);

	updateRandomNumbers(currentGameloopTime);
	
}

void HUDController::setTrackNode(osg::Node* trackNode)
{
    m_HUDView->setTrackNode(trackNode);
}

void HUDController::updateRandomNumbers(const long double currentGameloopTime)
{
	if (currentGameloopTime >= m_nextRandNumStart)
	{
		std::uniform_int_distribution<int> randNumDist1(0, 1000);
		int n = randNumDist1(m_randomGenerator);
		int n2 = randNumDist1(m_randomGenerator);
		m_HUDView->updateRandomNumber(std::to_string(n), osg::Vec2(n/1000.0f, n2/1000.f));

		std::uniform_int_distribution<int> randNumDist2(-500, 500);
		m_lastRandNumStart = currentGameloopTime;
		m_nextRandNumStart = currentGameloopTime + RANDOM_NUMBER_INTERVAL + RANDOM_NUMBER_DURATION + randNumDist2(m_randomGenerator);
	} 
	else if (currentGameloopTime >= m_lastRandNumStart + RANDOM_NUMBER_DURATION)
	{
		m_HUDView->updateRandomNumber("", osg::Vec2(0, 0));
	}



}


void HUDController::addNavigationErrorMessage()
{
	std::shared_ptr<IngameMessage> message = std::make_shared<IngameMessage>();

	message->text = diedOnNaviBoundary;
	message->color = osg::Vec4(1.0, 1.0, 1.0, 1.0);
	message->endTime = g_gameTime + RESPAWN_DURATION;

	m_ingameMessages.push_back(message);
}

void HUDController::addAllRoutesFinishedMessage()
{
	std::shared_ptr<IngameMessage> message = std::make_shared<IngameMessage>();

	std::string text = allRoutesCompleteMessage;

	message->text = text;
	message->color = osg::Vec4(1.0,1.0,1.0,1.0);
	message->endTime = g_gameTime + RESPAWN_DURATION;

	m_ingameMessages.push_back(message);
}

void HUDController::addCrashedMessage()
{
	std::shared_ptr<IngameMessage> message = std::make_shared<IngameMessage>();

	std::string text = diedOnWall;

	message->text = text;
	message->color = osg::Vec4(1.0, 1.0, 1.0, 1.0);
	message->endTime = g_gameTime + RESPAWN_DURATION;

	m_ingameMessages.push_back(message);
}



void HUDController::toggleVisibility()
{
	m_HUDView->toggleVisibility();
}