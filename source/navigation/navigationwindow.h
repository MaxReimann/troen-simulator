#pragma once

// OSG
#include <osgViewer/View>
#include <osgText/Text>

// troen
#include "../sampleosgviewer.h"
#include "../forwarddeclarations.h"


namespace troen
{

	class NavigationWindow
	{
	public:
		NavigationWindow(std::shared_ptr<BikeController> controller);
		void addElements(osg::ref_ptr<osg::Group> group);

		osg::ref_ptr<osgViewer::View> mapView()		{ return m_view; };
		osg::ref_ptr<osg::Group> mapNode()			{ return m_rootNode; };
		osg::ref_ptr<SampleOSGViewer> navViewer()		{ return m_viewer; };

	private:
		osg::ref_ptr<osgViewer::View>	m_view;
		osg::ref_ptr<SampleOSGViewer>	m_viewer;
		osg::ref_ptr<osg::Group>		m_rootNode;

	};
}