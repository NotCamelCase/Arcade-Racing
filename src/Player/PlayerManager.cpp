#include "Player\PlayerManager.h"

#include <OgreLog.h>
#include <OgreDataStream.h>
#include <OgreStringConverter.h>
#include <OgreResourceGroupManager.h>

#include <rapidxml_print.hpp>

#include "Game.h"
#include "NBID.h"
#include "Utils.h"
#include "Player\Player.h"
#include "Player\RaceManager.h"
#include "Network\GameClient.h"
#include "Network\NetworkManager.h"

using namespace rapidxml;

PlayerManager* Ogre::Singleton<PlayerManager>::msSingleton = NULL;

PlayerManager::PlayerManager()
	: m_logger(NULL), m_localPlayer(NULL), m_profilesDoc(NULL), m_xmlContent(NULL)
{
	m_logger = Game::getSingleton().getLog();

	Ogre::DataStreamPtr profilesContent = Ogre::ResourceGroupManager::getSingleton().openResource("users.list", "General");
	m_profilesDoc = new xml_document<>();
	const Ogre::String& profilesContentStr = profilesContent->getAsString();
	if (profilesContentStr.empty())	assert(false && "User profiles failed to be fetched!");
	m_xmlContent = strdup(profilesContentStr.c_str());
	m_profilesDoc->parse<0>(m_xmlContent);

	xml_node<>* profileNode = m_profilesDoc->first_node()->first_node();
	for (; profileNode; profileNode = profileNode->next_sibling())
	{
		const char* playerName = profileNode->first_attribute("name")->value();
		m_localPlayersList.push_front(playerName);
	}

	// FIXME: DELETE WHEN COMPLETED!
	/*setLocalPlayer(new Player("Tayfun", "123"));
	m_localPlayer->setSelectedVehicleName("carrera");
	RaceManager::getSingleton().createTrackData("forest1");*/
	// FIXME: DELETE WHEN COMPLETED!
}

PlayerManager::~PlayerManager()
{
	// Write new player names to disk in profiles.list
	std::ofstream saveStream("media/track/users.list");
	std::string cont;
	rapidxml::print(std::back_inserter(cont), *m_profilesDoc);
	saveStream << cont;
	saveStream.close();

	m_profilesDoc->clear();
	SAFE_DELETE(m_xmlContent);
	SAFE_DELETE(m_profilesDoc);

	for (auto& elm : m_gamePlayers)
	{
		SAFE_DELETE(elm.second);
	}
}

PlayerManager& PlayerManager::getSingleton()
{
	static PlayerManager* onlyInstance = new PlayerManager();

	return *onlyInstance;
}

Player* PlayerManager::getLocalPlayer() const
{
	return m_localPlayer;
}

bool PlayerManager::isLoginSuccessful(const Ogre::String& name, const Ogre::String& pass)
{
	bool retval = false;

	if (name == getLatestLocalPlayerName()) // Latest local player logs in
	{
		xml_node<>* latestUser = m_profilesDoc->first_node()->last_node("user");
		const Ogre::String pw = latestUser->first_attribute("password")->value();
		if (pw == pass)
		{
			setLocalPlayer(new Player(name, pass));
			retval = true;
		}
		else { // Wrong password!
			retval = false;
		}
	}
	else { // New player registers and log ins
		setLocalPlayer(new Player(name, pass));
		retval = true;
	}

	return retval;
}

void PlayerManager::setLocalPlayer(Player* p)
{
	assert(p || !p->getName().empty() && "Player fields missing!");

	m_localPlayer = p;
	m_localPlayer->setLoggedIn(true);
	addPlayer(m_localPlayer);

	for (auto& name : m_localPlayersList)
	{
		if (name == p->getName())
			return;
	}

	// If player's name is not already in profiles.list
	// insert it to xml doc so it'll be written to disk before exitting
	m_localPlayersList.push_front(m_localPlayer->getName());
	saveNewPlayerToDisk(*m_localPlayer);
}

void PlayerManager::saveNewPlayerToDisk(const Player& p)
{
	saveNewPlayerToDisk(p.getName(), p.getPassword());
}

void PlayerManager::saveNewPlayerToDisk(const Ogre::String& name, const Ogre::String& pass)
{
	xml_node<>* newLocal = m_profilesDoc->allocate_node(node_type::node_element, "user");
	xml_attribute<>* nameAttr = m_profilesDoc->allocate_attribute("name");
	nameAttr->value(strdup(name.c_str()));
	newLocal->insert_attribute(NULL, nameAttr);
	xml_attribute<>* passAttr = m_profilesDoc->allocate_attribute("password");
	passAttr->value(strdup(pass.c_str()));
	newLocal->insert_attribute(NULL, passAttr);
	m_profilesDoc->first_node()->append_node(newLocal);
}

bool PlayerManager::savePlayerStatsLocally(Player* owner)
{
	// Don't save multiplayer game stats !
	if (Game::getGameMode() != GameMode::PRACTICE) return false;

	assert(owner != NULL || owner->getPlayerStats() != NULL && "Corrupted player-stats data!");

	bool retVal = false;

	xml_node<>* usersNode = m_profilesDoc->first_node()->first_node();
	for (; usersNode; usersNode = usersNode->next_sibling())
	{
		const char* userName = usersNode->first_attribute("name")->value();
		const char* userPass = usersNode->first_attribute("password")->value();

		if (strcmp(userName, owner->getName().c_str()) == 0 && strcmp(userPass, owner->getPassword().c_str()) == 0)
		{
			// Correct user node found

			xml_node<>* recordNode = NULL;
			recordNode = m_profilesDoc->allocate_node(node_type::node_element, "stats");
			assert(recordNode != NULL && "rapidxml node allocation failed!");

			xml_attribute<>* recordAttr = m_profilesDoc->allocate_attribute("track_name");
			recordAttr->value(strdup(RaceManager::getSingleton().getTrackData()->objName.c_str()));
			recordNode->append_attribute(recordAttr);
			recordAttr = NULL;

			recordAttr = m_profilesDoc->allocate_attribute("timing");
			recordAttr->value(strdup(Utils::stringifyStatsTiming(owner->getPlayerStats()).c_str()));
			recordNode->append_attribute(recordAttr);
			recordAttr = NULL;

			recordAttr = m_profilesDoc->allocate_attribute("vehicle");
			recordAttr->value(strdup(owner->getSelectedVehicleName().c_str()));
			recordNode->append_attribute(recordAttr);

			usersNode->append_node(recordNode);
			m_logger->logMessage(owner->getName() + " stats saved locally successfully");
			recordAttr = NULL;

			retVal = true;
		}
	}

	return retVal;
}

Player* PlayerManager::getPlayerByID(NBID* id) const
{
	const auto it = m_gamePlayers.find(id);
	if (it != m_gamePlayers.end())
	{
		return it->second;
	}

	return NULL;
}

void PlayerManager::addPlayer(Player* p)
{
	if (p && !p->getName().empty())
	{
		if (m_gamePlayers.find(p->getID()) != m_gamePlayers.end()) return; // Already added

		m_gamePlayers[p->getID()] = p;

		m_logger->logMessage("Network Player " + p->getName() + " added");
	}
}

bool PlayerManager::isReadyForCountDown()
{
	for (const auto& elm : m_gamePlayers)
	{
		if (elm.second->getPlayerState() != PlayerState::READY_FOR_COUNTDOWN)
		{
			return false;
		}
	}

	return true;
}