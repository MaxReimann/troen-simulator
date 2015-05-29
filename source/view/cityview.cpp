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
#include "osg/BlendFunc"
#include "../player.h"

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
	{
		floorsGroup->addChild(floors);
		setTexture(floorStateSet, "data/textures/berlin_ground_l1.tga", 0); //berlin_ground_l1.tga"
	}
	else
	{
		setTexture(floorStateSet, "data/textures/grey.tga", 0);
		osg::ref_ptr<osg::MatrixTransform> z_adjust = new osg::MatrixTransform(osg::Matrix::translate(osg::Vec3(0.0, 0.0, -0.5)));
		floorsGroup->addChild(z_adjust);
		z_adjust->addChild(floors);

		//y adjust to push back ground floor to prevent z flickering
		osg::ref_ptr<osg::Uniform> bendingYAdjust = new osg::Uniform("bendingYAdjust", true);
		floorStateSet->addUniform(bendingYAdjust);
	}

	//will be overwritten if reflection is used
	addShaderAndUniforms(static_cast<osg::ref_ptr<osg::Node>>(floors), shaders::FLOOR_CITY, levelSize, GLOW, 1.0);

	floors->setNodeMask(CAMERA_MASK_MAIN);

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

class FilterSettingVisitor : public osg::NodeVisitor
{
public:
	FilterSettingVisitor()
		: osg::NodeVisitor( // Traverse all children.
		osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
	}

	// This method gets called for every node in the scene
	//   graph. 
	virtual void apply(osg::Node& node) { traverse(node);}

	virtual void apply(osg::Geode& geode)
	{

		for (auto dr : geode.getDrawableList())
			for (auto attribs : dr->getOrCreateStateSet()->getTextureAttributeList())
				for (auto texattrib : attribs)
					if (texattrib.second.first != NULL)
					{
						osg::Texture *tex = texattrib.second.first->asTexture();
						tex->setMaxAnisotropy(16.0);
					}
	}

};

void CityView::constructL0City(osg::ref_ptr<osg::Group> obstacleNode, osg::Vec2 levelSize)
{

	if (m_texturedModel)
	{

		std::string cityParts[] = { "L11.ive", "L12.ive", "L12_up.ive", "L13.ive", "L13_up.ive",
			"L21.ive", "L21_up.ive", "L22.ive", "L23.ive", "L31.ive", "L31_up.ive", "L32.ive", "L33.ive", "roads.ive" };

		float heightAdjusts[] = { /*L11*/37, 40, 37, 37, 37,
			/*L21*/37, 37, 37, 37,
			/*L31*/10, 37, 37, 37,
			/*roads*/ -0.1 };

		int i = 0;
		for (auto part : cityParts)
		{
			osg::ref_ptr<osg::MatrixTransform> partTransform = new osg::MatrixTransform();
			osg::Matrix trans = osg::Matrix::translate(osg::Vec3(0, 0, -heightAdjusts[i])); //transform in blender
			partTransform->setMatrix(trans);

			std::cout << "[CityView] reading " << part << std::endl;
			osg::Group *geode = static_cast<osg::Group*>(osgDB::readNodeFile(std::string("data/models/berlin/ive/") + part));
			partTransform->addChild(geode);
			obstacleNode->addChild(partTransform);
			i++;

			if (part.compare("roads.ive") == 0)
			{
				osg::ref_ptr<FilterSettingVisitor> filterVisitor = new FilterSettingVisitor();
				geode->accept(*filterVisitor.get());
				std::cout << "set texture to anisotropic filtering" << std::endl;
			}
		}

		obstacleNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
		//readObstacles->setCullingActive(false);
		obstacleNode->getOrCreateStateSet()->setDataVariance(osg::Object::STATIC);
		obstacleNode->setDataVariance(osg::Object::STATIC);

		//set all subNodes to static Data Variance
		osg::ref_ptr<OptimizeVisitor> staticMaker = new OptimizeVisitor();
		obstacleNode->accept(*staticMaker);
	}
	else
	{
		obstacleNode->addChild(static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/ive/L0Untextured.ive"))); // #"data/models/berlin/textured/3850_5817.obj"
	}
	//setTexture(readObstacles->getOrCreateStateSet(), "data/models/berlin/textured/packed_3850_58170.tga", 0, true);
	if (obstacleNode == nullptr)
		printf("reading model failed.. \n");

	addShaderAndUniforms(obstacleNode, shaders::DEFAULT, levelSize, DEFAULT, 0.5, 1.0);
}

class SelectiveObjectShaders : public osg::NodeVisitor
{
public:
    SelectiveObjectShaders()
        : osg::NodeVisitor( // Traverse all children.
        osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }


    virtual void apply(osg::Node& node)
    {
        if (node.getName() == "railwaynavi.001")
        {
            auto stateset = node.getOrCreateStateSet();
            osg::ref_ptr<osg::Uniform> distanceBasedAlpha = new osg::Uniform("distanceBasedAlpha", true);
            stateset->addUniform(distanceBasedAlpha);
        }

        traverse(node);
    }
};


void CityView::constructL1City(osg::ref_ptr<osg::Group> obstacleNode, osg::Vec2 levelSize)
{
	
	// l1model: "data/models/berlin/generalized/01_01/L1level.ive" <-- if we use this, routes are not seen clearly anymore ..
    osg::ref_ptr<osg::Group> loadedModel = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/berlin/ive/navimap.ive"));
	obstacleNode->addChild(loadedModel);

	addShaderAndUniforms(obstacleNode, shaders::LOD1BUILDINGS, levelSize, DEFAULT, 0.5, 1.0);
	auto stateset = obstacleNode->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//set transparent and number higher then normal transparency to render after other transparent geometry
	//this is especially for rendering after the route is rendered
	stateset->setRenderBinDetails(100, "DepthSortedBin");
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

	//y adjust to push back ground floor to prevent z flickering
	osg::ref_ptr<osg::Uniform> bendingYAdjust = new osg::Uniform("bendingYAdjust", false);
	stateset->addUniform(bendingYAdjust);

    osg::ref_ptr<osg::Uniform> distanceBasedAlpha = new osg::Uniform("distanceBasedAlpha", false);
    stateset->addUniform(distanceBasedAlpha);

    osg::ref_ptr<SelectiveObjectShaders> setNaviRoadTransparent = new SelectiveObjectShaders();
    loadedModel->accept(*setNaviRoadTransparent.get());
}

osg::ref_ptr<osg::Group> CityView::constructCity(osg::Vec2 levelSize, int LODlevel)
{

	osg::ref_ptr<osg::Group> LODBuildings = new osg::Group();

	if (LODlevel == 0)
	{
		LODBuildings->setName("L0CityGroup");//"data/models/berlin/generalized/01_00/L0scaled.ive""D:/Blender/troensimulator/Berlin3ds/Berlin3ds/all_merge_texattempt.ive"
		std::cout << "[CityView] reading LOD0 level model.." << std::endl;

		constructL0City(LODBuildings, levelSize);
	}
	else if (LODlevel == 1)
	{
		LODBuildings->setName("L1CityGroup");
		std::cout << "[CityView] reading LOD1 level model.." << std::endl;

		constructL1City(LODBuildings, levelSize);
	}


	LODBuildings->setNodeMask(CAMERA_MASK_MAIN);

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
