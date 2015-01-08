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