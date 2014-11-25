#pragma once
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "qimage.h"


//troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"
#include <btBulletDynamicsCommon.h>

#include "levelmodel.h"


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
		QImage m_collisionImage;
		int m_count;
	};

}