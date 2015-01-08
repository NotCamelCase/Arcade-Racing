#include "Audio\Music.h"

#include "fmod.hpp"

#include "Audio\AudioManager.h"
#include "Utils.h"

Music::Music(const Ogre::String& resourceName)
	: Audio(resourceName)
{
}

Music::~Music()
{
}

void Music::play()
{
	if (!AudioManager::getSingleton().m_muted)
	{
		AudioManager::getSingleton().m_fmodSystem->playSound(m_audioSource, AudioManager::getSingleton().m_musicChannelGroup, false, &m_audioChannel);
		m_playing = true;
		m_audioChannel->isPlaying(&m_playing);
	}
}

void Music::pause()
{
	if (m_playing)
	{
		m_playing = false;
		m_audioChannel->setPaused(m_playing);
	}
}

void Music::stop()
{
	if (m_playing)
	{
		m_audioChannel->stop();
		m_playing = false;
	}
}