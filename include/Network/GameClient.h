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

#include <OgreVector3.h>

class Player;

/* NetworkManager already manages internal connection states, it's enough to know if GameClient's connection is alive */
enum class ClientState
{
	CONNECTED, // Connected to the Photon server
	READY_FOR_COUNTDOWN, // Completed local loading and initializations
	DISCONNECTED // Lost connection
};

/*
Provides basic bookkeeping for game to use for networked parts without getting into Photon
*/
class GameClient
{
public:
	GameClient(int photonID, const char* playerName, const char* playerPass);
	GameClient(int photonID, Player* p);
	~GameClient();

	void setPlayerRepresented(Player* player) { m_playerRepresented = player; }
	Player* getPlayerRepresented() const { return m_playerRepresented; }

	unsigned int getKeyFlags() const { return m_keyFlags; }
	void setKeyFlags(unsigned int kf) { m_keyFlags = kf; }

	const Ogre::Vector3& getClientPosition() { return m_clientPos; }
	const Ogre::Vector3& getClientDirection() { return m_clientDir; }

	void setClientPosition(const Ogre::Vector3& pos) { m_clientPos = pos; }
	void setClientDirection(const Ogre::Vector3& dir) { m_clientDir = dir; }

	bool isRemotePlayer() const;

	bool operator<(GameClient* other) { return m_playerRepresented < other->getPlayerRepresented(); }

	void setClientState(ClientState cs) { m_clientState = cs; }
	ClientState getClientState() const { return m_clientState; }

	const int getPhotonID() const { return m_photonPlayerNumber; }

private:
	Player* m_playerRepresented;

	ClientState m_clientState;

	// To be used by GameObjectManager when spawning vehicles when there is no player stats to compare with others
	const int m_photonPlayerNumber;

	// Input data to be exchanged
	unsigned int m_keyFlags;

	Ogre::Vector3 m_clientPos;
	Ogre::Vector3 m_clientDir;
};