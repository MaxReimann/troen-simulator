#pragma once
// STD
#include <random>
#include <deque>
// osg
#include <osg/Array>
// troen
#include "../forwarddeclarations.h"
#include "abstractcontroller.h"
#include "../gamelogic.h"

namespace troen
{
	struct IngameMessage{
		std::string text;
		osg::Vec4	color;
		long double endTime;
	};

	class HUDController : public AbstractController
	{
	public:
		HUDController(const int index, const std::vector<std::shared_ptr<Player>>& players);
		void attachSceneToRadarCamera(osg::Group* scene);
		void setTrackNode(osg::Node* trackNode);

		//
		// updating
		//
		void resize(const int width, const int height);
		void update(
			const long double currentGameloopTime,
			const long double currentGameTime,
			const int timeLimit,
			const GameLogic::GAMESTATE gameState,
			const std::vector<std::shared_ptr<Player>>& players);

		//
		// ingame messages
		//
		void addNavigationErrorMessage();
		void addAllRoutesFinishedMessage();
		void addCrashedMessage();
		void toggleVisibility();
		void updateRandomNumbers(const long double currentGameloopTime);
	private:
		std::shared_ptr<HUDView> m_HUDView;
		std::deque<std::shared_ptr<IngameMessage>> m_ingameMessages;
		std::default_random_engine m_randomGenerator;

		std::weak_ptr<Player> m_player;
		long double m_nextRandNumStart;
		long double m_lastRandNumStart;
		int m_randNumIndex;
	};
}