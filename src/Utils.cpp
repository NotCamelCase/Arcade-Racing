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