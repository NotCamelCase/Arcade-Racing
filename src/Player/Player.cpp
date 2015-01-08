#include "Player\Player.h"

#include <OgreLog.h>

#include "Player\PlayerManager.h"
#include "Player\RaceManager.h"
#include "Utils.h"
#include "Game.h"
#include "NBID.h"

Player::Player(const Ogre::String& playerName, const Ogre::String& password)
	: m_playerName(playerName), m_playerPassword(password), m_logger(NULL), m_stats(NULL),
	m_playerState(PlayerState::INIT), m_uuid(new NBID(playerName)), m_loggedIn(false)
{
	m_logger = Game::getSingleton().getLog();

	m_stats = RaceManager::getSingleton().createPlayerStats(this);

	m_logger->logMessage("Player " + playerName + " created");
}

Player::~Player()
{
	RaceManager::getSingleton().destroyPlayerStats(this);

	SAFE_DELETE(m_uuid);

	m_logger->logMessage("Player " + m_playerName + " destroyed");
}

bool Player::isLocal() const
{
	return (this == PlayerManager::getSingleton().getLocalPlayer());
}