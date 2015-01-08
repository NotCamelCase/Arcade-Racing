#pragma once

#include <OgreString.h>

#include "Audio.h"

/*
* Used for big audio files such as main or gameplay background
* Audio is streamed by FMOD as it plays without getting uploaded beforehand
*/
class Music : public Audio
{
public:
	Music(const Ogre::String& resourceName);
	~Music();

	virtual void play();
	virtual void pause();
	virtual void stop();
};