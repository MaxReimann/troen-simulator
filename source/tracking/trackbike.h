#include "tracker.h"
#pragma once
#include "../forwarddeclarations.h"
#include "btBulletDynamicsCommon.h"





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
			TrackBike(std::shared_ptr<BikeController> controller, float frequency);
			void update(long double gameTime);


		protected:

			std::vector<CurrentBikeState> m_trackedStates;
			std::shared_ptr<BikeController> m_controller;
			long double m_lastUpdate = NULL;
			float m_frequency;


		};
	} //end namespcae tracking
} // end namespace troen