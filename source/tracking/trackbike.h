#include "tracker.h"
#pragma once
#include "../forwarddeclarations.h"
#include "btBulletDynamicsCommon.h"
#include "qfile.h"
#include "qtextstream.h"




namespace troen
{
	namespace tracking {
		struct CurrentBikeState
		{
			btVector3 position;
			btQuaternion rotation;
			long double time;
			float velocity;
		};

		class TrackBike
		{

		public:
			TrackBike(std::shared_ptr<BikeController> controller, float frequency, int participantNumber, bool exportCSV);
			~TrackBike();
			void update(long double gameTime);
			void writeTrajectoryLine(CurrentBikeState& state);
			void writeTrajectoryCSV();
			void exportTaskStats(long time);
			void recordCrash();
			void recordWrongTurn();
		protected:

			std::vector<CurrentBikeState> m_trackedStates;
			std::shared_ptr<BikeController> m_controller;
			long double m_lastUpdate = NULL;
			float m_frequency;
			int m_participantNumber;
			bool m_exportCSV;
			QTextStream* m_fileStream;
			QFile* m_file;
			QFile* m_statfile;
			int m_wrongTurns;
			int m_wallCrashes;
		};
	} //end namespcae tracking
} // end namespace troen
