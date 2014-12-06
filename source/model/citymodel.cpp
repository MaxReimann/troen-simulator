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
#include "osgDB/WriteFile"
#include "qfile.h"
#include "qtextstream.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"

static bool debugging = false;
const int viewer_width = 1024;
const int viewer_height = 1024;

using namespace troen;

CityModel::CityModel(const LevelController* levelController, std::string levelName) :
LevelModel(levelController, levelName)
{
	m_collisionImage = QImage("data/textures/berlin_binary_detailed.png");
	//m_collisionImage.load()
	m_count = 0;
	m_started = false;

	if (debugging)
		setupDebugView();
}

void CityModel::initSpecifics()
{
	addFloor(-10);
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

void CityModel::writeDebugImage(int x_pix, int y_pix)
{
	QRgb pix;
	for (long x = 0; x < viewer_width; x++)
	{
		for (long y = 0; y < viewer_height; y++)
		{
			pix = m_collisionImage.pixel(x + x_pix - viewer_width / 2,
				y + y_pix - viewer_height / 2);
			if (abs(x - viewer_width / 2) < 2 && abs(y - viewer_height / 2) < 2)
				pix = QColor::fromRgb(255, 0, 0).rgb();

			*(m_image->data(x, y) + 0) = qRed(pix);
			*(m_image->data(x, y) + 1) = qBlue(pix);
			*(m_image->data(x, y) + 2) = qGreen(pix);
		}
	}

	m_image->dirty();
	if (!osgDB::writeImageFile(*(m_image.get()), "data/test/result.tga"))
		std::cout << "fail" << std::endl;

	btVector3 pos = m_levelController->m_troenGame->activeBikeModel()->getPositionBt();

	QFile file("data/test/currentpos.txt");
	if (file.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream stream(&file);
		stream << " xpix: " << x_pix << " ypix:" << y_pix << " xpos: " << pos.x() << " posy: " << pos.y();
		stream.flush();
		file.close();
	}


}

void CityModel::debugUpdate(int x_pix, int y_pix)
{
	if (!m_started)
	{
		m_debugViewTimer = std::make_shared<util::ChronoTimer>(true, true);
		m_debugViewTimer->start();
		m_started = true;
		m_nextTime = m_debugViewTimer->elapsed();
	}

	if (m_debugViewTimer->elapsed() >= m_nextTime)
	{
		// assign the time for the next update
		m_nextTime += 200;
		///m_debugViewer->frame();
	}

	if (m_key_event)
	{
		writeDebugImage(x_pix, y_pix);
		m_key_event = false;
	}
}

void CityModel::physicsUpdate(btPersistentManifold *manifold)
{
	int w = m_collisionImage.width();
	int h = m_collisionImage.height();

	double lv_w = getLevelSize().first;
	double lv_h = getLevelSize().second;

	const btCollisionObject *bikeColObject = static_cast<const btCollisionObject*>(
		m_levelController->m_troenGame->activeBikeModel()->getRigidBody().get());
	const btCollisionObject *cityColObject = static_cast<const btCollisionObject*>(m_rigidBodies.at(0).get());
	manifold->setBodies(bikeColObject, cityColObject);
	manifold->setContactBreakingThreshold(1.0);

	btVector3 pos = m_levelController->m_troenGame->activeBikeModel()->getPositionBt();
	btCollisionShape* shape = m_levelController->m_troenGame->activeBikeModel()->getRigidBody()->getCollisionShape();
	btVector3 halfExtents = ((btBoxShape*)shape)->getHalfExtentsWithMargin();

	btVector3 minV, maxV;
	m_levelController->m_troenGame->activeBikeModel()->getRigidBody()->getAabb(minV, maxV);

	//osg::Vec2 checks[4]{osg::Vec2(pos.x() - BIKE_DIMENSIONS.x() / 2, pos.y() + BIKE_DIMENSIONS.y() / 2),
	//	osg::Vec2(pos.x() + BIKE_DIMENSIONS.x() / 2, pos.y() + BIKE_DIMENSIONS.y() / 2),
	//	osg::Vec2(pos.x() - BIKE_DIMENSIONS.x() / 2, pos.y() - BIKE_DIMENSIONS.y() / 2),
	//	osg::Vec2(pos.x() + BIKE_DIMENSIONS.x() / 2, pos.y() - BIKE_DIMENSIONS.y() / 2)};

	osg::Vec2 checks[4]{osg::Vec2(minV.x(), maxV.y()), toVec2(maxV), 
		toVec2(minV), osg::Vec2(maxV.x(), minV.y())};


	bool collision = false;
	std::vector<osg::Vec2> collisionPoint;
	for (osg::Vec2 corner : checks)
	{

		double xrel = (corner.x() + lv_w / 2.0) / lv_w;
		double yrel = (corner.y() + lv_h / 2.0) / lv_h;

		int x_pix = xrel * (double)w;
		int y_pix = yrel * (double)h;

		QRgb pixel = m_collisionImage.pixel(x_pix, y_pix);
		QColor color(pixel);
		if (color.red() + color.green() + color.blue() < 20.0)
		{
			collision = true;//std::cout << ++m_count << std::endl;
			collisionPoint.push_back(corner);

			if (debugging)
				debugUpdate(x_pix, y_pix);
		}
	}


	findCollisionEdge(collisionPoint,checks);




}

// static wrapper-function to be able to callback the member function
void CityModel::callbackWrapper(void* pObject, btPersistentManifold *manifold)
{
	CityModel* mySelf = (CityModel*)pObject;

	// call member
	mySelf->physicsUpdate(manifold);
}

void CityModel::findCollisionEdge(std::vector<osg::Vec2> &points, osg::Vec2 checks[4])
{
	osg::Vec2 direction1, direction2;

	if (points.size() == 1)
	{
		if (points.front() == checks[0])
		{
			direction1 = (checks[0]-checks[1]);
			direction2 = (checks[2] - checks[0]);
		}
		else if (points.front() == checks[1])
		{
			direction1 = (checks[1] - checks[0]);
			direction2 = (checks[3] - checks[0]);
		}
		
	}
	else if (points.size() == 2)
	{
		//int moves[][2]{{ 0, -1 }, { -1, 0 }, { 0, 1 }, { 1, 0 }};
		for (int i = 0; i < 4; i++)
		{
			if (points.front() == checks[i] && points.back() == checks[(i + 1) % 4] ||
				points.back() == checks[i] && points.front() == checks[(i + 1) % 4])
			{
				direction1 = checks[(i + 2) % 4] - checks[i];
				direction2 = checks[(i + 3) % 4] - checks[(i + 1) % 4];
			}
		}

	}


	//btTransform bikeTrans;
	//bikeTrans.setOrigin(btVector3(corner[0], corner[1], 10.0));

	//btCollisionObjectWrapper b1((const btCollisionObjectWrapper*)NULL,
	//	(const btCollisionShape*)NULL, bikeColObject, (const btTransform)bikeTrans, 0, 0);
	//btCollisionObjectWrapper b2((const btCollisionObjectWrapper*)NULL,
	//	(const btCollisionShape*)NULL, cityColObject, (const btTransform)btTransform(), 0, 1);

	//btManifoldResult colResult(&b1, &b2);
	//colResult.setPersistentManifold(manifold);
	//// adds contanct point to manifold
	//colResult.addContactPoint(btVector3(0, -1, 0), btVector3(corner[0], corner[1], 10.0), 0.1);

}


void CityModel::setupDebugView()
{

	m_image = new osg::Image;
	m_image->allocateImage(viewer_width, viewer_width, 1, GL_RGB, GL_UNSIGNED_BYTE);

	QRgb pix;
	for (long x = 0; x < viewer_width; x++)
	{
		for (long y = 0; y < viewer_height; y++)
		{
			pix = m_collisionImage.pixel(x, y);
			*(m_image->data(x, y) + 0) = qBlue(pix);
			*(m_image->data(x, y) + 1) = qRed(pix);
			*(m_image->data(x, y) + 2) = qGreen(pix);
		}
	}
	m_image->dirty();

	m_view = new osgViewer::View();

	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Texture2D> testTexture = new osg::Texture2D;
	assert(m_image.valid());

	testTexture->setImage(m_image);

	osg::Camera* camera = m_view->getCamera();
	// set the projection matrix
	//camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
	camera->setViewMatrix(osg::Matrix::identity());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(osg::Vec3(0.0f, 0.0f, 0.0f),
		osg::Vec3(viewer_width, 0.0f, 0.0f),
		osg::Vec3(0.0f, 0.0f, viewer_height),
		0.0f, 0.0f, 1.0f, 1.0f);
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

	m_debugViewer->realize();
}