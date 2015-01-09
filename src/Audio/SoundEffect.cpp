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

#include "Audio\SoundEffect.h"

#include "fmod.hpp"

#include "Audio\AudioManager.h"
#include "Utils.h"

SoundEffect::SoundEffect(const Ogre::String& resourceName)
	: Audio(resourceName)
{
}

SoundEffect::~SoundEffect()
{
}

void SoundEffect::play()
{
	if (!AudioManager::getSingleton().m_muted)
	{
		AudioManager::getSingleton().m_fmodSystem->playSound(m_audioSource, AudioManager::getSingleton().m_soundEffectChannelGroup, false, &m_audioChannel);
		m_playing = true;
		m_audioChannel->isPlaying(&m_playing);      
	}
}

void SoundEffect::pause()
{
	if (m_playing)
	{
		m_playing = false;
		m_audioChannel->setPaused(m_playing);
	}
}

void SoundEffect::stop()
{
	if (m_playing)
	{
		m_audioChannel->stop();
		m_playing = false;
	}
}