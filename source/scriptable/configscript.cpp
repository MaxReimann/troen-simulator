#pragma once

// OSG
#include "../forwarddeclarations.h"
#include <btBulletDynamicsCommon.h>
#include "qstring.h"
#include "scriptzeug\ScriptContext.h"
#include "../constants.h"
#include "../troengame.h"
#include "scriptwatcher.h"


#include "../BendedViews/src/SplineDeformationRendering.h"

#include "configscript.h"

using namespace troen;


ConfigScript::ConfigScript(TroenGame *game) : AbstractScript("configScript"), m_game(game)
{

	addProperty<int>("BENDED_VIEWS_ACTIVATED", *this, &ConfigScript::BendedViewsActivated, &ConfigScript::setBendedViewsActivated);
	addProperty<int>("BENDED_VIEWS_DEACTIVATED", *this, &ConfigScript::BendedViewsDeActivated, &ConfigScript::setBendedViewsDeActivated);
	addProperty<int>("TIME_TO_ACTIVATE_BENDED_VIEWS", *this, &ConfigScript::TimeToActivateBendedViews, &ConfigScript::setTimeToActivateBendedViews);
	addProperty<int>("BENDED_STYLING_PRESET", *this, &ConfigScript::BendedStylingPreset, &ConfigScript::setBendedStylingPreset);
	addProperty<int>("BENDED_DEFORMATION_END", *this, &ConfigScript::BendedDeformationEnd, &ConfigScript::setBendedDeformationEnd);
	addProperty<double>("BENDED_DEFORMATION_START", *this, &ConfigScript::BendedDeformationStart, &ConfigScript::setBendedDeformationStart);
	addProperty<double>("SPEED_TOREAL_RATIO", *this, &ConfigScript::SpeedToRealRatio, &ConfigScript::setSpeedToRealRatio);
	addProperty<double>("RESPAWN_DURATION", *this, &ConfigScript::RespawnDuration, &ConfigScript::setRespawnDuration);
	addProperty<double>("GAME_START_COUNTDOWN_DURATION", *this, &ConfigScript::GameStartCountDownDuration, &ConfigScript::setGameStartCountDownDuration);
	addProperty<double>("RANDOM_NUMBER_DURATION", *this, &ConfigScript::RandomNumberDuration, &ConfigScript::setRandomNumberDuration);
	addProperty<double>("RANDOM_NUMBER_INTERVAL", *this, &ConfigScript::RandomNumberInterval, &ConfigScript::setRandomNumberInterval);
	addProperty<double>("FOVY_INITIAL", *this, &ConfigScript::FovyInitial, &ConfigScript::setFovyInitial);
	addProperty<double>("FOVY_INITIAL_NAVI", *this, &ConfigScript::FovyInitialNavi, &ConfigScript::setFovyInitialNavi);
	addProperty<double>("FOVY_ADDITION_MAX", *this, &ConfigScript::FovyAdditionMax, &ConfigScript::setFovyAdditionMax);
	addProperty<double>("ENGINE_FREQUENCY_LOW", *this, &ConfigScript::EngineFrequencyLow, &ConfigScript::setEngineFrequencyLow);
	addProperty<double>("AUDIO_PITCH_FACTOR", *this, &ConfigScript::AudioPitchFactor, &ConfigScript::setAduioPitchFactor);
	addProperty<double>("BIKE_STEERING_INCREMENT", *this, &ConfigScript::BikeSteeringIncrement, &ConfigScript::setBikeSteeringIncrement);
	addProperty<double>("BIKE_STEERINGCLAMP", *this, &ConfigScript::SteeringClamp, &ConfigScript::setSteeringClamp);
	addProperty<double>("CAMERA_POSITION_OFFSET_X", *this, &ConfigScript::CameraPositionOffsetX, &ConfigScript::setCameraPositionOffsetX);
	addProperty<double>("CAMERA_POSITION_OFFSET_Y", *this, &ConfigScript::CameraPositionOffsetY, &ConfigScript::setCameraPositionOffsetY);
	addProperty<double>("CAMERA_POSITION_OFFSET_Z", *this, &ConfigScript::CameraPositionOffsetZ, &ConfigScript::setCameraPositionOffsetZ);
	addProperty<double>("CAMERA_EYE_POSITION_X", *this, &ConfigScript::CameraEyePositionX, &ConfigScript::setCameraEyePositionX);
	addProperty<double>("CAMERA_EYE_POSITION_Y", *this, &ConfigScript::CameraEyePositionY, &ConfigScript::setCameraEyePositionY);
	addProperty<double>("CAMERA_EYE_POSITION_Z", *this, &ConfigScript::CameraEyePositionZ, &ConfigScript::setCameraEyePositionZ);
	addProperty<double>("DAMPING_FORCE", *this, &ConfigScript::DampingForce, &ConfigScript::setDampingForce);
	addProperty<double>("CAMERA_NAVI_EYE_POSITION_Z", *this, &ConfigScript::CameraNaviEyePositionZ, &ConfigScript::setCameraNaviEyePositionZ);
	addProperty<double>("CAMERA_NAVI_CENTER_POSITION_Z", *this, &ConfigScript::CameraNaviPositionZ, &ConfigScript::setCameraNaviPositionZ);



	

	changesPending = new bool;
	*changesPending = false;

	m_scriptContext->registerObject(this);
	m_scriptWatcher.watchAndLoad("scripts/gameconstants.js", this, changesPending);

}


void ConfigScript::executePending()
{
	if (awaiting_update)
		m_scriptContext->evaluate(pendingContent);
	awaiting_update = false;
}

void ConfigScript::evaluate(std::string content)
{
	awaiting_update = true;
	pendingContent = content;
		//"try { if (typeof configure !== 'undefined') { configure(); } else { configScript.log('cant set values in script; check for errors!'); } } catch(ex) { ex } "
}

	