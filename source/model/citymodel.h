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
		const inline btPoint getLevelSize();

		static void callbackWrapper(void* pObject, btPersistentManifold *resultManifold);
	protected:
		void initSpecifics();
		void physicsUpdate(btPersistentManifold *manifold);
		void setupDebugView();
		void writeDebugImage(int x_pix, int y_pix, std::vector<osg::Vec2> *markPoints=nullptr, std::vector<osg::Vec2> *markPoints2=nullptr);
		void debugUpdate(int x_pix, int y_pix);
		void findCollisionEdge(std::vector<osg::Vec2> &points, std::vector<osg::Vec2> &checks);
		inline osg::Vec2 worldToPixelIndex(osg::Vec2 p);
		osg::Vec2 findBorder(osg::Vec2 startI, osg::Vec2 direction);
		QImage m_collisionImage;
		int m_count;
		osg::ref_ptr<osgViewer::View> m_view;
		osg::ref_ptr<SampleOSGViewer> m_debugViewer;
		std::shared_ptr<util::ChronoTimer>	m_debugViewTimer;
		bool m_started;
		int m_nextTime = false;
		bool m_key_event = false;
		osg::ref_ptr<osg::Image> m_image;
		std::vector<osg::Vec2> m_checks;
	};

}