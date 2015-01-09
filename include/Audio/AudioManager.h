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