#pragma once

#include <vector>

#include "LoadBalancing-cpp\inc\Listener.h"
#include "Common-cpp\inc\Hashtable.h"

class GameClient;
class Player;
class NBID;

namespace ExitGames { namespace LoadBalancing { class Client; class Room; } }

enum class NetworkState
{
	UNINITIALIZED,
	INITIALIZED,
	CONNECTING_TO_MASTER_SERVER,
	CONNECTED_TO_MASTER_SERVER,
	CONNECTING_TO_ROOM,
	CONNECTED_TO_ROOM,
	CONNECTED_TO_GAME, // In-game state of network & clients, update accordingly
	DISCONNECTING,
	DISCONNECTED,
	CONNECTION_FAILED
};

/*
* Manages connection and data exchange with Photon LoadBalancing client and game players
*/
class NetworkManager : public ExitGames::LoadBalancing::Listener
{
public:
	~NetworkManager();

	static NetworkManager& getSingleton();

	/* Initialize the connection of local player to Photon Master Server. Uses the Photon's EU Server as default */
	void initLocalClient(const char* masterServerAddress = "app-eu.exitgamescloud.com");

	/* Keep events/ops/connection alive and fresh */
	void update(float delta);

	/* Broadcast the event to others in game */
	void broadcastGameEvent(bool reliable, nByte eventType);
	
	/* Broadcast pre-defined data and event to others in game */
	void broadcastGameData(bool reliable, nByte eventType, const ExitGames::Common::Hashtable& data);

	void setNetworkState(NetworkState s) { m_networkState = s; }
	NetworkState getNetworkState() const { return m_networkState; }

	/* Join a room randomly based on race track selected */
	void joinRandomRoom();

	/* Join room selected off room list */
	void joinRoom(int index);

	/* Create a game room with property of selected track */
	void createRoom();

	/* Disconnect from Photon Server */
	void disconnect();

	/* Query GameClients' readiness (local post-init state) */
	bool isReadyForCountDown();

	const std::vector<GameClient*>& getGameClients() { return m_gameClients; }

	GameClient* getGameClientByPlayer(Player* const p) const;
	GameClient* getGameClientByID(NBID* const id) const;
	GameClient* getGameClientByPhotonID(int number) const;

	const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*>& fetchRoomList();

protected:
	void onLocalClientAuthentication();

	void onGameClientInputReceive(int playerNumber, const ExitGames::Common::Object& eventContent);
	void onGameClientStateChange(int playerNumber, const ExitGames::Common::Object& eventContent);
	void onGameClientPostInit(int playerNumber);

private:
	NetworkManager();

	static const char* PHOTON_APP_ID;
	static const char* GAME_VERSION;
	static const char* GAME_NAME;

	void broadcastInGameData();

	void debugReturn(ExitGames::Common::DebugLevel::DebugLevel debugLevel, const ExitGames::Common::JString& string);

	void connectionErrorReturn(int errorCode);
	void clientErrorReturn(int errorCode);
	void warningReturn(int warningCode);
	void serverErrorReturn(int errorCode);

	void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	void leaveRoomEventAction(int playerNr, bool isInactive);
	void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
	void disconnectEventAction(int playerNr);

	void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
	void disconnectReturn(void);
	void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
	void joinLobbyReturn(void);
	void leaveLobbyReturn(void);

	void onRoomPropertiesChange(const ExitGames::Common::Hashtable&);
	void onPlayerPropertiesChange(int playerNr, const ExitGames::Common::Hashtable& changes);

	ExitGames::LoadBalancing::Client* m_loadBalancingClient;

	std::vector<GameClient*> m_gameClients;

	NetworkState m_networkState;
};