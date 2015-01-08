#include "Utils.h"

#include <OgreStringConverter.h>

#include "Player\RaceManager.h"

Utils::Utils()
{
}

Utils::~Utils()
{
}

bool Utils::doesIncludeStr(const Ogre::String& source, const Ogre::String& check, bool lower)
{
	return (Ogre::StringUtil::match(source, check, lower) || Ogre::StringUtil::match(check, source, lower));
}

Ogre::String Utils::stringifyStatsTiming(PlayerStats* stats)
{
	static const Ogre::String zeroGap = "0";
	const unsigned int& splitSec = stats->splitSec;
	Ogre::String splitSecStr = Ogre::StringConverter::toString(splitSec);
	if (splitSec < 10) splitSecStr = zeroGap + splitSecStr;
	const unsigned int& sec = stats->sec;
	Ogre::String secStr = Ogre::StringConverter::toString(sec);
	if (sec < 10) secStr = zeroGap + secStr;
	const unsigned int& min = stats->min;
	Ogre::String minStr = Ogre::StringConverter::toString(min);
	if (min < 10) minStr = zeroGap + minStr;

	return (minStr + ":" + secStr + "." + splitSecStr);
}