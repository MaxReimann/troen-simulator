#pragma once
// OSG
#include <osg/Geometry>
// troen
#include "../forwarddeclarations.h"
#include "abstractview.h"

namespace troen
{
	class FenceView : public AbstractView
	{
	public:
		FenceView();
		void addFencePart(osg::Vec3 a, osg::Vec3 b);

	private:
		void initializeFence();
		void initializeShader();
		osg::Geometry* m_geometry;
		std::vector<osg::Vec3> m_coordinates;
		osg::DrawArrays* m_drawArrays;
	};
}