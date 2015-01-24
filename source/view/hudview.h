#pragma once
// STD
#include <deque>
#include <random>
// OSG
#include <osgViewer/View>
#include <osgText/Text>
// troen
#include "../forwarddeclarations.h"
#include "abstractview.h"
#include "../controller/hudcontroller.h"

namespace troen
{
	// helper class to retrieve the position of a node in world coordinates
	// could potentially be in it's own header file
	class getWorldCoordOfNodeVisitor : public osg::NodeVisitor
	{
	public:
		getWorldCoordOfNodeVisitor() :
			osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
		{
				wcMatrix = new osg::Matrixd();
			}
		virtual void apply(osg::Node &node)
		{
			if (!done)
			{
				if (0 == node.getNumParents()) // no parents
				{
					wcMatrix->set(osg::computeLocalToWorld(this->getNodePath()));
					done = true;
				}
				traverse(node);
			}
		}
		osg::Matrixd* worldCoordinatesMatrix()
		{
			return wcMatrix;
		}
	private:
		bool done;
		osg::Matrix* wcMatrix;
	};



	class HUDView : public AbstractView
	{
	public:
		HUDView(const int i, const std::vector<std::shared_ptr<Player>>& players);

		//
		// initialization
		//
		//void attachSceneToRadarCamera(osg::Group* scene);
		void setTrackNode(osg::Node* trackNode);
		
		//
		// updating
		//
		void resize(const int width, const int height);
		void updateIngameMessageTexts(std::deque<std::shared_ptr<IngameMessage>>& messages);

		//
		// texts
		//
		void initializeText(
			osg::ref_ptr<osgText::Text> text,
			const osg::Vec3& position,
			const osg::Vec4& color,
			const osgText::Text::AlignmentType alignment,
			const int size);
		void setSpeedText(const float speed);
		void setCountdownText(const int countdown);
		void setCountdownText(const std::string text);
		void setTimeText(const double gameTime, const int timeLimit);
		void toggleVisibility();
		//void toggleRadar();
	private:
		osg::ref_ptr<osg::Camera> createHUD(const std::vector<std::shared_ptr<Player>>& players);
		//osg::ref_ptr<osg::Camera> createRadar(const int index);

		void resizeHudComponents(const int width, const int height);
		osg::ref_ptr<osg::Camera>	m_camera;
		//osg::ref_ptr<osg::Camera>	m_radarCamera;
		osg::ref_ptr<osg::Geode>	m_savedGeode;
		osg::ref_ptr<osg::Node>		m_trackNode;
		osg::ref_ptr<osg::Group> m_hudGroup;
		osg::ref_ptr<osgGA::NodeTrackerManipulator> m_radarManipulator;

		osg::ref_ptr<osgText::Font> m_font;
		osg::ref_ptr<osgText::Text> m_speedText;
		osg::ref_ptr<osgText::Text> m_countdownText;
		//osg::ref_ptr<osgText::Text> m_timeText;
		osg::ref_ptr<osgText::Text> m_ingameMessageTexts[4];

		osg::Vec4 m_playerColor;
		osg::Vec4 m_fontColor;
	};
}