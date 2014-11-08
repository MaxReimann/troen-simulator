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
			void writeLine(CurrentBikeState& state);
			void writeCSV();
		protected:

			std::vector<CurrentBikeState> m_trackedStates;
			std::shared_ptr<BikeController> m_controller;
			long double m_lastUpdate = NULL;
			float m_frequency;
			int m_participantNumber;
			bool m_exportCSV;
			QTextStream* m_fileStream;
			QFile* m_file;
		};
	} //end namespcae tracking
} // end namespace troen