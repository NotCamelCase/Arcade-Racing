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