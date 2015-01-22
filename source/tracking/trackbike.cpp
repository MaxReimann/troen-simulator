#include "trackbike.h"
#include "..\controller\bikecontroller.h"
#include "..\model\bikemodel.h"
#include "qdir.h"


using namespace troen::tracking;

troen::tracking::TrackBike::TrackBike(std::shared_ptr<BikeController> controller, float frequency, int participantNumber, bool exportCSV) :
m_controller(controller), m_frequency(frequency), m_participantNumber(participantNumber), m_exportCSV(exportCSV)
{
	m_trackedStates = std::vector<CurrentBikeState>();

	if (m_exportCSV)
	{
		QString fileDir("export" + QString(QDir::separator()) + "user" + QString::number(m_participantNumber));
		std::cout << fileDir.toStdString() << std::endl;
		bool success = true;
		if (!QDir(fileDir).exists())
			success = QDir().mkpath(fileDir);

		if (!success)
		{
			std::cout << "couldnt create export directory" << std::endl;
			return;
		}

		m_file = new QFile(fileDir + QString(QDir::separator()) + "trajectories.csv");
		if (m_file->open(QFile::WriteOnly | QFile::Truncate))
		{
			m_fileStream = new QTextStream(m_file);
			(*m_fileStream) << "time" << "\t" << "x" << "\t" << "y" << "\t" << "z" << "\t" << "yaw" << "\t" << "velocity" << "\n";
		}
	}

	m_wallCrashes = 0;
	m_wrongTurns = 0;
}



//gets called every frame
void TrackBike::update(long double gameTime)
{
	if (m_lastUpdate == NULL || gameTime - m_lastUpdate > 1.0f / m_frequency)
	{
		m_lastUpdate = gameTime;

		CurrentBikeState bikeState = CurrentBikeState();
		bikeState.position = m_controller->getModel()->getPositionBt();
		bikeState.rotation = m_controller->getModel()->getRotationQuat();
		bikeState.time = gameTime;
		bikeState.velocity = m_controller->getModel()->getVelocity();

		m_trackedStates.push_back(bikeState);
	}
}



void TrackBike::writeTrajectoryLine(CurrentBikeState& state)
{
	//get rotation around x axis from quat
	btMatrix3x3 m; 
	m.setRotation(state.rotation);
	btScalar yaw, pitch, roll;
	m.getEulerYPR(yaw, pitch, roll);
	
	std::string s = std::to_string(static_cast<double>(state.time));
	QString q = QString::fromStdString(s);

	(*m_fileStream) << q << "\t";
	(*m_fileStream) << state.position.x() << "\t" << state.position.y() << "\t" << state.position.z() << "\t";
	(*m_fileStream) << (float) yaw << "\t";
	(*m_fileStream) << state.velocity << "\n";

}
void TrackBike::writeTrajectoryCSV()
{
	if (m_exportCSV)
	{
		for (CurrentBikeState state : m_trackedStates)
		{
			if (state.time > 0.0)
				writeTrajectoryLine(state);
		}
		m_fileStream->flush();
		m_file->close();
	}
}


TrackBike::~TrackBike()
{
	writeTrajectoryCSV();
}

void TrackBike::exportTaskStats(long time)
{
	QString fileDir("export" + QString(QDir::separator()) + "user" + QString::number(m_participantNumber));
	std::cout << "exporting task stats" << std::endl;
	bool success = true;
	if (!QDir(fileDir).exists())
		success = QDir().mkpath(fileDir);

	if (!success)
	{
		std::cout << "couldnt create export directory" << std::endl;
		return;
	}

	m_statfile = new QFile(fileDir + QString(QDir::separator()) + "task_stats.csv");
	QTextStream statStream(m_statfile);
	if (m_statfile->open(QFile::WriteOnly | QFile::Truncate))
	{
		statStream << "time" << "\t" << "wallCrashes" << "\t" << "wrongTurns" << "\n";
		statStream << time << "\t" << m_wallCrashes << "\t" << m_wrongTurns << "\n";
	}

	statStream.flush();
	m_statfile->close();
}

void TrackBike::recordCrash()
{
	m_wallCrashes++;
}

void TrackBike::recordWrongTurn()
{
	m_wrongTurns++;
}

