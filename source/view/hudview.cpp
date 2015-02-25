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
#include "osgDB/ReadFile"

using namespace troen;

HUDView::HUDView(const int i, const std::vector<std::shared_ptr<Player>>& players) :
AbstractView(),
m_trackNode(nullptr),
m_speedText(new osgText::Text()),
m_countdownText(new osgText::Text()),
m_randomNumberText(new osgText::Text()),
m_width(DEFAULT_MAINWINDOW_WIDTH),
m_height(DEFAULT_MAINWINDOW_HEIGHT),
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
		new osg::Viewport(0, 0, DEFAULT_MAINWINDOW_WIDTH, DEFAULT_MAINWINDOW_HEIGHT));
	m_camera->setProjectionMatrix(
		osg::Matrix::ortho2D(0, HUD_PROJECTION_SIZE, 0, HUD_PROJECTION_SIZE));
	m_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_camera->setViewMatrix(osg::Matrix::identity());
	m_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_camera->setAllowEventFocus(false);
	// draw subgraph after main camera view.
	m_camera->setRenderOrder(osg::Camera::POST_RENDER);

	m_mainNode = new osg::Group();
	m_camera->addChild(m_mainNode);

	m_savedGeode = new osg::Geode();
	m_mainNode->addChild(m_savedGeode);

	m_font = osgText::readFontFile("data/fonts/arial.ttf");
	//use to make 
	m_font->setMinFilterHint(osg::Texture::NEAREST);
	m_font->setMagFilterHint(osg::Texture::NEAREST);

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
		osg::Vec3(m_width / 2, offset, 0),
		m_fontColor,
		osgText::Text::AlignmentType::CENTER_BOTTOM,
		DEFAULT_MAINWINDOW_HEIGHT / 4);
	setSpeedText(0);
	m_savedGeode->addDrawable(m_speedText);

	initializeText(
		m_countdownText,
		osg::Vec3(HUD_PROJECTION_SIZE / 2, HUD_PROJECTION_SIZE / 2, 0.f),
		m_fontColor,
		osgText::Text::AlignmentType::CENTER_CENTER,
		DEFAULT_MAINWINDOW_HEIGHT / 3);
	setCountdownText(-1);
	m_savedGeode->addDrawable(m_countdownText);


	//random numbers
	initializeText(
		m_randomNumberText,
		osg::Vec3(offset, offset, 0),
		m_fontColor,
		osgText::Text::AlignmentType::CENTER_CENTER,
		DEFAULT_MAINWINDOW_HEIGHT / 4);

	m_savedGeode->addDrawable(m_randomNumberText);

	m_randomNumberText->setText("test");
	osg::BoundingBox bb;
	bb.expandBy(m_randomNumberText->getBound());
	m_randomNumberText->setText("");

	m_backgroundGeode = new osg::Geode();
	m_backgroundGeode->addDrawable(createRandNumBackground(bb));
	m_backgroundTransform = new osg::MatrixTransform();
	m_backgroundTransform->addChild(m_backgroundGeode);
	m_mainNode->addChild(m_backgroundTransform);



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
			DEFAULT_MAINWINDOW_HEIGHT / 10);
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
	text->setColor(color);
	text->setAlignment(alignment);
	text->setCharacterSizeMode(osgText::TextBase::CharacterSizeMode::SCREEN_COORDS);
	text->setPosition(position);
	setSize(text, size);
}

void HUDView::setSize(
	osg::ref_ptr<osgText::Text> text,
	const int size)
{
	text->setCharacterSize(size);
	text->setFontResolution(size, size);
}


void HUDView::resize(const int width,const int height)
{
	osg::ref_ptr<osg::Viewport> hudViewport = new osg::Viewport(0, 0, width, height);
	m_camera->setViewport(hudViewport);
	resizeHudComponents(width, height);

	m_width = width;
	m_height = height;

	int normSize = sqrt(width*width + height*height) / 2;
	int offsetX = normSize / 20;
	int offsetY = height / 30;

	int size = normSize;
}

void HUDView::resizeHudComponents(const int width, const int height)
{
	int projW = width / 20;
	int projH = height / 20;
	//transformText(m_speedText, osg::Vec3(width / 2, projH / 2, 0), height / 12);
	setSize(m_countdownText, height / 3);
	setSize(m_randomNumberText, height / 5);

	
	for (size_t i = 0; i < 4; i++)
	{
		setSize(m_ingameMessageTexts[i], height / 15);
	}
}


void HUDView::updateRandomNumber(std::string number, osg::Vec2 normalizedPosition)
{
	if (number == "")
	{
		m_randomNumberText->setText("");
		m_backgroundGeode->setNodeMask(0); //invisible
		return;
	}
	

	float offset = m_width / 15;
	float halfextent = m_backgroundGeode->getBound().radius();
	float max_x = m_width - offset - halfextent;
	float max_y = m_height - offset;
	float p_x = clamp(halfextent, max_x, (normalizedPosition.x() * m_width));
	float p_y = clamp(halfextent + offset, max_y, (normalizedPosition.y() * m_height));


	osg::Vec2 position(p_x, p_y);

	m_backgroundGeode->setNodeMask(0xffffffff); //visible

	m_backgroundTransform->setMatrix(osg::Matrix::translate(osg::Vec3(position, 0)));
	m_randomNumberText->setPosition(osg::Vec3(position,0.0));
	m_randomNumberText->setText(number);


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

osg::ref_ptr<osg::Geometry> HUDView::createRandNumBackground(osg::BoundingBox bb)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

	osg::Vec3 sizeUpY(0, bb.yMax() + bb.yMax() / 2, 0.0);
	osg::Vec3 sizeUpYN(0, -bb.yMax() / 2, 0.0);
	bb.expandBy(sizeUpY);
	bb.expandBy(sizeUpYN);


	osg::Vec3 halfExtent((bb.xMax() - bb.xMin()) / 2.0, (bb.yMax() - bb.yMin()) / 2.0f, 0);
	float x_length = bb.xMax() - bb.xMin();
	float y_length = bb.yMax() - bb.yMin();

	vertices->push_back(osg::Vec3(0, y_length, 0) - halfExtent);
	vertices->push_back(osg::Vec3(0, 0, 0) - halfExtent);
	vertices->push_back(osg::Vec3(x_length, 0, 0) - halfExtent);
	vertices->push_back(osg::Vec3(x_length, y_length, 0) - halfExtent);
	geom->setVertexArray(vertices);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	geom->setNormalArray(normals, osg::Array::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f, 0.2f, 1.0f, 0.7f));
	geom->setColorArray(colors, osg::Array::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
	osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//stateset->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	return geom;

}