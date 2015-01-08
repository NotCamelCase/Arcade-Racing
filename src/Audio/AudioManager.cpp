#include "Audio\AudioManager.h"

#include <OgreLog.h>
#include <OgreException.h>

#include "fmod.hpp"
#include "fmod_errors.h"

#include "NBID.h"
#include "Game.h"
#include "Audio\Audio.h"
#include "Audio\Music.h"
#include "Audio\SoundEffect.h"

using namespace Ogre;
using namespace FMOD;

const Ogre::String AudioManager::AUDIO_PATH = "media/audio/";

#define FMOD_ERROR_CHECK(res) { if (res != FMOD_OK) { OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "FMOD failed to execute!", "AudioManager"); } }

AudioManager::AudioManager()
	: m_logger(NULL), m_fmodSystem(NULL), m_soundEffectChannelGroup(NULL), m_musicChannelGroup(NULL),
	m_muted(false)
{
	FMOD_RESULT res = System_Create(&m_fmodSystem);
	FMOD_ERROR_CHECK(res);
	res = m_fmodSystem->init(64, FMOD_INIT_NORMAL, 0);
	FMOD_ERROR_CHECK(res);
	res = m_fmodSystem->createChannelGroup("SoundEffectChannelGroup", &m_soundEffectChannelGroup);
	FMOD_ERROR_CHECK(res);
	res = m_fmodSystem->createChannelGroup("MusicChannelGroup", &m_musicChannelGroup);
	FMOD_ERROR_CHECK(res);

	m_logger = Game::getSingleton().getLog();
	m_logger->logMessage("AudioManager initialized successfully");
}

/* Can be deleted by getting only class instance through &AudioMananger::getSingleton() */
AudioManager::~AudioManager()
{
	for (auto& elm : m_loadedAudio)
	{
		SAFE_DELETE(elm.second);
	}
	m_loadedAudio.clear();
	m_logger->logMessage("Loaded Audios released");

	m_soundEffectChannelGroup->release();
	m_musicChannelGroup->release();
	m_fmodSystem->close();
	m_fmodSystem->release();

	m_logger->logMessage("AudioManager destroyed");
}

AudioManager& AudioManager::getSingleton()
{
	// C++11 guarentees that's thread-safety
	static AudioManager* oneAndOnlyInstance = new AudioManager();

	return *oneAndOnlyInstance;
}

void AudioManager::update()
{
	if (!m_muted)
		m_fmodSystem->update();
}

SoundEffect* AudioManager::createSoundEffect(const Ogre::String& name, AudioType type)
{
	const Ogre::String sen = AudioManager::AUDIO_PATH + name;
	NBID checkVal(sen);
	const auto it = m_loadedAudio.find(&checkVal);
	if (it == m_loadedAudio.end())
	{
		Sound* source = NULL;
		unsigned int mode = getAudioMode(type);
		FMOD_RESULT res = m_fmodSystem->createSound(sen.c_str(), mode, 0, &source);
		FMOD_ERROR_CHECK(res);
		SoundEffect* effect = new SoundEffect(sen);
		effect->setAudioSource(source);
		m_loadedAudio[effect->getID()] = effect;

		return effect;
	}
	else {
		return (static_cast<SoundEffect*> (it->second));
	}
}

Music* AudioManager::createMusic(const Ogre::String& name, AudioType type)
{
	const Ogre::String sen = AudioManager::AUDIO_PATH + name;
	NBID checkVal(sen);
	const auto it = m_loadedAudio.find(&checkVal);
	if (it == m_loadedAudio.end())
	{
		Sound* source = NULL;
		unsigned int mode = getAudioMode(type);
		FMOD_RESULT res = m_fmodSystem->createStream(sen.c_str(), mode, 0, &source);
		FMOD_ERROR_CHECK(res);
		Music* music = new Music(sen);
		music->setAudioSource(source);
		m_loadedAudio[music->getID()] = music;

		return music;
	}
	else {
		return (static_cast<Music*> (it->second));
	}
}

unsigned int AudioManager::getAudioMode(AudioType type) const
{
	unsigned int mode;
	switch (type)
	{
	case SOUND_2D:
	default:
		mode = FMOD_2D | FMOD_LOOP_OFF;
		break;
	case SOUND_2D_LOOPING:
		mode = FMOD_2D | FMOD_LOOP_NORMAL;
		break;
	case SOUND_3D:
		mode = FMOD_3D | FMOD_LOOP_OFF;
		break;
	case SOUND_3D_LOOPING:
		mode = FMOD_3D | FMOD_LOOP_NORMAL;
		break;
	}

	return mode;
}

void AudioManager::release(Audio* res)
{
	const auto it = m_loadedAudio.find(res->getID());
	if (it != m_loadedAudio.end())
	{
		SAFE_DELETE(it->second);
		m_loadedAudio.erase(it);
	}
}