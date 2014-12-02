#include "citymodel.h"
#include "osgViewer/Viewer"
#include "osgViewer/config/SingleWindow"
//bullet
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btHashMap.h"
//troen
#include "../controller/itemcontroller.h"
#include "objectinfo.h"
#include "../constants.h"
#include "../controller/abstractcontroller.h"
#include "../controller/levelcontroller.h"
#include "../troengame.h"
#include "../model/bikemodel.h"

#include <string>
#include <iostream>
#include <fstream>
#include "qimage.h"
#include "../sampleosgviewer.h"
#include "../util/chronotimer.h"
#include "osgDB/ReadFile"

static bool debugging = true;


using namespace troen;

CityModel::CityModel(const LevelController* levelController, std::string levelName) :
LevelModel(levelController, levelName)
{
	m_collisionImage = QImage("data/textures/berlin_binary_detailed.png");
	//m_collisionImage.load()
	m_count = 0;

	if (debugging)
		setupDebugView();

}

void CityModel::initSpecifics()
{
	addFloor(-10);
}


void CityModel::setupDebugView()
{
	const int viewer_width = 512;
	const int viewer_height = 512;

	m_debugViewTimer = std::make_shared<util::ChronoTimer>(true, true);
	m_view = new osgViewer::View();

	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Texture2D> testTexture = new osg::Texture2D;

	m_data = std::vector<unsigned char>(viewer_width*viewer_height * 3);
	unsigned char *data = new unsigned char[viewer_width*viewer_height * 3];

	QRgb pix;
	for (long y = 0; y < viewer_height; y++)
	{
		for (long x = 0; x < viewer_width; x++)
		{
			pix = m_collisionImage.pixel(x, y);
			data[x*3 + y*viewer_width] = 255;// qRed(pix);
			data[x*3 + y*viewer_width + 1] = 100;//qGreen(pix);
			data[x*3 + y*viewer_width + 2] = 255;// qBlue(pix);
		}
	}

	osg::ref_ptr<osg::Image> image = new osg::Image;
	//image->allocateImage(width, height, length, GL_RGB, GL_UNSIGNED_BYTE);
	image->setOrigin(osg::Image::BOTTOM_LEFT);//start countingpixels on the Bottom left of the picture 
	image->setImage(viewer_width, viewer_height, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE);


	assert(image.valid());

	testTexture->setImage(image);

	osg::Camera* camera = m_view->getCamera();

	// set the projection matrix
	//camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));

	// set the view matrix
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());

	// only clear the depth buffer
	//camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	//// draw subgraph after main camera view.
	//camera->setRenderOrder(osg::Camera::POST_RENDER);

	osg::Geode* geode(new osg::Geode());
	//geode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(-1, -1, 0), osg::Vec3(2, 0, 0), osg::Vec3(0, 2, 0)));
	osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(osg::Vec3(0.0f, 0.0f, 0.0f),
		osg::Vec3(viewer_width, 0.0f, 0.0f),
		osg::Vec3(0.0f, 0.0f, viewer_height),
		0.0f,
		0.0f,
		1.0f,
		1.0f);
	pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0, testTexture.get());
	geode->addDrawable(pictureQuad);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	root->addChild(geode);
	m_view->setSceneData(root);

#ifdef WIN32
	m_view->apply(new osgViewer::SingleWindow(800, 800, viewer_width, viewer_height));
#else
	m_view->setUpViewInWindow(100, 100, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
#endif

	m_debugViewer = new SampleOSGViewer();
	m_debugViewer->addView(m_view);
	osg::ref_ptr<RealizeOperation> renderOp = new RealizeOperation;
	m_debugViewer->setRealizeOperation(renderOp);

	//m_debugViewer->realize();
	m_debugViewer->run();
}

void CityModel::reload(std::string levelName)
{
	m_rigidBodies.clear();
	m_motionStates.clear();
	m_collisionShapes.clear();
	m_floors.clear();

	addFloor(-10);
}


btPoint CityModel::getLevelSize()
{
	return btPoint(13002, 11761); //from blender
}

void CityModel::physicsUpdate()
{
	int w = m_collisionImage.width();
	int h = m_collisionImage.height();

	double lv_w = getLevelSize().first;
	double lv_h = getLevelSize().second;

	double x_offset = 0;// -50.0;
	double y_offset = 0;// -10.0;

	btVector3 pos = m_levelController->m_troenGame->activeBikeModel()->getPositionBt();

	btCollisionShape* shape = m_levelController->m_troenGame->activeBikeModel()->getRigidBody()->getCollisionShape();
	btVector3 halfExtents = ((btBoxShape*)shape)->getHalfExtentsWithMargin();
	int count = 0;
	double xChecks[]{ pos.x() };// -halfExtents.x(), pos.x(), pos.x() + halfExtents.x()};

	for (double x : xChecks)
	{

		double xrel = (x + x_offset + lv_w / 2.0) / lv_w;
		double yrel = (pos.y() + y_offset + lv_h / 2.0) / lv_h;

		int x_pix = xrel * (double)w;
		int y_pix = yrel * (double)h;

		QRgb pixel = m_collisionImage.pixel(x_pix, y_pix);
		QColor color(pixel);
		//if (color.red() + color.green() + color.blue() < 20.0)
		//	std::cout << ++m_count << std::endl;

		if (debugging)
		{
			if (!m_started)
			{
				m_debugViewTimer->start();
				m_started = true;
				m_nextTime = m_debugViewTimer->elapsed();
			}

			//if (m_debugViewTimer->elapsed() >= m_nextTime)
			//{
			//	// assign the time for the next update
			//	m_nextTime += 200;
			//	m_debugViewer->frame();
			//}
		}


	}
}

// static wrapper-function to be able to callback the member function
void CityModel::callbackWrapper(void* pObject)
{
	CityModel* mySelf = (CityModel*)pObject;

	// call member
	mySelf->physicsUpdate();
}
