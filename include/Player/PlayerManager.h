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