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

#include <rapidxml.hpp>

namespace Ogre { class Log; }

/*
* GOD is a basic data-oriented way to apply
* fundamental game object properties as mapped key-value pairs.
*
* GODs are loaded via @GODLoader from .god file
*/
class GOD
{
public:
	friend class GODLoader;

	std::map<Ogre::String, Ogre::String>& getParameters() { return m_params; }

private:
	GOD() {}
	~GOD() { m_params.clear(); }
	
	std::map<Ogre::String, Ogre::String> m_params;
};

/*
* Loads and keeps the GOD defined in .god file
*/
class GODLoader
{
public:
	~GODLoader();

	GOD* getGameObjectDefinitionByTag(const Ogre::String& tag);
	Ogre::String getGameObjectDefinitionValue(const Ogre::String& tag, const Ogre::String& key) const;

	static GODLoader& getSingleton();

private:
	GODLoader();

	Ogre::Log* m_logger;

	std::map<Ogre::String, GOD*> m_loadedDefinitions;
	bool loadAllDefinitions();

	rapidxml::xml_document<>* m_xmlDoc;
	char* m_xmlContent;
};