#pragma once
//troen
#include "../forwarddeclarations.h"
#include "abstractmodel.h"
#include <btBulletDynamicsCommon.h>

#include "levelmodel.h"

namespace troen
{

	class CityModel : public LevelModel
	{
	public:
		CityModel(const LevelController* levelController, std::string levelName);
		void reload(std::string levelName);
		btScalar getLevelSize();

	private:
		void initSpecifics();

	};

}