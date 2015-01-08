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