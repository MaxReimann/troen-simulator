#pragma once
// OSG
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Shader>
#include <osg/Program>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
// troen
#include "../forwarddeclarations.h"
#include "abstractview.h"
#include "../resourcepool.h"
#include "LinearMath/btQuaternion.h"


namespace troen
{


	class BikeView : public AbstractView
	{
		friend class BikeController;
	public:
		BikeView(const osg::Vec3 color, ResourcePool* resourcePool);
		osg::ref_ptr<osg::Node> createCyclePart(
			ResourcePool::ModelResource objName,
			ResourcePool::TextureResource specularTexturePath,
			ResourcePool::TextureResource diffuseTexturePath,
			ResourcePool::TextureResource normalTexturePath,
			int modelIndex,
			float glowIntensity = 1.f);

		osg::ref_ptr<osg::PositionAttitudeTransform> m_pat;
		osg::ref_ptr<osg::Node> m_MovieCycle_Body;

		void update();
		void createPlayerMarker(const osg::Vec3 color);
		void addWheel(float radius, osg::Vec3 pointOne, osg::Vec3 pointTwo);
		void setWheelRotation(int index, btTransform t);
		osg::ref_ptr<osg::Geode> getLookatGeode() { return m_lookatGeode; }
		osg::ref_ptr<osg::MatrixTransform> getLookatTransform() { return m_lookatPoint; };
		osg::ref_ptr<osg::MatrixTransform>  createNavigationArrow();
		void removeWheels();
	private:
		void setTexture(osg::ref_ptr<osg::StateSet> stateset, const ResourcePool::TextureResource textureName, const int unit);
		void setTexture(osg::ref_ptr<osg::StateSet> stateset, std::string filePath, int unit, bool override);
		void addShaderAndUniforms(osg::ref_ptr<osg::StateSet> stateSet, int shaderIndex, float alpha);
		osg::Vec3 m_playerColor;
		osg::ref_ptr<osg::Node> m_playermarkerNode;
		ResourcePool* m_resourcePool;
		osg::ref_ptr<osg::Group> m_wheelGroup;
		std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> wheels;
		osg::ref_ptr<osg::MatrixTransform> m_lookatPoint;
		osg::ref_ptr<osg::Geode>  m_lookatGeode;
		osg::ref_ptr<osg::MatrixTransform> m_navigationArrowTransform;
		osg::ref_ptr<osg::Group> m_naviNode;
	};
}