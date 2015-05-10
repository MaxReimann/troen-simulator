#include "bikeview.h"
//osg
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/Material>
#include <osg/Geode>
#include <osgDB/WriteFile>
#include <osg/CullFace>
#include <osg/TexGenNode>
#include <osgUtil/CullVisitor>
#include <osg/ShapeDrawable>
#include <stdio.h>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
// troen
#include "../constants.h"
#include "shaders.h"
#include "../input/bikeinputstate.h"
#include "../model/bikemodel.h"
#include "playermarker.h"
#include "../resourcepool.h"
#include "../util/basicshapes.h"
#include "hudview.h"
#include "btBulletDynamicsCommon.h"

using namespace troen;

BikeView::BikeView(osg::Vec3 color, ResourcePool *resourcePool) : AbstractView(),
	m_playerColor(color)
{
	m_resourcePool = resourcePool;
	m_node = new osg::Group();
	m_pat = new osg::PositionAttitudeTransform();
	m_naviNode = new osg::Group();


	osg::Matrixd initialTransform;
	//initialTransform *= initialTransform.scale(btToOSGVec3(BIKE_DIMENSIONS));
	//initialTransform *= initialTransform.translate(osg::Vec3(0, 0, 1.0));
	//initialTransform *= initialTransform.rotate(180.0, )
	
	osg::Box* unitCube = new osg::Box(osg::Vec3(0, 0, 0), 0.2f);
	osg::ShapeDrawable* unitCubeDrawable = new osg::ShapeDrawable(unitCube);
	// Declare a instance of the geode class: 
	//osg::Geode* chassisGeode = new osg::Geode();
	//chassisGeode->addDrawable(unitCubeDrawable);

	osg::Group *carNode = static_cast<osg::Group*>(osgDB::readNodeFile("data/models/car/lambo.ive"));
    //carNode->getOrCreateStateSet()->setAttributeAndModes(shaders::m_allShaderPrograms[shaders::BIKE], osg::StateAttribute::ON);
	
	

	osg::MatrixTransform* matrixTransform = new osg::MatrixTransform(initialTransform);
	matrixTransform->addChild(carNode);
	m_wheelGroup = new osg::Group();
	m_pat->addChild(m_wheelGroup);

	wheels = std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>>();
	
	osg::Matrixd lookAtTransform;
	lookAtTransform *= lookAtTransform.translate(osg::Vec3(CAMERA_LOOKAT_POSITION_X, CAMERA_LOOKAT_POSITION_Y, CAMERA_LOOKAT_POSITION_Z));
	m_lookatPoint = new osg::MatrixTransform(lookAtTransform);
	m_lookatGeode = new osg::Geode();
	//m_lookatGeode->addDrawable(unitCubeDrawable);
	m_lookatGeode->setName("lookatGeode");
	m_lookatPoint->addChild(m_lookatGeode);

    osg::ref_ptr<osg::Light> light = new osg::Light();
    // each light must have a unique number
    light->setLightNum(5);
    // we set the light's position via a PositionAttitudeTransform object
    light->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));


    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
    lightSource->setLight(light);
    lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
    lightSource->setStateSetModes(*(m_pat->getOrCreateStateSet()), osg::StateAttribute::ON);

    osg::Matrixd lightTransform;
    lightTransform *= lightTransform.translate(osg::Vec3(0, 1.5, 3.0));
    osg::ref_ptr<osg::MatrixTransform> lightMatTransform = new osg::MatrixTransform(lightTransform);
    lightMatTransform->addChild(lightSource);


    m_pat->addChild(lightMatTransform);


	m_pat->addChild(m_lookatPoint);

	m_pat->addChild(matrixTransform);
	m_pat->setName("bikeGroup");

	m_pat->addChild(PlayerMarker(color).getNode());
	m_node->addChild(m_pat);

	m_naviNode->addChild(createNavigationArrow());
}

osg::ref_ptr<osg::Node> BikeView::createCyclePart(ResourcePool::ModelResource objName, ResourcePool::TextureResource specularTexturePath,
	ResourcePool::TextureResource diffuseTexturePath, ResourcePool::TextureResource normalTexturePath,
	int modelIndex, float glowIntensity)
{
	enum BIKE_TEXTURES { DIFFUSE, SPECULAR, NORMAL };

	osg::Node* Node = m_resourcePool->getNode(objName);

	//osgDB::writeNodeFile(*Node, std::string("file.osg")); //to look at the scenegraph
	osg::ref_ptr<osg::StateSet> NodeState = Node->getOrCreateStateSet();

	osg::ref_ptr<osg::Geode> singleGeode = dynamic_cast<osg::Geode*>(Node->asGroup()->getChild(0));
	osg::ref_ptr<osg::StateSet> childState = singleGeode->getDrawable(0)->getStateSet();
	osg::StateAttribute* stateAttributeMaterial = childState->getAttribute(osg::StateAttribute::MATERIAL);

	osg::Uniform* modelIndexU = new osg::Uniform("modelID", modelIndex);
	NodeState->addUniform(modelIndexU);

	osg::Uniform* glowIntensityU = new osg::Uniform("glowIntensity", glowIntensity);
	NodeState->addUniform(glowIntensityU);

	if (stateAttributeMaterial != nullptr)
	{
		osg::Material *objMaterial = dynamic_cast<osg::Material*>(stateAttributeMaterial);

		if (modelIndex != GLOW) {
			// if modelIndex == GLOW we will set it later (this avoids some ugly warnings from osg)
			osg::Vec4 diffuse = objMaterial->getDiffuse(osg::Material::FRONT_AND_BACK);
			osg::Uniform* diffuseMaterialColorU = new osg::Uniform("diffuseMaterialColor", diffuse);
			NodeState->addUniform(diffuseMaterialColorU);
		}

		osg::Vec4 ambient = objMaterial->getAmbient(osg::Material::FRONT_AND_BACK);
		osg::Uniform* ambientMaterialColorU = new osg::Uniform("ambientMaterialColor", ambient);
		NodeState->addUniform(ambientMaterialColorU);

		osg::Vec4 specular = objMaterial->getSpecular(osg::Material::FRONT_AND_BACK);
		osg::Uniform* specularMaterialColorU = new osg::Uniform("specularMaterialColor", specular);
		NodeState->addUniform(specularMaterialColorU);

		float shininess = objMaterial->getShininess(osg::Material::FRONT_AND_BACK);
		osg::Uniform* shininessU = new osg::Uniform("shininess", shininess);
		NodeState->addUniform(shininessU);
	}

	osg::Uniform* ColorU;
	if (modelIndex == GLOW) {
		ColorU = new osg::Uniform("playerColor", m_playerColor);
		// set parts to white/gray so that we can color it
		NodeState->addUniform(new osg::Uniform("diffuseMaterialColor", osg::Vec4(0.5f, 0.5f, 0.5f, 1.f)));
	}
	else {
		ColorU = new osg::Uniform("playerColor", osg::Vec3(1.f, 1.f, 1.f));
	}
	NodeState->addUniform(ColorU);

	if (specularTexturePath != ResourcePool::None)
	{
		osg::Uniform* specularMapU = new osg::Uniform("specularTexture", SPECULAR);
		NodeState->addUniform(specularMapU);
		setTexture(NodeState, specularTexturePath, SPECULAR);
	}

	if (diffuseTexturePath != ResourcePool::None)
	{
		osg::Uniform* diffuseMapU = new osg::Uniform("diffuseTexture", DIFFUSE);
		NodeState->addUniform(diffuseMapU);
		setTexture(NodeState, diffuseTexturePath, DIFFUSE);
	}

	if (normalTexturePath != ResourcePool::None){
		osg::Uniform* normalMapU = new osg::Uniform("normalTexture", NORMAL);
		NodeState->addUniform(normalMapU);
		setTexture(NodeState, normalTexturePath, NORMAL);
	}

	NodeState->setAttributeAndModes(shaders::m_allShaderPrograms[shaders::BIKE], osg::StateAttribute::ON);

	return Node;
}

void BikeView::setTexture(osg::ref_ptr<osg::StateSet> stateset, ResourcePool::TextureResource textureName, int unit)
{

	//osg::Image* image = osgDB::readImageFile(filePath);
	osg::Image* image = m_resourcePool->getImage(textureName);

	osg::Texture2D* texture = new osg::Texture2D;
	texture->setImage(image);
	texture->setResizeNonPowerOfTwoHint(false);

	stateset->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON);


}

void BikeView::setTexture(osg::ref_ptr<osg::StateSet> stateset, std::string filePath, int unit, bool override)
{

	osg::Image* image = osgDB::readImageFile(filePath);
	if (!image)
		std::cout << "[TroenGame::levelView]  File \"" << filePath << "\" not found." << std::endl;
	else
	{
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
		texture->setResizeNonPowerOfTwoHint(false);
		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
		if (override)
			stateset->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		else
			stateset->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON);
	}
}

void BikeView::update()
{
	m_MovieCycle_Body->getStateSet()->addUniform(new osg::Uniform("transform", m_pat->asMatrixTransform()->getMatrix()));
}

class CopyPAT : public osg::NodeCallback
{
public:
	CopyPAT(osg::PositionAttitudeTransform *pat) : m_copy_pat(pat) {}

	virtual void operator()(osg::Node* node,
		osg::NodeVisitor* nv)
	{
		if (!nv)
			return;

		osg::MatrixTransform *transform = static_cast<osg::MatrixTransform*>(node);
		
		float zoffset = 4.0;

		btMatrix3x3 rotMat(OSGToBtQuat((m_copy_pat->getAttitude())));
		float y, p, r;
		rotMat.getEulerYPR(y, p, r);
		btQuaternion zRot(btVector3(0, 0, 1), y);

		osg::Matrixd m(btToOSGQuat(zRot));
		m.setTrans(m_copy_pat->getPosition() + osg::Vec3(0,0,zoffset));
		transform->setMatrix(m);



		// Continue traversing so that OSG can process
		//   any other nodes with callbacks.
		traverse(node, nv);
	}

protected:
	osg::PositionAttitudeTransform *m_copy_pat;
};

osg::ref_ptr<osg::MatrixTransform> BikeView::createNavigationArrow()
{
	m_navigationArrowTransform = new osg::MatrixTransform();
	osg::ref_ptr<osg::Node> navi_arrow = osgDB::readNodeFile("data/models/navi_arrow.ive");
	m_navigationArrowTransform->addChild(navi_arrow);
	m_navigationArrowTransform->setCullCallback(new CopyPAT(m_pat));
	osg::ref_ptr<osg::StateSet> state = navi_arrow->getOrCreateStateSet();
	state->setMode(GL_BLEND, osg::StateAttribute::ON);
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	addShaderAndUniforms(state, shaders::DEFAULT, 0.5);
	setTexture(state, "data/textures/naviarrow.tga", 0, true);
	m_navigationArrowTransform->setNodeMask(CAMERA_MASK_ROUTE | CAMERA_MASK_MAIN);


	return m_navigationArrowTransform;
}


void BikeView::addShaderAndUniforms(osg::ref_ptr<osg::StateSet> stateSet, int shaderIndex, float alpha)
{
	stateSet->setAttributeAndModes(shaders::m_allShaderPrograms[shaderIndex], osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateSet->addUniform(new osg::Uniform("alpha", alpha));
}


void BikeView::createPlayerMarker(osg::Vec3 color)
{
	//PlayerMarker *marker = new PlayerMarker(color)
	m_playermarkerNode = PlayerMarker(color).getNode();
	m_pat->addChild(m_playermarkerNode);
}

void BikeView::addWheel(float radius, osg::Vec3 pointOne, osg::Vec3 pointTwo)
{
	osg::ref_ptr<osg::Geometry> geom =  BasicShapes::cylinderTriStrips(0.5, 10, pointOne, pointTwo);
	osg::ref_ptr<osg::Geode> wheelGeode = new osg::Geode();
	wheelGeode->addDrawable(geom);
	osg::ref_ptr<osg::PositionAttitudeTransform> wheelTransform = new osg::PositionAttitudeTransform;
	wheelTransform->setPivotPoint((pointOne + pointTwo) / 2);
	wheelTransform->setPosition((pointOne + pointTwo) / 2);
	wheelTransform->addChild(wheelGeode);
	wheels.push_back(wheelTransform);
	m_wheelGroup->addChild(wheelTransform);
}

void BikeView::removeWheels()
{
	m_wheelGroup->removeChildren(0, 4);
	wheels.clear();
}



void BikeView::setWheelRotation(int index, btTransform t)
{
	//osg::Vec3 pos = wheels[index]->getPosition();
	//pos = osg::Vec3(pos.x(), pos.y(), t.getOrigin().z());
	//wheels[index]->setPosition(pos);
	wheels[index]->setAttitude(btToOSGQuat((t.getRotation())));
}