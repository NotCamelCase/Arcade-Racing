#pragma once

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
};