#include "Audio\Audio.h"

#include "fmod.hpp"

#include "Utils.h"
#include "NBID.h"

Audio::Audio(const Ogre::String& name)
	: m_resourceName(name), m_audioSource(NULL), m_audioChannel(NULL),
	m_id(NULL), m_playing(false)
{
	m_id = new NBID(m_resourceName);
}

Audio::~Audio()
{
	if (m_playing)
		m_audioChannel->stop();
	m_audioSource->release();
	// SAFE_RELEASE(m_audioChannel);

	SAFE_DELETE(m_id);
}