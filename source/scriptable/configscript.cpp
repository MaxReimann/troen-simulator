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

	