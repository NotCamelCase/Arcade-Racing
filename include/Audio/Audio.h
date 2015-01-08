#pragma once

#include <OgreString.h>

#include "fmod.hpp"

class NBID;

/* Declares NBID* varname and getter function */
#define ID_DECL(varname) \
	private:\
		NBID* varname;\
	public:\
		NBID* getID() const { return varname;}

/*
* Abstract class that encapsulates low-level FMOD API's audio source and channels.
* To be derived by @SoundEffect and @Music that use the exposed general audio functionality.
*/
class Audio
{
public:
	Audio(const Ogre::String& name);
	virtual ~Audio();

	/* Play FMOD sound source */
	virtual void play() = 0;

	/* Pause FMOD sound source */
	virtual void pause() = 0;

	/* Stop FMOD sound source */
	virtual void stop() = 0;

	bool isPlaying() const { return m_playing; }

	const Ogre::String& getResourceName() const { return m_resourceName; }

	void setAudioSource(FMOD::Sound* sound) { m_audioSource = sound; }
	FMOD::Sound* getAudioSource() const { return m_audioSource; }

	void setVolume(float v) { m_audioChannel->setVolume(v); }
	float getVolume() const { float volume; m_audioChannel->getVolume(&volume); return volume; }

	void setPitch(float v) { m_audioChannel->setPitch(v); }
	float getPitch() const { float pitch; m_audioChannel->getPitch(&pitch); return pitch; }

	ID_DECL(m_id);

protected:
	FMOD::Sound* m_audioSource;
	FMOD::Channel* m_audioChannel;

	const Ogre::String m_resourceName;

	bool m_playing;
};