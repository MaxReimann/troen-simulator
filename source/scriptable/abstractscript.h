#pragma once

#include "qstring.h"
#include "scriptzeug\ScriptContext.h"

using namespace reflectionzeug;


namespace troen
{

	// base class for all scripts
	class	AbstractScript : public reflectionzeug::Object
	{
	public:
		AbstractScript(const std::string name) : reflectionzeug::Object(name)
		{
			addFunction("log", this, &AbstractScript::log);
		}

		virtual void evaluate(std::string content)
		{
			m_scriptContext.evaluate(content);
		}

		virtual void log(std::string message) {
			std::cout << "script log:   " << message << std::endl;
		}

		
		scriptzeug::ScriptContext m_scriptContext;
	};

}

	