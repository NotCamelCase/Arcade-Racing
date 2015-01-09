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

#include <OgreString.h>

namespace Ogre { class Log; class Timer; }

class NBID;
struct PlayerStats;

/* Declares NBID* varname and getter function */
#define ID_DECL(varname) \
	private:\
		NBID* varname;\
	public:\
		NBID* getID() const { return varname;}

enum class PlayerState
{
	INIT, READY_FOR_COUNTDOWN,
	RACING, LEFT_TRACK, FINISHED_TRACK
};

class Player
{
public:
	Player(const Ogre::String& playerName, const Ogre::String& password);
	~Player();

	const Ogre::String& getSelectedVehicleName() const { return m_selectedVehicleName; }
	void setSelectedVehicleName(const Ogre::String& name) { m_selectedVehicleName = name; }

	const Ogre::String& getName() const { return m_playerName; }
	const Ogre::String& getPassword() const { return m_playerPassword; } // O_o

	PlayerStats* getPlayerStats() const { return m_stats; }

	PlayerState getPlayerState() const { return m_playerState; }
	void setPlayerState(PlayerState state) { m_playerState = state; }

	/* Non-local, network players return False */
	bool isLocal() const;

	ID_DECL(m_uuid);

	bool operator<(Player* const other) const
	{
		return (m_uuid < other->getID());
	}

	bool operator<(const Player& other) const
	{
		return (m_uuid < other.getID());
	}

	void setLoggedIn(bool v) { m_loggedIn = v; }
	/* True if player logged in with PlayerLoginScreen successfully. Logging is required per game run */
	bool isLoggedIn() const { return m_loggedIn; }

private:
	Player(const Player& other);

	bool m_loggedIn;

	const Ogre::String m_playerName;
	const Ogre::String m_playerPassword;

	Ogre::String m_selectedTrackName;
	Ogre::String m_selectedVehicleName;

	PlayerStats* m_stats;
	PlayerState m_playerState;

	Ogre::Log* m_logger;
};