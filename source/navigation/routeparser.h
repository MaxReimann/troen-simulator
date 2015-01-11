#pragma once

// OSG
#include <osgViewer/View>
#include <osgText/Text>

// troen
#include "../sampleosgviewer.h"
#include "../forwarddeclarations.h"
#include "../constants.h"


namespace troen
{

	class Route
	{
	public:
		troen::trackDifficulty difficulty;
		std::vector<osg::Vec3> waypoints;
		std::string filePath;

		btTransform getTransform(int index);

	};

	class RouteParser
	{
	public:
		RouteParser::RouteParser();
		bool parse(QByteArray& contents, Route& currentRoute);
		osg::Vec3 inline lineToPoint(QByteArray line);

		std::vector<Route> routes() { return m_routes; };

	private:
		std::vector<Route> m_routes;
	};
}