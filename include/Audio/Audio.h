/*
The MIT License (MIT)

Copyright (c) 2015 Tayfun Kayhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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