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

template<class T>
inline bool contains(std::vector<T>  v, T x)
{
	return std::find(v.begin(), v.end(), x) != v.end();
}

//return positive modulo
template<class T>
inline T posMod(T a, T b)
{
	return (a%b + b) % b;
}

CityModel::CityModel(const LevelController* levelController, std::string levelName) :
LevelModel(levelController, levelName)
{
	m_collisionImage = QImage("data/textures/berlin_binary_detailed.png");
	//m_collisionImage.load()
	m_count = 0;
	m_started = false;

	m_checks = std::vector<osg::Vec2>{ osg::Vec2(0.0, 0.0), osg::Vec2(0.0, 0.0),
		osg::Vec2(0.0, 0.0), osg::Vec2(0.0, 0.0)};
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


const btPoint CityModel::getLevelSize()
{
	return btPoint(13002, 11761); //from blender
}

void CityModel::writeDebugImage(int x_pix, int y_pix, std::vector<osg::Vec2> *markPoints, std::vector<osg::Vec2> *markPoints2)
{
	QRgb pix;
	std::vector<osg::Vec2> redPoints = std::vector<osg::Vec2>();
	//redPoints.push_back(osg::Vec2(viewer_width/2, viewer_height/2));
	if (markPoints != nullptr)
	{
		for (osg::Vec2 p : *markPoints)
			redPoints.push_back(p);
	}

	for (long x = 0; x < viewer_width; x++)
	{
		for (long y = 0; y < viewer_height; y++)
		{
			pix = m_collisionImage.pixel(x + x_pix - viewer_width / 2,
				y + y_pix - viewer_height / 2);
			for (osg::Vec2 p : redPoints)
			{
				float adjustedX = p.x() - (x_pix - viewer_width / 2);
				float adjustedY = p.y() - (y_pix - viewer_height / 2);
				if (abs(x - adjustedX) < 0.5 && abs(y - adjustedY) < 0.5)
					pix = QColor::fromRgb(255, 0, 0).rgb();
			}
			if (markPoints2!=nullptr)
			for (osg::Vec2 p : *markPoints2)
			{
				float adjustedX = p.x() - (x_pix - viewer_width / 2);
				float adjustedY = p.y() - (y_pix - viewer_height / 2);
				if (abs(x - adjustedX) < 0.5 && abs(y - adjustedY) < 0.5)
					pix = QColor::fromRgb(0, 255, 0).rgb();
			}

			*(m_image->data(x, y) + 0) = qRed(pix);
			*(m_image->data(x, y) + 1) = qGreen(pix);
			*(m_image->data(x, y) + 2) = qBlue(pix);
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
	osg::Vec2 pos = toVec2(m_levelController->m_troenGame->activeBikeModel()->getPositionBt());
	osg::Vec2 direction = toVec2(m_levelController->m_troenGame->activeBikeModel()->getDirection());
	direction.normalize();
	osg::Vec2 frontPoint = pos + direction * BIKE_DIMENSIONS.y();
	osg::Vec2 backPoint = pos - direction * BIKE_DIMENSIONS.y();
	
	osg::Vec2 perp(-direction.y(), direction.x());

	m_checks[0] = frontPoint - perp*BIKE_DIMENSIONS.x() / 2;
	m_checks[1] = frontPoint + perp*BIKE_DIMENSIONS.x() / 2;
	m_checks[2] = backPoint + perp*BIKE_DIMENSIONS.x() / 2;
	m_checks[3] = backPoint - perp*BIKE_DIMENSIONS.x() / 2;


	bool collision = false;
	std::vector<osg::Vec2> collisionPoint;
	for (osg::Vec2 corner : m_checks)
	{

		osg::Vec2 pixelIndex = worldToPixelIndex(corner);

		QRgb pixel = m_collisionImage.pixel(pixelIndex.x(), pixelIndex.y());
		QColor color(pixel);
		if (color.red() + color.green() + color.blue() < 20.0)
		{
			collision = true;//std::cout << ++m_count << std::endl;
			collisionPoint.push_back(corner);

			if (debugging)
				debugUpdate(pixelIndex.x(), pixelIndex.y());
		}
	}

	if (collision)
	{
		findCollisionEdge(collisionPoint,m_checks);

		const btCollisionObject *bikeColObject = static_cast<const btCollisionObject*>(
			m_levelController->m_troenGame->activeBikeModel()->getRigidBody().get());
		const btCollisionObject *cityColObject = static_cast<const btCollisionObject*>(m_rigidBodies.at(0).get());
		manifold->setBodies(bikeColObject, cityColObject);
		manifold->setContactBreakingThreshold(1.0);
	}




}

osg::Vec2 CityModel::worldToPixelIndex(osg::Vec2 p)
{
	osg::Vec2 levelSize = pairToVec2(getLevelSize());
	osg::Vec2 picSize = osg::Vec2(m_collisionImage.width(), m_collisionImage.height());
	return compMult(compDiv( (p + levelSize / 2.0), (levelSize) ), picSize);
}

// static wrapper-function to be able to callback the member function
void CityModel::callbackWrapper(void* pObject, btPersistentManifold *manifold)
{
	CityModel* mySelf = (CityModel*)pObject;

	// call member
	mySelf->physicsUpdate(manifold);
}

void CityModel::findCollisionEdge(std::vector<osg::Vec2> &points, std::vector<osg::Vec2> &checks)
{
	osg::Vec2 direction1, direction2;

	if (points.size() == 1)
	{
		int i=0;
		for (; i < 4; i++)
		{
			if (points.front() == checks[i])
				break;
		}

		direction1 = checks[(i + 1) % 4] - checks[i];
		direction2 = checks[posMod(i - 1,4)] - checks[i];

		// insert same again, to apply 2nd direction later
		points.push_back(points.front());
		
	}
	else if (points.size() == 2)
		for (int i = 0; i < 4; i++)
			if (contains(points, checks[i]) && contains(points, checks[(i + 1) % 4]))
			{
				direction1 = checks[posMod(i - 1, 4)] - checks[i];
				direction2 = checks[(i + 2) % 4] - checks[(i + 1) % 4];
				break;
			}

	else if (points.size() == 3)
		for (int i = 0; i < 4; i++)
			if (!contains(points, checks[i]))
			{
				direction1 = checks[posMod(i - 1, 4)] - checks[i];
				direction2 = checks[(i + 2) % 4] - checks[(i + 1) % 4];
				break;
			}

	direction1.normalize();
	direction2.normalize();
	osg::Vec2 pix1 = worldToPixelIndex(points[0]);
	osg::Vec2 pix2 = worldToPixelIndex(points[1]);

	osg::Vec2 p1 = findBorder(pix1, direction1);
	osg::Vec2 p2 = findBorder(pix2, direction2);
	std::vector<osg::Vec2> markPoints{ p1, p2 };

	std::vector<osg::Vec2> markPoints2{ worldToPixelIndex(checks[0]),
		worldToPixelIndex(checks[1]),
		worldToPixelIndex(checks[2]),
		worldToPixelIndex(checks[3])
	};


	if (debugging)
	{
		writeDebugImage(pix1.x(), pix1.y(), &markPoints,&markPoints2);
		while (1);
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

osg::Vec2 CityModel::findBorder(osg::Vec2 startI, osg::Vec2 normalizedDirection)
{
	osg::Vec2 pixelIndex = startI;
	int count = 0;

	//search for border in direction, max 100 iterations
	while (count++ < 100) 
	{
		QRgb pixel = m_collisionImage.pixel(pixelIndex.x(), pixelIndex.y());
		if (qRed(pixel) + qGreen(pixel) + qBlue(pixel) >= 200.0*3)
			return pixelIndex;
		
		pixelIndex += normalizedDirection;
	}

	return pixelIndex;
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