#include "cityview.h"
// STD
#include <math.h>
// OSG
#include <osg/Notify>
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

//#define TEXTURED_MODEL

using namespace troen;


CityView::CityView(std::shared_ptr<AbstractModel> model, std::string levelName, bool texturedModel) :
LevelView(model, levelName), m_texturedModel(texturedModel)
{
}

//specific to cityview, called by LevelView()
void CityView::initSpecifics(std::shared_ptr<AbstractModel> model)
{
	m_model = model;
	//freom blender file
	osg::Vec2 levelSize = pairToVec2(getCityModel()->getLevelSize());
	loadSpeedSigns();

	auto floor = constructFloors(levelSize, 0);
	m_node->addChild(floor);
	m_node->addChild(constructCity(levelSize, 0));

	auto naviFloor = constructFloors(levelSize, 1);
	m_naviNode = new osg::Group();
	m_naviNode->addChild(naviFloor);
	m_naviNode->addChild(constructCity(levelSize, 1));
}

osg::ref_ptr<osg::Group> CityView::constructFloors(osg::Vec2 levelSize, int LOD)
{
	osg::ref_ptr<osg::Group> floorsGroup = new osg::Group();

	osg::ref_ptr<osg::Node> floors = osgDB::readNodeFile("data/models/berlin/generalized/01_00/floor_highres_scaled.ive");
	floors->setNodeMask(CAMERA_MASK_MAIN);
	floors->setName("floorsNode");

	osg::StateSet *floorStateSet = floors->getOrCreateStateSet();
	if (LOD == 0)
		setTexture(floorStateSet, "data/textures/berlin_ground_l1.tga", 0); //berlin_ground_l1.tga"
	else
		setTexture(floorStateSet, "data/textures/grey.tga", 0);

	//will be overwritten if reflection is used
	addShaderAndUniforms(static_cast<osg::ref_ptr<osg::Node>>(floors), shaders::FLOOR_CITY, levelSize, GLOW, 1.0);

	floors->setNodeMask(CAMERA_MASK_MAIN);
	floorsGroup->addChild(floors);

	osg::ref_ptr<osg::Group> radarFloors = constructRadarElementsForBoxes(getLevelModel()->getFloors());
	radarFloors->setNodeMask(CAMERA_MASK_RADAR);
	floorsGroup->addChild(radarFloors);

	return floorsGroup;
}


class OptimizeVisitor : public osg::NodeVisitor
{
public:
	OptimizeVisitor()
		: osg::NodeVisitor( // Traverse all children.
		osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
	}

	// This method gets called for every node in the scene
	//   graph. 
	virtual void apply(osg::Node& node)
	{
		node.getOrCreateStateSet()->setDataVariance(osg::Object::STATIC);
		node.setDataVariance(osg::Object::STATIC);
		node.setCullingActive(false);

		// Keep traversing the rest of the scene graph.
		traverse(node);
	}

	virtual void apply(osg::Geode& geode)
	{

		for (unsigned int i = 0; i < geode.getNumDrawables(); i++)
		{
			osg::Drawable* dr = geode.getDrawable(i);
			int numTextures = dr->getOrCreateStateSet()->getNumTextureAttributeLists();
			for (auto attribs : dr->getOrCreateStateSet()->getTextureAttributeList())
			{
				for (auto texattrib : attribs)
				{
					//delete textures after upload to gpu to save main memory
					if (texattrib.second.first != NULL)
					{
						osg::Texture *tex = texattrib.second.first->asTexture();
						tex->setUnRefImageDataAfterApply(true);
					}

				}
			}
		}

	}

};


osg::ref_ptr<osg::Group> CityView::constructCity(osg::Vec2 levelSize, int LODlevel)
{

	osg::ref_ptr<osg::Group> LODBuildings = new osg::Group();
	osg::ref_ptr<osg::Group> readObstacles = new osg::Group();

	if (LODlevel == 0)
	{

		LODBuildings->setName("L0CityGroup");//"data/models/berlin/generalized/01_00/L0scaled.ive""D:/Blender/troensimulator/Berlin3ds/Berlin3ds/all_merge_texattempt.ive"
		std::cout << "[CityView] reading level model.." << std::endl;

		if (m_texturedModel)
		{

			std::string cityParts[] = { "L11.ive", "L12.ive", "L12_up.ive", "L13.ive", "L13_up.ive",
				"L21.ive", "L21_up.ive", "L22.ive", "L23.ive", "L31.ive", "L31_up.ive", "L32.ive", "L33.ive" };

			float heightAdjusts[] = { /*L11*/37, 40, 37, 37, 37,
				/*L21*/37, 37, 37, 37,
				/*L31*/10, 37, 37, 37 };

			int i = 0;
			for (auto part : cityParts)
			{
				osg::ref_ptr<osg::MatrixTransform> partTransform = new osg::MatrixTransform();
				osg::Matrix trans = osg::Matrix::translate(osg::Vec3(0, 0, -heightAdjusts[i])); //transform in blender
				partTransform->setMatrix(trans);

				std::cout << "[CityView] reading " << part << std::endl;
				osg::Group *geode = static_cast<osg::Group*>(osgDB::readNodeFile(std::string("data/models/berlin/ive/") + part));
				partTransform->addChild(geode);
				readObstacles->addChild(partTransform);
				i++;
			}

			readObstacles->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
			//readObstacles->setCullingActive(false);
			readObstacles->getOrCreateStateSet()->setDataVariance(osg::Object::STATIC);
			readObstacles->setDataVariance(osg::Object::STATIC);

			//set all subNodes to static Data Variance
			OptimizeVisitor *staticMaker = new OptimizeVisitor();
			readObstacles->accept(*staticMaker);
		}
		else
		{
			readObstacles = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/generalized/01_00/L0scaled.ive")); // #"data/models/berlin/textured/3850_5817.obj"
		}
		//setTexture(readObstacles->getOrCreateStateSet(), "data/models/berlin/textured/packed_3850_58170.tga", 0, true);
		if (readObstacles == nullptr)
			printf("reading model failed.. \n");
	}
	else if (LODlevel == 1)
	{
		LODBuildings->setName("L1CityGroup");
		// l1model: "data/models/berlin/generalized/01_01/L1level.ive" <-- if we use this, routes are not seen clrearly anymore ..
		readObstacles = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/generalized/01_01/L1level_scaled.ive"));
	}


	LODBuildings->addChild(readObstacles);

	if (LODlevel == 0)
		addShaderAndUniforms(readObstacles, shaders::DEFAULT, levelSize, DEFAULT, 0.5, 1.0);
	else
	{
		addShaderAndUniforms(readObstacles, shaders::LOD1BUILDINGS, levelSize, DEFAULT, 0.5, 1.0);
		readObstacles->getStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		readObstacles->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);

		// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
		readObstacles->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	}

	return LODBuildings;
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
