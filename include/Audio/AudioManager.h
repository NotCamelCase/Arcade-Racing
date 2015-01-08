#pragma once

#include <map>

#include <OgreString.h>

#include "Utils.h"

class NBID;
class Audio;
class SoundEffect;
class Music;

namespace Ogre { class Log; }
namespace FMOD { class System; class ChannelGroup; }

enum AudioType
{
	SOUND_2D,
	SOUND_2D_LOOPING,
	SOUND_3D,
	SOUND_3D_LOOPING
};

class AudioManager
{
public:
	~AudioManager();

	friend class SoundEffect;
	friend class Music;

	SoundEffect* createSoundEffect(const Ogre::String& name, AudioType type = AudioType::SOUND_2D);
	Music* createMusic(const Ogre::String& name, AudioType type = AudioType::SOUND_2D);

	void release(Audio* res);

	void muteAudio() { m_muted = true; }
	void unmuteAudio() { m_muted = false; }
	bool isMuted() const { return m_muted; }

	void update();

	static AudioManager& getSingleton();

private:
	AudioManager();

	FMOD::System* m_fmodSystem;
	FMOD::ChannelGroup* m_soundEffectChannelGroup;
	FMOD::ChannelGroup* m_musicChannelGroup;

	std::map<NBID*, Audio*, Utils::ComparisonFunctor<NBID>> m_loadedAudio;

	bool m_muted;

	Ogre::Log* m_logger;

	unsigned int getAudioMode(AudioType type) const;

	static const Ogre::String AUDIO_PATH;

	static AudioManager* getSingletonPtr();
};