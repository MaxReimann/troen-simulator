#pragma once

// OSG
#include "../forwarddeclarations.h"
#include <btBulletDynamicsCommon.h>
#include "qstring.h"
#include "scriptwatcher.h"
#include "scriptzeug\ScriptContext.h"
#include "../constants.h"

#include "BendedViews/src/SplineDeformationRendering.h"

using namespace reflectionzeug;


namespace troen
{
	// Sets constants in game, the constants should be defined in constants.cpp
	// Can only set non-const variables
	// Script is executed for first time before anything is initialized, so check for nullptrs !
	class ConfigScript : public AbstractScript
	{
	public:
		ConfigScript(TroenGame *game);

		void executePending();

		virtual void evaluate(std::string content);

		int BendedViewsActivated() const { return BENDED_VIEWS_ACTIVATED; }
		void setBendedViewsActivated(const int & val) { BENDED_VIEWS_ACTIVATED = val; }
		int BendedViewsDeActivated() const { return BENDED_VIEWS_DEACTIVATED; }
		void setBendedViewsDeActivated(const int & val) { BENDED_VIEWS_DEACTIVATED = val; }
		int TimeToActivateBendedViews() const { return TIME_TO_ACTIVATE_BENDED_VIEWS; }
		void setTimeToActivateBendedViews(const int & val) { TIME_TO_ACTIVATE_BENDED_VIEWS = val; }
		int BendedStylingPreset() const { return BENDED_STYLING_PRESET; }
		void setBendedStylingPreset(const int & val) 
		{ 
			BENDED_STYLING_PRESET = val; 
			if (m_game->m_deformationRendering != nullptr)
				m_game->m_deformationRendering->setPreset(val);
		}
		int BendedDeformationEnd() const { return BENDED_DEFORMATION_END; }
		void setBendedDeformationEnd(const int & val)
		{
			BENDED_DEFORMATION_END = val;
			if (m_game->m_deformationRendering != nullptr){
				m_game->m_deformationRendering->setDeformationStartEnd(BENDED_DEFORMATION_START, BENDED_DEFORMATION_END);
				m_game->m_deformationRendering->reloadShaders();
			}
		}

		double BendedDeformationStart() const { return BENDED_DEFORMATION_END; }
		void setBendedDeformationStart(const double & val)
		{
			BENDED_DEFORMATION_START = val;
			if (m_game->m_deformationRendering != nullptr){
				m_game->m_deformationRendering->setDeformationStartEnd(BENDED_DEFORMATION_START, BENDED_DEFORMATION_END);
				m_game->m_deformationRendering->reloadShaders();
			}
		}
		double RespawnDuration() const { return RESPAWN_DURATION; }
		void setRespawnDuration(const double & val){RESPAWN_DURATION = val;}
		double GameStartCountDownDuration() const { return GAME_START_COUNTDOWN_DURATION; }
		void setGameStartCountDownDuration(const double & val){GAME_START_COUNTDOWN_DURATION = val;}
		double RandomNumberDuration() const { return RANDOM_NUMBER_DURATION; }
		void setRandomNumberDuration(const double & val){RANDOM_NUMBER_DURATION = val;}
		double RandomNumberInterval() const { return RANDOM_NUMBER_INTERVAL; }
		void setRandomNumberInterval(const double & val){RANDOM_NUMBER_INTERVAL = val;}
		double FovyInitial() const { return FOVY_INITIAL; }
		void setFovyInitial(const double & val){FOVY_INITIAL = val;}
		double FovyAdditionMax() const { return FOVY_ADDITION_MAX; }
		void setFovyAdditionMax(const double & val){FOVY_ADDITION_MAX = val;}
		double FovyInitialNavi() const { return FOVY_INITIAL_NAVI; }
		void setFovyInitialNavi(const double & val){ FOVY_INITIAL_NAVI = val; }
		double EngineFrequencyLow() const { return ENGINE_FREQUENCY_LOW; }
		void setEngineFrequencyLow(const double & val){ENGINE_FREQUENCY_LOW = val;}
		double AudioPitchFactor() const { return AUDIO_PITCH_FACTOR; }
		void setAduioPitchFactor(const double & val){AUDIO_PITCH_FACTOR = val;}
		double SpeedToRealRatio() const { return SPEED_TOREAL_RATIO; }
		void setSpeedToRealRatio(const double & val){ SPEED_TOREAL_RATIO = val; }
		double BikeSteeringIncrement() const { return BIKE_STEERING_INCREMENT; }
		void setBikeSteeringIncrement(const double & val) { BIKE_STEERING_INCREMENT = val; }
		double SteeringClamp() const { return BIKE_STEERINGCLAMP; }
		void setSteeringClamp(const double & val) { BIKE_STEERINGCLAMP = val; }
		double CameraPositionOffsetX() const { return CAMERA_POSITION_OFFSET_X; }
		void setCameraPositionOffsetX(const double & val) { CAMERA_POSITION_OFFSET_X = val; updateCamera(); }
		double CameraPositionOffsetY() const { return CAMERA_POSITION_OFFSET_Y; }
		void setCameraPositionOffsetY(const double & val) { CAMERA_POSITION_OFFSET_Y = val; updateCamera(); }
		double CameraPositionOffsetZ() const { return CAMERA_POSITION_OFFSET_Z; }
		void setCameraPositionOffsetZ(const double & val) { CAMERA_POSITION_OFFSET_Z = val; updateCamera(); }
		double CameraEyePositionX() const { return CAMERA_EYE_POSITION_X; }
		void setCameraEyePositionX(const double & val) { CAMERA_EYE_POSITION_X = val; updateCamera(); }
		double CameraEyePositionY() const { return CAMERA_EYE_POSITION_Y; }
		void setCameraEyePositionY(const double & val) { CAMERA_EYE_POSITION_Y = val; updateCamera(); }
		double CameraEyePositionZ() const { return CAMERA_EYE_POSITION_Z; }
		void setCameraEyePositionZ(const double & val) { CAMERA_EYE_POSITION_Z = val; updateCamera(); }
		double CameraLookatPositionX() const { return CAMERA_LOOKAT_POSITION_X; }
		void setCameraLookatPositionX(const double & val) { CAMERA_LOOKAT_POSITION_X = val; updateCamera(); }
		double CameraLookatPositionY() const { return CAMERA_LOOKAT_POSITION_Y; }
		void setCameraLookatPositionY(const double & val) { CAMERA_LOOKAT_POSITION_Y = val; updateCamera(); }
		double CameraLookatPositionZ() const { return CAMERA_LOOKAT_POSITION_Z; }
		void setCameraLookatPositionZ(const double & val) { CAMERA_LOOKAT_POSITION_Z = val; updateCamera(); }

		double DampingForce() const { return DAMPING_FORCE; }
		void setDampingForce(const double & val) { DAMPING_FORCE = val; }
		double CameraNaviEyePositionZ() const { return CAMERA_NAVI_EYE_POSITION_Z; }
		void setCameraNaviEyePositionZ(const double & val) { CAMERA_NAVI_EYE_POSITION_Z = val; }
		double CameraNaviPositionZ() const { return CAMERA_NAVI_CENTER_POSITION_Z; }
		void setCameraNaviPositionZ(const double & val) { CAMERA_NAVI_CENTER_POSITION_Z = val; }
		int defaultMainWindowWidth() const { return DEFAULT_MAINWINDOW_WIDTH; }
		void setdefaultMainWindowWidth(const int & val) { DEFAULT_MAINWINDOW_WIDTH = val; }
		int defaultMainWindowHeight() const { return DEFAULT_MAINWINDOW_HEIGHT; }
		void setdefaultMainWindowHeight(const int & val) { DEFAULT_MAINWINDOW_HEIGHT = val; }
		double windowRatioFullscreen() const { return WINDOW_RATIO_FULLSCREEN; }
		void setwindowRatioFullscreen(const double & val) { WINDOW_RATIO_FULLSCREEN = val; }
		int useCulling() const { return USE_CULLING; }
		void setuseCulling(const int & val) { USE_CULLING = val; }

		;

		void updateCamera();
	private:


		bool *changesPending;
		TroenGame *m_game;
		bool awaiting_update;
		std::string pendingContent;
		ScriptWatcher m_scriptWatcher;
	};
}

	