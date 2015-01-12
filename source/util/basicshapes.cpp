#include "basicshapes.h"

#include <vector>
#include <algorithm>
#include "osg\Geometry"

#include "../constants.h"

using namespace troen;


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to transform points.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
	struct TransformPoints
	{
		TransformPoints(const osg::Matrix& m) : _m(m) { }

		template < class T >
		void operator () (T& t) const
		{
			t = _m.preMult(t);
		}

	private:
		osg::Matrix _m;
	};
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to transform normal vectors.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
	struct TransformNormals
	{
		TransformNormals(const osg::Matrix& m) : _m(m) { }

		template < class T >
		void operator () (T& t) const
		{
			t = osg::Matrix::transform3x3(t, _m);
		}

	private:
		osg::Matrix _m;
	};
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to normalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
	struct NormalizeFunctor
	{
		NormalizeFunctor() { }

		template < class T >
		void operator () (T& t) const
		{
			t.normalize();
		}
	};
};


template < class Points, class Normals, class Attributes >
inline void BasicShapes::cylinder(double radius, unsigned int sides, Points& points, Normals& normals, Attributes& attributes, bool normalize)
{
    typedef typename Points::value_type Point;
    typedef typename Normals::value_type Normal;
	typedef typename Attributes::value_type Attribute;

    Point p(0.0, 1.0, 0.0);

    for (unsigned int i = 0; i < sides; ++i)
    {
        const double u(static_cast <double> (i) / static_cast <double> (sides - 1));
        const double theta(u * PI * 2);
        const double x(radius * sin(theta));
        const double z(radius * cos(theta));

        Point v0(static_cast <typename Point::value_type> (x),
            static_cast <typename Point::value_type> (0),
            static_cast <typename Point::value_type> (z));

        // Store points now.
        points.push_back(v0 + p);
        points.push_back(v0);

		attributes.push_back(0.0f); //top
		attributes.push_back(0.7f); //bottom


        // Normalize and save as normal vector.
        if (normalize)
            v0.normalize();
        normals.push_back(v0);
        normals.push_back(v0);
    }
}


osg::ref_ptr<osg::Geometry> BasicShapes::cylinderTriStrips(float radius, unsigned int sides, const osg::Vec3& pointOne, const osg::Vec3& pointTwo)
{
	osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec3Array > normals = new osg::Vec3Array;
	osg::ref_ptr< osg::FloatArray > relativeHeights = new osg::FloatArray();


    // Reserve enough room.
    vertices->reserve(sides * 4 + 2);
    normals->reserve(sides * 4 + 2);
	relativeHeights->reserve(sides * 4 + 2);

    // Make the body.
	cylinder(radius, sides, *vertices, *normals, *relativeHeights, false);

    // Get distance.
    osg::Vec3 dist(pointTwo - pointOne);
    float d(dist.length());
    dist.normalize();

    // Scale to the right size.
    osg::Matrix scale(osg::Matrix::scale(1, d, 1));

    // Rotate from y-axis to proper orientation.
    osg::Matrix rotate(osg::Matrix::rotate(osg::Vec3(0.0, 1.0, 0.0), dist));

    // Translate to the correct location.
    osg::Matrix translate(osg::Matrix::translate(pointOne));

    osg::Matrix matrix(scale * rotate * translate);

    // Move the vertices into the right location.
    std::for_each(vertices->begin(), vertices->end(), Detail::TransformPoints(matrix));

    // Calculate the normals for the tri-strips.
    std::for_each(normals->begin(), normals->end(), Detail::NormalizeFunctor());
    std::for_each(normals->begin(), normals->end(), Detail::TransformNormals(rotate));

    // Save the size.
    unsigned int size(vertices->size());
    unsigned int fanSize(sides + 1);

    // Add the top fan
    vertices->push_back(pointOne);
	for (unsigned int i = 1; i < size; i += 2)
	{
        vertices->push_back(vertices->at(i));
		relativeHeights->push_back(1.0); //top
	}

    // Add the bottom fan.
    vertices->push_back(pointTwo);
	for (unsigned int i = 0; i < size; i += 2)
	{
        vertices->push_back(vertices->at(i));
		relativeHeights->push_back(0.0); //bottom
	}

    // Insert the normals for the fans.
    normals->insert(normals->end(), fanSize, osg::Vec3(0.0, -1.0, 0.0) * rotate);
    normals->insert(normals->end(), fanSize, osg::Vec3(0.0, 1.0, 0.0) * rotate);

    // Set the vertices and normals.
    geometry->setVertexArray(vertices);
    geometry->setNormalArray(normals);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(5, relativeHeights);
	geometry->setVertexAttribBinding(5, osg::Geometry::BIND_PER_VERTEX);

    // Add the primitive sets.
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, size));
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, size, fanSize));
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, size + fanSize, fanSize));

    /*_cylinders[key] = geometry;
    return geometry.get();*/
    return geometry;
}
