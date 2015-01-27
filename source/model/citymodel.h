#pragma once

//troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"
#include "../controller/levelcontroller.h"
#include <btBulletDynamicsCommon.h>

#include "levelmodel.h"
#include "qimage.h"


namespace troen
{
	struct rigidBodyWrap
	{
		btBoxShape shape;
		btDefaultMotionState motionState;
		btTransform worldTransform;
		std::shared_ptr<btRigidBody> body;

		rigidBodyWrap() : shape(btBoxShape(btVector3(0,0,1))),
			motionState(btDefaultMotionState()),
			worldTransform(btTransform())
		{}
	};

	class CityModel : public LevelModel
	{
		friend LevelController;
	public:
		CityModel(LevelController* levelController, std::string levelName);
		void reload(std::string levelName);
		const inline btPoint getLevelSize();

		static void callbackWrapper(void* pObject);
		void attachWorld(std::shared_ptr<PhysicsWorld> &world);

		std::vector<std::shared_ptr<btGhostObject>> getGhostObjects() 
		{ 
			return m_ghostObjectList; 
		}

	protected:
		void initSpecifics();
		void physicsUpdate();
		void setupDebugView();
		void writeDebugImage(int x_pix, int y_pix, std::vector<osg::Vec2> *markPoints=nullptr, std::vector<osg::Vec2> *markPoints2=nullptr);
		void debugUpdate(int x_pix, int y_pix);
		osg::Vec2 findCollisionEdge(std::vector<osg::Vec2> &points, std::vector<osg::Vec2> &checks, osg::Vec2 &resultVector);
		inline osg::Vec2 worldToPixelIndex(osg::Vec2 p);
		inline osg::Vec2 pixelToWorld(osg::Vec2 pixel);
		osg::Vec2 findBorder(osg::Vec2 startI, osg::Vec2 direction);
		void clearTemporaryWalls();
		void addSpeedZone(btTransform position, int speedLimit);
		Speedzone findSpeedZone(btGhostObject *collided);
		void removeGhosts();
		void addBoxesAsGhosts(std::vector<BoxModel> &boxes, COLLISIONTYPE type);
		QImage m_collisionImage;
		int m_count;
		osg::ref_ptr<osgViewer::View> m_view;
		osg::ref_ptr<SampleOSGViewer> m_debugViewer;
		std::shared_ptr<util::ChronoTimer>	m_debugViewTimer;
		bool m_started;
		int m_nextTime = false;
		bool m_key_event = false;
		int m_lastCollidingTimer = 0;

		osg::ref_ptr<osg::Image> m_image;
		std::vector<osg::Vec2> m_checks;
		std::vector<std::shared_ptr<rigidBodyWrap>> m_lastBodies;
		std::vector<std::shared_ptr<btGhostObject>>		m_ghostObjectList;
		std::vector<Speedzone>							m_speedZoneList;
	};

}
