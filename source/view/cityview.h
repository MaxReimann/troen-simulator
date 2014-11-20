#pragma once
// OSG
#include <osg/ref_ptr>
#include <osg/Geode>
#include "osg/StateAttribute"
// bullet
#include <btBulletDynamicsCommon.h>
// troen
#include "../forwarddeclarations.h"
#include "abstractview.h"
#include "levelview.h"
#include "../model/citymodel.h"

namespace troen
{
	class CityView : public LevelView
	{
	public:
		CityView(std::shared_ptr<AbstractModel> model, std::string levelName);
		void initSpecifics(std::shared_ptr<AbstractModel> model);
	
	protected:
		//osg::ref_ptr<osg::Group> constructFloors(const int levelSize);
		osg::ref_ptr<osg::Group> constructCity(int levelSize, std::string levelName);
	};
}