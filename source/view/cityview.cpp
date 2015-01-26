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
}

//specific to cityview, called by LevelView()
void CityView::initSpecifics(std::shared_ptr<AbstractModel> model)
{
	m_model = model;
	 //freom blender file
	osg::Vec2 levelSize = pairToVec2(getCityModel()->getLevelSize());
	loadSpeedSigns();


	m_node->addChild(constructFloors(levelSize));
	m_node->addChild(constructCity(levelSize, m_levelName));
}

osg::ref_ptr<osg::Group> CityView::constructFloors(osg::Vec2 levelSize)
{
	osg::ref_ptr<osg::Group> floorsGroup = new osg::Group();

	osg::ref_ptr<osg::Node> floors = osgDB::readNodeFile("data/models/berlin/generalized/01_00/floor_highres.ive");
	floors->setNodeMask(CAMERA_MASK_MAIN);
	floors->setName("floorsNode");

	osg::StateSet *floorStateSet = floors->getOrCreateStateSet();
	setTexture(floorStateSet, "data/textures/berlin_ground_l1.tga", 0);

	//will be overwritten if reflection is used
	addShaderAndUniforms(static_cast<osg::ref_ptr<osg::Node>>(floors), shaders::FLOOR_CITY, levelSize, GLOW, 1.0);

	floors->setNodeMask(CAMERA_MASK_MAIN);
	floorsGroup->addChild(floors);

	osg::ref_ptr<osg::Group> radarFloors = constructRadarElementsForBoxes(getLevelModel()->getFloors());
	radarFloors->setNodeMask(CAMERA_MASK_RADAR);
	floorsGroup->addChild(radarFloors);

	return floorsGroup;
}

osg::ref_ptr<osg::Group> CityView::constructCity(osg::Vec2 levelSize, std::string levelName)
{

	osg::ref_ptr<osg::Group> obstaclesGroup = new osg::Group();
	obstaclesGroup->setName("obstaclesGroup");

	osg::ref_ptr<osg::Group> readObstacles = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/generalized/01_00/full_level.ive"));
	obstaclesGroup->addChild(readObstacles);

	//setTexture(readObstacles->getOrCreateStateSet(), "data/models/berlin/generalized/01_00/texatlas.tga", 0, true);
	addShaderAndUniforms(readObstacles, shaders::DEFAULT, levelSize, DEFAULT, 0.5, 1.0);

	return obstaclesGroup;
}

std::shared_ptr<CityModel> CityView::getCityModel()
{
	return std::dynamic_pointer_cast<CityModel>(m_model);
}




void CityView::loadSpeedSigns()
{
	std::string filePath("data/models/signs/");
	m_zone30 = osgDB::readNodeFile(filePath + "30Zone.ive");
	m_zone50 = osgDB::readNodeFile(filePath + "50Zone.ive");
	
	m_signsGroup = new osg::Group();
	m_signsGroup->setName("signsGroup");
	m_node->addChild(m_signsGroup);
	m_signsGroup->setNodeMask(CAMERA_MASK_MAIN);

	setTexture(m_signsGroup->getOrCreateStateSet(), "data/models/signs/speedlimits.tga", 0);
	addShaderAndUniforms(m_signsGroup, shaders::DEFAULT, osg::Vec2f(LEVEL_SIZE, LEVEL_SIZE), DEFAULT, 1.0);

	
}

void CityView::addSpeedZone(osg::Vec3 position, osg::Quat rotation, int speedLimit)
{
	std::cout << position.x() << std::endl;
	osg::ref_ptr<osg::MatrixTransform> mxt = new osg::MatrixTransform;
	osg::Matrixd mat = osg::Matrix::translate(position);
	mat.setRotate(rotation);
	mxt->setMatrix(mat);

	if (speedLimit == 30)
		mxt->addChild(m_zone30);
	else if (speedLimit == 50)
		mxt->addChild(m_zone50);

	m_signsGroup->addChild(mxt);
}
