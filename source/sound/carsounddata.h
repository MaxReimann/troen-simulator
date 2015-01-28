// -*- Mode: c++ -*-
/***************************************************************************
    file                 : CarSoundData.h
    created              : Tue Apr 5 19:57:35 CEST 2005
    copyright            : (C) 2005 Christos Dimitrakakis
    email                : dimitrak@idiap.ch
    version              : $Id: CarSoundData.h 4965 2012-09-29 14:34:55Z pouillot $

 ***************************************************************************/

#pragma once

#include "../forwarddeclarations.h"
#include "btBulletDynamicsCommon.h"
#include "../model/carengine.h"

namespace troen {



	typedef struct QSoundChar_
	{
		float amplitude;
		float frequency;
		float lowpass;
	}QSoundChar;


	/// Manages the source sound of each individual car.
	class CarSoundData
	{


	public:

		CarSoundData();
		void setCarPosition(btVector3 p);
		void setCarSpeed(btVector3 u);
		void getCarPosition(btVector3 p) const;
		void getCarSpeed(btVector3 u) const;
		void setListenerPosition(btVector3 p);
		void update(CarEngine* car);

	protected:

		btVector3 listener_position;
		btVector3 position;
		btVector3 speed;
		void calculateEngineSound(CarEngine* car);
		void calculateBackfireSound(CarEngine* car);
		void calculateGearChangeSound(CarEngine* car);

	public:

		float attenuation; ///< global distance attenuation
		float base_frequency; ///< engine base frequency for ~ 6000 rpm
		float smooth_accel; ///< smoothed acceleration input
		float pre_axle; ///< axle related
		QSoundChar engine;
		QSoundChar axle;
		QSoundChar engine_backfire;
		int prev_gear;

		bool gear_changing;

	};

}
