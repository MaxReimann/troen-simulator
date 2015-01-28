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


int randomNumbers[] = {
	544, 569, 408, 968, 565, 520, 291, 120, 960, 419, 106, 746, 336, 877, 297, 910, 458, 326, 245, 643, 200, 238, 716, 773, 174, 370,
	852, 311, 690, 184, 224, 776, 702, 166, 385, 684, 838, 887, 397, 599, 248, 743, 296, 968, 798, 438, 987, 824, 216, 970, 439, 730,
	173, 893, 817, 574, 771, 640, 210, 450, 173, 264, 988, 638, 696, 183, 726, 232, 673, 210, 503, 172, 321, 575, 702, 532, 381, 125,
	264, 256, 741, 133, 319, 471, 398, 262, 470, 600, 765, 676, 912, 327, 157, 873, 572, 610, 500, 136, 733, 387, 263, 870, 680, 532,
	813, 241, 123, 515, 126, 364, 349, 690, 497, 806, 974, 183, 754, 402, 837, 956, 378, 172, 724, 702, 616, 394, 671, 702, 467, 101,
	808, 951, 235, 986, 360, 349, 306, 684, 433, 991, 579, 664, 751, 595, 352, 722, 989, 263, 910, 326, 110, 358, 466, 243, 844, 290,
	775, 997, 880, 266, 781, 594, 479, 260, 262, 696, 967, 443, 105, 985, 354, 383, 966, 659, 537, 755
};

using namespace troen;

HUDController::HUDController(const int id,
	const std::vector<std::shared_ptr<Player>>& players) :
AbstractController(),
m_player(players[id]),
m_nextRandNumStart(8000),
m_lastRandNumStart(0),
m_randNumIndex(0)
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

	updateRandomNumbers(currentGameTime);
	
}

void HUDController::setTrackNode(osg::Node* trackNode)
{
    m_HUDView->setTrackNode(trackNode);
}

void HUDController::updateRandomNumbers(const long double currentGameTime)
{
	if (currentGameTime >= m_nextRandNumStart)
	{

		int displayNum = randomNumbers[m_randNumIndex++];;

		std::uniform_int_distribution<int> randNumDist1(0, 1000);
		int x = randNumDist1(m_randomGenerator);
		int y = randNumDist1(m_randomGenerator);
		m_HUDView->updateRandomNumber(std::to_string(displayNum), osg::Vec2(x/1000.0f, y/1000.f));

		std::uniform_int_distribution<int> randNumDist2(-500, 500);
		m_lastRandNumStart = currentGameTime;
		m_nextRandNumStart = currentGameTime + RANDOM_NUMBER_INTERVAL + RANDOM_NUMBER_DURATION + randNumDist2(m_randomGenerator);
	} 
	else if (currentGameTime >= m_lastRandNumStart + RANDOM_NUMBER_DURATION)
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