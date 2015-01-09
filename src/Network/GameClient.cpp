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

#include "Network\GameClient.h"

#include "Player\Player.h"
#include "Player\PlayerManager.h"

#include "Utils.h"

GameClient::GameClient(int photonID, const char* playerName, const char* playerPass)
	: m_photonPlayerNumber(photonID), m_playerRepresented(NULL), m_clientState(ClientState::DISCONNECTED)
{
	assert(playerName != NULL || playerPass != NULL && "Corrupted remote player data!");

	m_playerRepresented = new Player(playerName, playerPass);
	PlayerManager::getSingleton().addPlayer(m_playerRepresented);
}

GameClient::GameClient(int photonID, Player* p)
	: m_photonPlayerNumber(photonID), m_playerRepresented(p), m_clientState(ClientState::DISCONNECTED)
{
	assert(p != NULL && "Corrupted remote player data!");

	PlayerManager::getSingleton().addPlayer(m_playerRepresented);
}

GameClient::~GameClient()
{
}

__forceinline bool GameClient::isRemotePlayer() const
{
	return !(m_playerRepresented->isLocal());
}
