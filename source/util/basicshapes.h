#pragma once
// STD

//OSG
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Vec4>
#include <osg/PositionAttitudeTransform>
#include <osg/ref_ptr>

// troen
#include "../forwarddeclarations.h"

namespace troen
{
	class BasicShapes
	{
	public:
		static osg::ref_ptr<osg::Geometry> cylinderTriStrips(float radius, unsigned int sides, const osg::Vec3& pointOne, const osg::Vec3& pointTwo);
	private:
		template < class Points, class Normals, class Attributes >
		static inline void cylinder(double radius, unsigned int sides, Points& points, Normals& normals, Attributes& attributes, bool normalize);

	};
}