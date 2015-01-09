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

#include <forward_list>

#include <rapidxml.hpp>

#include <OgreString.h>

#include "Utils.h"

namespace Ogre { class Log; }

class NBID;
class Game;
class Player;

class PlayerManager
{
public:
	~PlayerManager();

	friend class Game;

	Player* getLocalPlayer() const;
	Player* getPlayerByID(NBID* id) const;

	// Utility function for filling player name UI field when logging in
	const Ogre::String getLatestLocalPlayerName()
	{
		if (!m_localPlayersList.empty()) return m_localPlayersList.front();
		else return "";
	}

	bool savePlayerStatsLocally(Player* owner);

	void addPlayer(Player* player);
	void setLocalPlayer(Player* p);

	bool isLoginSuccessful(const Ogre::String& name, const Ogre::String& pass);

	bool isReadyForCountDown();

	static PlayerManager& getSingleton();

	const std::map<NBID*, Player*, Utils::ComparisonFunctor<NBID>>& getGamePlayers() const { return m_gamePlayers; }

private:
	PlayerManager();
	PlayerManager(const PlayerManager&);

	std::map<NBID*, Player*, Utils::ComparisonFunctor<NBID>> m_gamePlayers;
	Player* m_localPlayer;

	// List of local players created early and stored in profiles.list file
	std::forward_list<Ogre::String> m_localPlayersList;
	rapidxml::xml_document<>* m_profilesDoc;
	char* m_xmlContent;

	void saveNewPlayerToDisk(const Player& p);
	void saveNewPlayerToDisk(const Ogre::String& name, const Ogre::String& pass);

	Ogre::Log* m_logger;
};