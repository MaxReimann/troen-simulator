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
#include "../model/levelmodel.h"
#include "../controller/itemcontroller.h"

namespace troen
{
	class LevelView : public AbstractView
	{
	public:
		LevelView(std::shared_ptr<AbstractModel> model, std::string levelName);
		virtual ~LevelView(){};
		virtual void reload(std::string levelName);

		virtual osg::ref_ptr<osg::Group> getFloor();
		void addItemBox(osg::ref_ptr<osg::MatrixTransform>& matrixTransform);
		void removeItemBox(osg::ref_ptr<osg::MatrixTransform>& matrixTransform);

		void setBendingFactor(float bendingFactor);
		void setBendingActive(bool val);
	protected:
		virtual osg::ref_ptr<osg::Group> constructFloors(const int levelSize, std::string texPath="data/textures/floor.tga");
		virtual osg::ref_ptr<osg::Group> constructObstacles(const int levelSize, std::string levelName);
		virtual void initSpecifics(std::shared_ptr<AbstractModel> model);

		virtual osg::ref_ptr<osg::Group> constructGroupForBoxes(std::vector<BoxModel> &boxes);
		virtual osg::ref_ptr<osg::Group> constructRadarElementsForBoxes(std::vector<BoxModel> &boxes);

		virtual void setTexture(osg::ref_ptr<osg::StateSet> stateset, std::string filePath, int unit, bool override = false);
		virtual void addShaderAndUniforms(osg::ref_ptr<osg::Node> node, int shaderIndex, int levelSize, int modelID, float alpha, float trueColor = 0.0);

		std::shared_ptr<LevelModel> m_model;
		osg::ref_ptr<osg::Group> m_floors;

		osg::Uniform *m_bendedUniform;
		osg::Uniform *m_bendingActiveUniform;
		osg::Group *m_itemGroup;
		std::string m_levelName;
	};
}