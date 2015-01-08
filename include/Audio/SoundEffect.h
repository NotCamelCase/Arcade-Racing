#pragma once

#include <OgreString.h>

#include "Audio.h"

/*
* Used for small audio files such as click or vehicle crash sound. 
* Loaded completely before playing
*/
class SoundEffect : public Audio
{
public:
	SoundEffect(const Ogre::String& resourceName);
	~SoundEffect();

	virtual void play();
	virtual void pause();
	virtual void stop();
};