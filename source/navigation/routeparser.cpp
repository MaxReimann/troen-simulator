#include "routeparser.h"
//qt
#include <qdir.h>
#include <qstringlist.h>
#include <qfile.h>
// osg
#include <osgViewer/View>
#include <osgViewer/config/SingleWindow>
#include <osg/ValueObject>

// troen
#include "../controller/bikecontroller.h"


using namespace troen;

RouteParser::RouteParser()
{
	m_routes = std::vector<Route>();

	//if (!QDir("data/routes").exists())
		//return;

	QDir myDir("data/routes");
	QStringList filesList = myDir.entryList();

	for (QString fileName : filesList)
	{
		if (!fileName.endsWith(".route"))
			continue;
		std::cout << (myDir.path() + myDir.separator() + fileName).toStdString() << std::endl;
		QFile routeFile(myDir.path() + myDir.separator() +  fileName);
		if (routeFile.open(QFile::ReadOnly))
		{
			Route currentRoute;
			parse(routeFile.readAll().trimmed(), currentRoute);
			m_routes.push_back(currentRoute);
		}
	}
}



bool RouteParser::parse(QByteArray& contents, Route& currentRoute)
{

	QList<QByteArray> lines = contents.split('\n');

	currentRoute.difficulty = QString(lines.takeFirst()) == QString("easy") ? DIFFICULTY_EASY : DIFFICULTY_HARD;
	currentRoute.waypoints = std::vector<osg::Vec3>();
	
	for (auto line : lines)
	{
		try{
			osg::Vec3 val = lineToPoint(line);
			currentRoute.waypoints.push_back(val);
		}
		catch (char* exception){
			std::cout << "error parsing line :" << QString(line).toStdString() << std::endl;
		}

	}

	return true;
}

osg::Vec3 inline RouteParser::lineToPoint(QByteArray line)
{
	QList<QByteArray> pos = line.split('\t');
	if (pos.length() < 3)
	{
		throw "line parsing error";
	}

	double dpos[3] {0.0,0.0,0.0};
	dpos[0] = QString(pos[0]).toDouble();
	dpos[1] = QString(pos[1]).toDouble();
	dpos[2] = QString(pos[2]).toDouble();

	return osg::Vec3(dpos[0], dpos[1], dpos[2]);

}