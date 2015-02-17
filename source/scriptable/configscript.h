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
			m_game->m_deformationRendering->setPreset(val);
		}
		int BendedDeformationEnd() const { return BENDED_DEFORMATION_END; }
		void setBendedDeformationEnd(const int & val)
		{
			BENDED_DEFORMATION_END = val;
			m_game->m_deformationRendering->setDeformationStartEnd(BENDED_DEFORMATION_START, BENDED_DEFORMATION_END);
		}

		double BendedDeformationStart() const { return BENDED_DEFORMATION_END; }
		void setBendedDeformationStart(const double & val)
		{
			BENDED_DEFORMATION_START = val;
			m_game->m_deformationRendering->setDeformationStartEnd(BENDED_DEFORMATION_START, BENDED_DEFORMATION_END);
		}
		double SpeedToRealRatio() const { return SPEED_TOREAL_RATIO; }
		void setSpeedToRealRatio(const double & val){SPEED_TOREAL_RATIO = val;}



	private:


		bool *changesPending;
		TroenGame *m_game;
		bool awaiting_update;
		std::string pendingContent;
		ScriptWatcher m_scriptWatcher;
	};
}

	