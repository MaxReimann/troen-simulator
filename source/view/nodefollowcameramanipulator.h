#pragma once
// OSG
#include <osgGA/NodeTrackerManipulator>
#include <osg/Matrixd>
// troen
#include "../forwarddeclarations.h"

namespace troen
{

	//class orbit : public osg::NodeCallback
	//{
	//public:
	//	orbit() : heading(M_PI / 2.0) {}

	//	osg::Matrix getWCMatrix(){ return worldCoordMatrix; }

	//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	//	{
	//		osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
	//		if (tx != NULL)
	//		{
	//			heading += M_PI / 180.0;
	//			osg::Matrixd orbitRotation;
	//			orbitRotation.makeRotate(
	//				osg::DegreesToRadians(-10.0), osg::Vec3(0, 1, 0), // roll
	//				osg::DegreesToRadians(-20.0), osg::Vec3(1, 0, 0), // pitch
	//				heading, osg::Vec3(0, 0, 1)); // heading
	//			osg::Matrixd orbitTranslation;
	//			orbitTranslation.makeTranslate(0, -40, 4);
	//			tx->setMatrix(orbitTranslation * orbitRotation);
	//			worldCoordMatrix = osg::computeLocalToWorld(nv->getNodePath());
	//		}
	//		traverse(node, nv);
	//	}
	//private:
	//	osg::Matrix worldCoordMatrix;
	//	float heading;
	//};



	/*! The NodeFollowCameraManipulator is a osg::NodeTrackerManipulator and is used to point the camera always to the bike of the current player.*/
	class NodeFollowCameraManipulator : public osgGA::NodeTrackerManipulator
	{
	public:
		virtual osg::Matrixd getMatrix() const;
		virtual osg::Matrixd getInverseMatrix() const;
		virtual void setByMatrix(const osg::Matrixd& matrix);
		virtual void setByInverseMatrix(const osg::Matrixd& matrix);

		virtual void computeNodeCenterAndRotation(osg::Vec3d& nodeCenter, osg::Quat& nodeRotation) const;

		virtual void setBikeInputState(osg::ref_ptr<input::BikeInputState> bikeInputState);

	protected:
		osg::Vec3 rollPitchYaw(float x, float y, float z, float w) const;
		osg::ref_ptr<input::BikeInputState> m_bikeInputState;
		mutable osg::Quat m_oldPlayerViewingRotation;
	};
}