#include "AudioManager.h"
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "../globals.h"
#include "../constants.h"
#include "../model/carengine.h"
#include "fmod.hpp"
#include <fmod_errors.h>


using namespace troen::sound;

void FmodErrorCheck(FMOD_RESULT result)	// this is an error handling function
{						// for FMOD errors
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
}


float ChangeSemitone(float frequency, float variation) {
	static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
	return frequency * pow(semitone_ratio, variation);
}

float RandomBetween(float min, float max) {
	if(min == max) return min;
	float n = (float)rand()/(float)RAND_MAX;
	return min + n * (max - min);
}

AudioManager::AudioManager() : currentSong(0), engineChannel(0), fade(FADE_NONE) {
	// Initialize system
	FMOD::System_Create(&system);
	system->init(100, FMOD_INIT_NORMAL, 0);

	// Create channels groups for each category
	system->getMasterChannelGroup(&master);
	for(int i = 0; i < CATEGORY_COUNT; ++i) {
		system->createChannelGroup(0, &groups[i]);
		master->addGroup(groups[i]);
	}
	// Set up modes for each category
	modes[CATEGORY_SFX] = FMOD_DEFAULT;
	modes[CATEGORY_SONG] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL | FMOD_SOFTWARE;
	modes[CATEGORY_ENGINE] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL | FMOD_SOFTWARE;

	// Seed random number generator for SFXs
	srand((unsigned int) time(0));
	m_engineSoundData = std::make_shared<CarSoundData>();


	LoadSFX("data/sound/explosion.wav");
	LoadSFX("data/sound/gear_change1.wav");
	LoadEngineSound();

	auto result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_dplowpass);
	FmodErrorCheck(result);

}

AudioManager::~AudioManager() {
	// Release sounds in each category
	SoundMap::iterator iter;
	for(int i = 0; i < CATEGORY_COUNT; ++i) {
		for (iter = sounds[i].begin(); iter != sounds[i].end(); ++iter)
			iter->second->release();
		sounds[i].clear();
	}

	// Release system
	system->release();
}

void AudioManager::LoadSFX(const std::string& path) {
	Load(CATEGORY_SFX, path);
}

void AudioManager::LoadSong(const std::string& path) {
	Load(CATEGORY_SONG, path);
}

void AudioManager::LoadEngineSound()
{
	Load(CATEGORY_ENGINE, "data/sound/porsche_engine.wav");// engine - loop - 1 - normalized.wav");
}

void AudioManager::PlayEngineSound()
{
	// Search for a matching sound in the map
	SoundMap::iterator sound = sounds[CATEGORY_ENGINE].find("data/sound/porsche_engine.wav");
	if (sound == sounds[CATEGORY_ENGINE].end()) return;
	
	system->playSound(FMOD_CHANNEL_FREE, sound->second, true, &engineChannel);
	auto result = engineChannel->addDSP(m_dplowpass, 0);
	FmodErrorCheck(result);
	result = m_dplowpass->setParameter(FMOD_DSP_LOWPASS_CUTOFF, 0);
	FmodErrorCheck(result);




	engineChannel->setChannelGroup(groups[CATEGORY_ENGINE]);
	engineChannel->setVolume(0.9f);
	engineChannel->setPaused(false);

	//sound = sounds[CATEGORY_ENGINE].find("data/sound/axle.wav");
	//if (sound == sounds[CATEGORY_ENGINE].end()) return;
	//system->playSound(FMOD_CHANNEL_FREE, sound->second, true, &axleChannel);
	//axleChannel->setChannelGroup(groups[CATEGORY_ENGINE]);
	//axleChannel->setVolume(0.1f);
	//axleChannel->setPaused(false);
}

void AudioManager::Load(Category type, const std::string& path) {
	if (sounds[type].find(path) != sounds[type].end()) return;
	FMOD::Sound* sound;
	system->createSound(path.c_str(), modes[type], 0, &sound);
	sounds[type].insert(std::make_pair(path, sound));
}

void AudioManager::PlaySFX(const std::string& path, float minVolume, float maxVolume, float minPitch, float maxPitch)
{
	// Try to find sound effect and return if not found
	SoundMap::iterator sound = sounds[CATEGORY_SFX].find(path);
	if (sound == sounds[CATEGORY_SFX].end()) return;

	// Calculate random volume and pitch in selected range
	float volume = RandomBetween(minVolume, maxVolume);
	float pitch = RandomBetween(minPitch, maxPitch);

	// Play the sound effect with these initial values
	system->playSound(FMOD_CHANNEL_FREE, sound->second,	true, &m_channel);
	m_channel->setChannelGroup(groups[CATEGORY_SFX]);
	m_channel->setVolume(volume);
	float frequency;
	m_channel->getFrequency(&frequency);
	m_channel->setFrequency(ChangeSemitone(frequency, pitch));
	m_channel->setPaused(false);
}

void AudioManager::StopSFXs() {
	groups[CATEGORY_SFX]->stop();
}

void AudioManager::PlaySong(const std::string& path) {
	// Ignore if this song is already playing
	if(currentSongPath == path) return;

	// If a song is playing stop them and set this as the next song
	if(currentSong != 0) {
		StopSongs();
		nextSongPath = path;
		return;
	}

	// Search for a matching sound in the map
	SoundMap::iterator sound = sounds[CATEGORY_SONG].find(path);
	if (sound == sounds[CATEGORY_SONG].end()) return;

	// Start playing song with volume set to 0 and fade in
	currentSongPath = path;
	system->playSound(FMOD_CHANNEL_FREE, sound->second, true, &currentSong);
	currentSong->setChannelGroup(groups[CATEGORY_SONG]);
	currentSong->setVolume(0.0f);
	currentSong->setPaused(false);
	fade = FADE_IN;
}

void AudioManager::StopSongs() {
	if(currentSong != 0)
		fade = FADE_OUT;
	nextSongPath.clear();
}

void AudioManager::setMotorSpeed(troen::CarEngine * engine) {
	m_engineSoundData->update(engine);

	float freq = ENGINE_FREQUENCY_LOW + m_engineSoundData->engine.frequency * 6000;
	freq = clamp(freq, ENGINE_FREQUENCY_HIGH, freq);
	engineChannel->setFrequency(freq);// currentFrequency + (desiredFrequency - currentFrequency) / 100);

	//axleChannel->setLo

	// For the moment, simulate LP filter by tweaking the volume
	engineChannel->setVolume(m_engineSoundData->engine.amplitude);

	auto result = m_dplowpass->setParameter(FMOD_DSP_LOWPASS_CUTOFF, ENGINE_FREQUENCY_LOW  +m_engineSoundData->engine.lowpass);
	FmodErrorCheck(result);



}

float AudioManager::getTimeSinceLastBeat()
{
	return 1.0;
}


void AudioManager::auxiliaryCarSounds()
{
	if (m_engineSoundData->gear_changing)
	{
		PlaySFX("data/sound/gear_change1.wav", 0.05f, 0.1f, 1.0, 1.0);
	}
}


void AudioManager::Update(float elapsed)
{

	auxiliaryCarSounds();


	const float fadeTime = 1.0f; // in seconds

	if(currentSong != 0 && fade == FADE_IN) {
		float volume;
		currentSong->getVolume(&volume);
		float nextVolume = volume + elapsed / fadeTime;
		if(nextVolume >= 1.0f) {
			currentSong->setVolume(1.0f);
			fade = FADE_NONE;
		} else {
			currentSong->setVolume(nextVolume);
		}
	} else if(currentSong != 0 && fade == FADE_OUT) {
		float volume;
		currentSong->getVolume(&volume);
		float nextVolume = volume - elapsed / fadeTime;
		if(nextVolume <= 0.0f) {
			currentSong->stop();
			currentSong = 0;
			currentSongPath.clear();
			fade = FADE_NONE;
		} else {
			currentSong->setVolume(nextVolume);
		}
	} else if(currentSong == 0 && !nextSongPath.empty()) {
		PlaySong(nextSongPath);
		nextSongPath.clear();
	}
	system->update();
}

void AudioManager::SetMasterVolume(float volume) {
	master->setVolume(volume);
}
float AudioManager::GetMasterVolume(){
	float volume;
	master->getVolume(&volume);
	return volume;
}
void AudioManager::SetSFXsVolume(float volume) {
	groups[CATEGORY_SFX]->setVolume(volume);
}
void AudioManager::SetSongsVolume(float volume) {
	groups[CATEGORY_SONG]->setVolume(volume);
}