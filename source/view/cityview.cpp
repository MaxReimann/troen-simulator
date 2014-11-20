#include "cityview.h"
// STD
#include <math.h>
// OSG
#include <osg/ImageStream>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/TexGenNode>
#include "osg/Node"

#include <osg/PolygonMode>
// bullet
#include <btBulletDynamicsCommon.h>
// troen
#include "shaders.h"
#include "../constants.h"

using namespace troen;


CityView::CityView(std::shared_ptr<AbstractModel> model, std::string levelName) :
LevelView(model, levelName)
{
	m_node->removeChildren(0, m_node->getNumChildren()); //undo effects of initspecific in base class
	initSpecifics(model);
}

//specific to cityview, called by LevelView()
void CityView::initSpecifics(std::shared_ptr<AbstractModel> model)
{
	m_model = std::dynamic_pointer_cast<CityModel>(model);
	int levelSize = m_model->getLevelSize();

	m_node->addChild(constructFloors(levelSize));
	m_node->addChild(constructCity(levelSize, m_levelName));
}

osg::ref_ptr<osg::Group> CityView::constructFloors(int levelSize)
{
	return LevelView::constructFloors(levelSize);
}

osg::ref_ptr<osg::Group> CityView::constructCity(int levelSize, std::string levelName)
{

	osg::ref_ptr<osg::Group> obstaclesGroup = new osg::Group();
	obstaclesGroup->setName("obstaclesGroup");

	osg::ref_ptr<osg::Group> readObstacles = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/generalized/01_00/berlin_l1.ive"));
	obstaclesGroup->addChild(readObstacles);

	return obstaclesGroup;
}

