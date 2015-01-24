#include "hudview.h"
// osg
#include <osgUtil/Optimizer>
#include <osgText/Text>
#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/MatrixTransform>
#include <osg/RenderInfo>
#include <osgText/Text>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/View>
#include <osgGA/NodeTrackerManipulator>
#include <osg/Quat>
// troen
#include "../player.h"
#include "../constants.h"

using namespace troen;

HUDView::HUDView(const int i, const std::vector<std::shared_ptr<Player>>& players) :
AbstractView(),
m_trackNode(nullptr),
m_speedText(new osgText::Text()),
m_countdownText(new osgText::Text()),
m_playerColor(osg::Vec4(players[i]->color(),1))
{
	m_node->addChild(createHUD(players));
}

void HUDView::toggleVisibility()
{
	//m_radarCamera->setNodeMask(~m_radarCamera->getNodeMask());
}

osg::ref_ptr<osg::Camera> HUDView::createHUD(const std::vector<std::shared_ptr<Player>>& players)
{
	// create a camera to set up the projection & 
	// model view matrices and the subgraph to draw in the HUD
	m_camera = new osg::Camera;
	m_camera->setViewport(
		new osg::Viewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT));
	m_camera->setProjectionMatrix(
		osg::Matrix::ortho2D(0, HUD_PROJECTION_SIZE, 0, HUD_PROJECTION_SIZE));
	m_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_camera->setViewMatrix(osg::Matrix::identity());
	m_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_camera->setAllowEventFocus(false);
	// draw subgraph after main camera view.
	m_camera->setRenderOrder(osg::Camera::POST_RENDER);

	m_savedGeode = new osg::Geode();
	m_camera->addChild(m_savedGeode);

	m_font = osgText::readFontFile("data/fonts/arial.ttf");

	m_fontColor = osg::Vec4(1.0, 1.0, 1.0, 1.0);

	////////////////////////////////////////////////////////////////////////////////
	//
	// Main HUD elements
	//
	////////////////////////////////////////////////////////////////////////////////

	// turn lighting off for the text and disable
	// depth test to ensure it's always on top.
	osg::StateSet* stateset = m_savedGeode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	int offset = HUD_PROJECTION_SIZE / 20;

	initializeText(
		m_speedText,
		osg::Vec3(offset, offset, 0),
		m_fontColor,
		osgText::Text::AlignmentType::LEFT_BOTTOM,
		DEFAULT_WINDOW_HEIGHT / 8);
	setSpeedText(0);
	m_savedGeode->addDrawable(m_speedText);

	initializeText(
		m_countdownText,
		osg::Vec3(HUD_PROJECTION_SIZE / 2, HUD_PROJECTION_SIZE / 2, 0.f),
		m_fontColor,
		osgText::Text::AlignmentType::CENTER_CENTER,
		DEFAULT_WINDOW_HEIGHT / 3);
	setCountdownText(-1);
	m_savedGeode->addDrawable(m_countdownText);

	//initializeText(
	//	m_timeText,
	//	osg::Vec3(HUD_PROJECTION_SIZE - offset, HUD_PROJECTION_SIZE - offset, 0.f),
	//	m_fontColor,
	//	osgText::Text::AlignmentType::RIGHT_TOP,
	//	DEFAULT_WINDOW_HEIGHT / 8);
	//setTimeText(-1, -1);
	//m_savedGeode->addDrawable(m_timeText);


	////////////////////////////////////////////////////////////////////////////////
	//
	// Ingame Message Texts
	//
	////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 4; i++)
	{
		m_ingameMessageTexts[i] = new osgText::Text();
		initializeText(
			m_ingameMessageTexts[i],
			osg::Vec3(HUD_PROJECTION_SIZE / 2.f, HUD_PROJECTION_SIZE - offset * (5 + i*2.f), 0.f),
			osg::Vec4(1, 1, 1, 1),
			osgText::Text::AlignmentType::CENTER_TOP,
			DEFAULT_WINDOW_HEIGHT / 20);
		m_ingameMessageTexts[i]->setText("");
		m_savedGeode->addDrawable(m_ingameMessageTexts[i]);
	}

	return m_camera;
}

void HUDView::initializeText(
	osg::ref_ptr<osgText::Text> text,
	const osg::Vec3& position,
	const osg::Vec4& color,
	const osgText::Text::AlignmentType alignment,
	const int size)
{
	text->setFont(m_font);
	text->setPosition(position);
	text->setColor(color);
	text->setAlignment(alignment);
	text->setCharacterSizeMode(osgText::TextBase::CharacterSizeMode::SCREEN_COORDS);
	text->setCharacterSize(size);
	text->setFontResolution(size, size);
}


void HUDView::resize(const int width,const int height)
{
	osg::ref_ptr<osg::Viewport> hudViewport = new osg::Viewport(0, 0, width, height);
	m_camera->setViewport(hudViewport);
	resizeHudComponents(width, height);

	int normSize = sqrt(width*width + height*height) / 2;
	int offsetX = normSize / 20;
	int offsetY = height / 30;

	int size = normSize;
}

void HUDView::resizeHudComponents(const int width, const int height)
{
	m_speedText->setCharacterSize(height / 15);
	m_speedText->setFontResolution(height / 15, height / 15);

	m_countdownText->setCharacterSize(height / 3);
	m_countdownText->setFontResolution(height / 3, height / 3);

	//m_timeText->setCharacterSize(height / 8);
	//m_timeText->setFontResolution(height / 8, height / 8);
	
	for (size_t i = 0; i < 4; i++)
	{
		m_ingameMessageTexts[i]->setCharacterSize(height / 20);
		m_ingameMessageTexts[i]->setFontResolution(height / 20, height / 20);
	}
}




void HUDView::setTrackNode(osg::Node* trackNode)
{
    m_trackNode = trackNode;
}


void HUDView::setSpeedText(float speed)
{
	if (speed < 0)
		speed = 0;
	std::string speedString = std::to_string((int) speed);
	m_speedText->setText(speedString + " km/h");
}


void HUDView::setCountdownText(const int countdown)
{
	if (countdown == -1)
	{
		m_countdownText->setText("");
	}
	else
	{
		std::string countdownString = std::to_string(abs(countdown));
		m_countdownText->setText(countdownString);
	}
}

void HUDView::setCountdownText(const std::string text)
{
	m_countdownText->setText(text);
}

//
void HUDView::updateIngameMessageTexts(std::deque<std::shared_ptr<IngameMessage>>& messages)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (i < messages.size())
		{
			m_ingameMessageTexts[i]->setText(messages[i]->text);
			m_ingameMessageTexts[i]->setColor(messages[i]->color);
		}
		else
		{
			m_ingameMessageTexts[i]->setText("");
		}
	}
}

