#pragma once

//troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"
#include <btBulletDynamicsCommon.h>

#include "levelmodel.h"
#include "qimage.h"



namespace troen
{

	class CityModel : public LevelModel
	{
		friend LevelController;
	public:
		CityModel(const LevelController* levelController, std::string levelName);
		void reload(std::string levelName);
		btPoint getLevelSize();

		static void callbackWrapper(void* pObject, btPersistentManifold *resultManifold);
	protected:
		void initSpecifics();
		void physicsUpdate(btPersistentManifold *manifold);
		void setupDebugView();
		void writeDebugImage(int x_pix, int y_pix);
		void debugUpdate(int x_pix, int y_pix);
		void findCollisionEdge(std::vector<osg::Vec2> &points, osg::Vec2 checks[4]);
		QImage m_collisionImage;
		int m_count;
		osg::ref_ptr<osgViewer::View> m_view;
		osg::ref_ptr<SampleOSGViewer> m_debugViewer;
		std::shared_ptr<util::ChronoTimer>	m_debugViewTimer;
		bool m_started;
		int m_nextTime = false;
		bool m_key_event = false;
		osg::ref_ptr<osg::Image> m_image;
	};

}