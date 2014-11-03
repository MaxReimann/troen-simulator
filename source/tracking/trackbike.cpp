#include "trackbike.h"
#include "..\controller\bikecontroller.h"
#include "..\model\bikemodel.h"

using namespace troen::tracking;

troen::tracking::TrackBike::TrackBike(std::shared_ptr<BikeController> controller, float frequency) :
m_controller(controller), m_frequency(frequency)
{
	m_trackedStates = std::vector<CurrentBikeState>();
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

		//std::cout << bikeState.position.getX() << std::endl;
	}
}