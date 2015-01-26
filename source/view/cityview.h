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
		void addSpeedZone(osg::Vec3 position, osg::Quat rotation, int speedLimit);
	
	protected:
		osg::ref_ptr<osg::Group> constructFloors(osg::Vec2 levelSize);
		osg::ref_ptr<osg::Group> constructCity(osg::Vec2 levelSize, std::string levelName);

		std::shared_ptr<CityModel> getCityModel();
		void loadSpeedSigns();

		osg::ref_ptr<osg::Node> m_zone30;
		osg::ref_ptr<osg::Node> m_zone50;
		osg::ref_ptr<osg::Group> m_signsGroup;
	};
}