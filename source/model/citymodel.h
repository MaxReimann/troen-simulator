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
		
	public:
		CityModel(const LevelController* levelController, std::string levelName);
		void reload(std::string levelName);
		btPoint getLevelSize();

		static void callbackWrapper(void* pObject);
	private:
		void initSpecifics();
		void physicsUpdate();
		void setupDebugView();
		QImage m_collisionImage;
		int m_count;
		osg::ref_ptr<osgViewer::View> m_view;
		osg::ref_ptr<SampleOSGViewer> m_debugViewer;
		std::shared_ptr<util::ChronoTimer>	m_debugViewTimer;
		bool m_started;
		int m_nextTime=false;
		std::vector<unsigned char> m_data;
	};

}