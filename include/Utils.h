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
#include <vector>

#include <OgreString.h>

#include <OgreRenderQueue.h>

#define SAFE_DELETE(x) { if (x) { delete x; x = NULL; } }
#define SAFE_DELETE_ARRAY(x) { if (x) delete[] x; x = NULL;}

#define BIT_ME(x) (1 << x)

#if _DEBUG
#define LOG_ME(msg) { printf("%s\n", msg); }
#else
#define LOG_ME(msg) //printf("");
#endif

struct PlayerStats;

class Utils
{
public:
	static bool doesIncludeStr(const Ogre::String& source, const Ogre::String& check, bool lower = false);

	static const Ogre::uint32 RV_Hud = 2;

	static Ogre::String stringifyStatsTiming(PlayerStats* stats);

	static const Ogre::uint8 RQG_Hud1 = Ogre::RENDER_QUEUE_OVERLAY - 5;
	static const Ogre::uint8 RQG_Hud2 = Ogre::RENDER_QUEUE_OVERLAY - 2;
	static const Ogre::uint8 RQG_Hud3 = Ogre::RENDER_QUEUE_OVERLAY - 1;

	/* Creates a Comparison Functor for STL containers having Key* objects that have operator< overloaded */
	template<class T>
	struct ComparisonFunctor
	{
		bool operator()(T* const lhs, T* const rhs) const
		{
			return (*lhs < *rhs);
		}
	};

private:
	Utils();
	~Utils();
};