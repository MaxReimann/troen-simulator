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
#include "../view/shaders.h"


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
	SampleOSGViewer *viewer, osg::Viewport *viewport, bool fullscreen) :
m_viewer(viewer)
{
	m_sceneNode = new osg::Group();
	m_rootNode = new osg::Group();
	m_view = new osgViewer::View();
	m_viewer->addView(m_view);

	m_view->getCamera()->setCullMask(CAMERA_MASK_MAIN);
	m_view->setSceneData(m_sceneNode);
	m_view->addEventHandler(eventHandler.get());
	m_view->setUserValue("window_type", (int) NAVIGATION_WINDOW);
	
	if (!fullscreen)
		m_view->apply(new osgViewer::SingleWindow(DEFAULT_MAINWINDOW_WIDTH + 230, 200, DEFAULT_MAINWINDOW_WIDTH / 2, DEFAULT_MAINWINDOW_HEIGHT / 2));
	else
		m_view->getCamera()->setViewport(viewport);
	
	m_view->getCamera()->setCullCallback(new CameraCopyCallback(bikeController, m_view));

	auto preRenderCam = createPreRenderCamera(viewport->width(), viewport->height());
	preRenderCam->addChild(m_sceneNode);
	m_rootNode->addChild(preRenderCam);
	preRenderCam->setCullCallback(new CameraCopyCallback(bikeController, m_view));





	m_debugView = new osgViewer::View();
	m_viewer->addView(m_debugView);

	m_debugView->getCamera()->setReferenceFrame(osg::Camera::ABSOLUTE_RF);

	auto stateset = m_view->getCamera()->getOrCreateStateSet();
	//stateset->addUniform(new osg::Uniform("routeMask", 3));
	//stateset->setTextureAttributeAndModes(3, m_fboTexture, osg::StateAttribute::ON);

	osg::Geode* geode(new osg::Geode());
	geode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(viewport->width(), 0, 0), osg::Vec3(0, viewport->height(), 0)));
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_debugView->getCamera()->addChild(geode);
	osg::ref_ptr<osg::Viewport> debugViewport = new osg::Viewport(viewport->x(), viewport->y() - viewport->height() + 10, viewport->width(), viewport->height());

	m_debugView->getCamera()->setViewport(debugViewport);
	m_debugView->setSceneData(geode);
	m_debugView->getCamera()->setRenderOrder(osg::Camera::POST_RENDER);
	m_debugView->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	//m_preRenderCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	auto state = m_debugView->getCamera()->getOrCreateStateSet();
	//state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	state->setAttributeAndModes(shaders::m_allShaderPrograms[shaders::LOD1BUILDINGS], osg::StateAttribute::ON);
	state->addUniform(new osg::Uniform("routeMask", 3));
	state->setTextureAttributeAndModes(3, m_fboTexture, osg::StateAttribute::ON);
	//m_debugView->getCamera()->setProjectionMatrixAsOrtho(-10, 10, -10, 10, -10, 10);
	
	float znear = 1.0f;
	float zfar = 3.0f;

	// 2:1 aspect ratio as per flag geomtry below.
	float proj_top = 0.5f*znear;
	float proj_right = 0.5f*znear;

	znear *= 0.9f;
	zfar *= 1.1f;

	// set up projection.
	m_debugView->getCamera()->setProjectionMatrixAsFrustum(-proj_right, proj_right, -proj_top, proj_top, znear, zfar);

	// set view
	m_debugView->getCamera()->setViewMatrixAsLookAt(osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));

}

void NavigationWindow::addElements(osg::ref_ptr<osg::Group> group)
{
	m_sceneNode->addChild(group);
}


osg::ref_ptr<osg::Camera> NavigationWindow::createPreRenderCamera(int sizeX, int sizeY)

{
	m_preRenderCam = new osg::Camera();

	m_preRenderCam->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	m_preRenderCam->setRenderOrder(osg::Camera::PRE_RENDER);
	m_preRenderCam->setClearColor(osg::Vec4(0.0f, 0.f, 0.0f, 0.0f));
	m_preRenderCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	//m_preRenderCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	//m_preRenderCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_preRenderCam->setViewport(0, 0, sizeX, sizeY);
	m_preRenderCam->setClearDepth(1.0);
	//m_preRenderCam->setCullMask(CAMERA_MASK_ROUTE);

	m_fboTexture = new osg::Texture2D();
	m_fboTexture->setTextureSize(sizeX, sizeY);
	m_fboTexture->setInternalFormat(GL_RGBA);
	m_fboTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_fboTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	m_preRenderCam->attach((osg::Camera::BufferComponent) osg::Camera::COLOR_BUFFER0, m_fboTexture);

	m_preRenderCam->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);


	auto stateset = m_view->getCamera()->getOrCreateStateSet();
	stateset->addUniform(new osg::Uniform("routeMask", 3));
	stateset->setTextureAttributeAndModes(3, m_fboTexture, osg::StateAttribute::ON);

	return m_preRenderCam;
}