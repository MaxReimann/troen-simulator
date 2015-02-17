// -*- Mode: c++ -*-
/***************************************************************************
    file                 : CarSoundData.cpp
    created              : Tue Apr 5 19:57:35 CEST 2005
    copyright            : (C) 2005 Christos Dimitrakakis
    email                : dimitrak@idiap.ch
    version              : $Id: CarSoundData.cpp 4965 2012-09-29 14:34:55Z pouillot $

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "carsounddata.h"
#include "../constants.h"

using namespace troen;


CarSoundData::CarSoundData()
{
    engine.amplitude = 0.0f;
    engine.frequency = 1.0f;
    engine.lowpass = 1.0f;
    smooth_accel = 0.0f;
    pre_axle = 0.0f;
    axle.amplitude = 0.0f;
    axle.frequency = 0.0f;
    engine_backfire.amplitude=0.0f;
    prev_gear = 0;
    gear_changing = false;
	base_frequency = 1.1f;

    btVector3 zeroes(0.0f, 0.0f, 0.0f);
    setCarPosition(zeroes);
    setCarSpeed(zeroes);
    setListenerPosition(zeroes);
    
    attenuation = 0.0f;
}


void CarSoundData::setCarPosition (btVector3 p)
{
	for (int i=0; i<3; i++) {
		position[i] = p[i];
	}
}

void CarSoundData::setCarSpeed (btVector3 u)
{
	for (int i=0; i<3; i++) {
		speed[i] = u[i];
	}
}

void CarSoundData::getCarPosition (btVector3 p) const
{
	for (int i=0; i<3; i++) {
		p[i] = position[i];
	}
}

void CarSoundData::getCarSpeed (btVector3 u) const
{
	for (int i=0; i<3; i++) {
		u[i] = speed[i];
	}
}

void CarSoundData::setListenerPosition (btVector3 p)
{
	for (int i=0; i<3; i++) {
		listener_position[i] = p[i];
	}
}

void CarSoundData::update (CarEngine* car)
{
    calculateEngineSound (car);
    calculateBackfireSound (car);
    calculateGearChangeSound (car);
}

/// Calculate characteristics of the engine sound.
void CarSoundData::calculateEngineSound (CarEngine* car)
{
	float mpitch = (base_frequency * car->getRPM() / 800.0 / SPEED_TOREAL_RATIO);
    engine.frequency = mpitch;
    engine.amplitude = 1.0f;
    //if (car->_state & RM_CAR_STATE_NO_SIMU) {
    //    engine.a = 0.0f;
    //    engine.lp = 1.0;
    //    return;
    //}
    //assert(car->index == eng_pri.id);

    float gear_ratio = car->Gears[car->getCurGear()];
    axle.amplitude = 0.2f*(tanh(100.0f*(fabs(pre_axle - mpitch))));
    axle.frequency = (pre_axle + mpitch)*0.05f*fabs(gear_ratio);
    pre_axle = (pre_axle + mpitch)*0.5f;

    smooth_accel = (smooth_accel*0.5 + 0.5*(car->getThrottle()*.99+0.01));

    // engine filter proportional to revcor2.
    // when accel = 0, lp \in [0.0, 0.25]
    // when accel = 1, lp \in [0.25, 1.0]
    // interpolate linearly for values in between.
    float rev_cor = car->getRPM()/car->MaxRPM;
    float rev_cor2 = rev_cor * rev_cor;
    engine.lowpass = (0.75f*rev_cor2 + 0.25f)*smooth_accel
        + (1.0f-smooth_accel)*0.25f*rev_cor2;

    // TODO: filter for exhaust and car body resonance?
                
}


/// Calculate the frequency and amplitude of a looped backfiring sound.
void CarSoundData::calculateBackfireSound (CarEngine* car)
{               

    engine_backfire.frequency = (car->getRPM() / 600.0);
    engine_backfire.amplitude *= (.9*.5+.5*exp(-engine_backfire.frequency));
}


void CarSoundData::calculateGearChangeSound (CarEngine* car) {
    if (car->getGear() != prev_gear) {
		prev_gear = car->getGear();
        gear_changing = true;
    } else {
        gear_changing = false;
    }

}

