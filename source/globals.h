#pragma once
#ifndef EXTERN
#define EXTERN extern
#endif

#include <reflectionzeug/Object.h>
#include <scriptzeug/ScriptContext.h>

namespace troen
{
	EXTERN long double g_gameTime;
	EXTERN long double g_gameLoopTime;
	EXTERN long double g_timeSinceLastUpdate;
	EXTERN bool g_gameRunning;
	EXTERN scriptzeug::ScriptContext* g_scripting;
}

