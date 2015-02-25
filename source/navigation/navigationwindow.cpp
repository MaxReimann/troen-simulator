#include "navigationwindow.h"
// osg

#include <osgViewer/View>
#include <osgViewer/config/SingleWindow>
#include <osgViewer/config/SingleScreen>
#include <osg/ValueObject>

// troen
#include "../constants.h"
#include "../view/nodefollowcameramanipulator.h"
#include "../controller/bikecontroller.h"
#include "../model/bikemodel.h"
#include "../gameeventhandler.h"


using namespace troen;




class CameraCopyCallback : public osg::NodeCallback
{
public:
	osg::ref_ptr<osgViewer::View> m_navView;
	std::shared_ptr<BikeController> bikeController;


	CameraCopyCallback(std::shared_ptr<BikeController> controller, osg::ref_ptr<osgViewer::View> navigationView) : NodeCallback()
	{
		m_navView = navigationView;
		bikeController = controller;

	}

	void operator()(osg::Node *node, osg::NodeVisitor *nv)
	{
		if (nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
		{
 
			osg::Vec3f gameEye, c, gameUp;
			osg::Vec3f eye, center, up;

			eye = bikeController->getModel()->getPositionOSG() - btToOSGVec3(bikeController->getModel()->getDirection()) * 50 + osg::Vec3d(0.0, 0, CAMERA_NAVI_EYE_POSITION_Z);
			center = bikeController->getModel()->getPositionOSG() + osg::Vec3d(0.0, 0, CAMERA_NAVI_CENTER_POSITION_Z);
			//center.set(center.x(), center.y(), 28.0);
			up = osg::Vec3d(0.0, 0.0, 1.0);
			m_navView->getCamera()->setViewMatrixAsLookAt(eye, center, up);


			double fovy, aspect, znear, zfar;
			m_navView->getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, znear, zfar);
			m_navView->getCamera()->setProjectionMatrixAsPerspective(FOVY_INITIAL_NAVI + FOVY_ADDITION_MAX * 1.5, aspect, znear, zfar);


		}
		//dont traverse ?
		this->traverse(node, nv);
	}
};

NavigationWindow::NavigationWindow(std::shared_ptr<BikeController> bikeController, osg::ref_ptr<GameEventHandler> eventHandler,
	SampleOSGViewer *viewer, bool fullscreen) :
m_viewer(viewer)
{
	m_rootNode = new osg::Group();
	m_view = new osgViewer::View();
	m_viewer->addView(m_view);

	m_view->getCamera()->setCullMask(CAMERA_MASK_MAIN);
	m_view->setSceneData(m_rootNode);
	m_view->addEventHandler(eventHandler.get());
	m_view->setUserValue("window_type", (int) NAVIGATION_WINDOW);
	
	if (!fullscreen)
		m_view->apply(new osgViewer::SingleWindow(DEFAULT_MAINWINDOW_WIDTH + 230, 200, DEFAULT_MAINWINDOW_WIDTH / 2, DEFAULT_MAINWINDOW_HEIGHT / 2));
	
	m_view->getCamera()->setCullCallback(new CameraCopyCallback(bikeController, m_view));
}

void NavigationWindow::addElements(osg::ref_ptr<osg::Group> group)
{
	m_rootNode->addChild(group);
}
