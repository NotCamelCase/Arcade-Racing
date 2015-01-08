#include "Network\NetworkManager.h"

#include "LoadBalancing-cpp/inc/Client.h"

#include "Network\NetworkTypenames.h"
#include "Event\IEventListener.h"
#include "Player\PlayerManager.h"
#include "Network\GameClient.h"
#include "Event\EventManager.h"
#include "Player\RaceManager.h"
#include "Player\Player.h"
#include "InputMap.h"
#include "Utils.h"
#include "Game.h"

using namespace ExitGames;

const char* NetworkManager::PHOTON_APP_ID = "5e205299-19f4-437b-93fc-894c479686bc";
const char* NetworkManager::GAME_VERSION = "0.1";
const char* NetworkManager::GAME_NAME = "Race Sim";

// max number of players that a game session can have connected
#define MAX_NETWORKED_GAME_CLIENT 2

NetworkManager::NetworkManager()
	: m_loadBalancingClient(NULL), m_networkState(NetworkState::UNINITIALIZED)
{
}

NetworkManager::~NetworkManager()
{
	disconnect();
	SAFE_DELETE(m_loadBalancingClient);

	for (GameClient* client : m_gameClients)
	{
		SAFE_DELETE(client);
	}
	m_gameClients.clear();
}

NetworkManager& NetworkManager::getSingleton()
{
	static NetworkManager* onlyInstance = new NetworkManager();

	return *onlyInstance;
}

void NetworkManager::disconnect()
{
	if (m_networkState != NetworkState::DISCONNECTING)
	{
		if (m_loadBalancingClient)
		{
			m_networkState = NetworkState::DISCONNECTING;
			if (m_loadBalancingClient->getIsInGameRoom()) m_loadBalancingClient->opLeaveRoom();
			if (m_loadBalancingClient->getIsInLobby()) m_loadBalancingClient->opLeaveLobby();
			m_loadBalancingClient->disconnect();
			m_networkState = NetworkState::DISCONNECTED;
		}
	}

	for (GameClient* client : m_gameClients)
	{
		SAFE_DELETE(client);
	}
	m_gameClients.clear();

	// Default to Practice when left
	Game::setGameMode(GameMode::PRACTICE);
}

void NetworkManager::initLocalClient(const char* address)
{
	if (m_networkState == NetworkState::UNINITIALIZED)
	{
		Player* localPlayer = PlayerManager::getSingleton().getLocalPlayer();
		assert(localPlayer != NULL && "PlayerManager's local player couldn't be fetched!");

		// Concatenate name and password. e.g Name: "Tayfun" || Pass: "123" -> "Tayfun&&123"
		const std::string namePassConcat = localPlayer->getName() + "&&" + localPlayer->getPassword();

		m_loadBalancingClient = new LoadBalancing::Client(*this, PHOTON_APP_ID, GAME_VERSION, namePassConcat.c_str());
		assert(m_loadBalancingClient != NULL && "NetworkManager failed to create LoadBalancing::Client instance!");
		m_loadBalancingClient->setDebugOutputLevel(Common::DebugLevel::INFO);
		Common::Base::setListener(this);
		Common::Base::setDebugOutputLevel(Common::DebugLevel::INFO);

		// Add player's vehicle name as custom property
		m_loadBalancingClient->getLocalPlayer().addCustomProperty<Common::JString, Common::JString>("player_vehicle", Common::JString(localPlayer->getSelectedVehicleName().c_str()));
		// Also pass player's best timing on this track to be used by GameObjectManager when sorting vehicles at pole positions !!!
		//m_loadBalancingClient->getLocalPlayer().addCustomProperty<Common::JString, long long>("best_time_on_track", PlayerManager::getSingleton().getBestTimingOfPlayer(localPlayer));

		m_networkState = NetworkState::INITIALIZED;
	}
	else if (m_networkState == NetworkState::CONNECTION_FAILED || m_networkState == NetworkState::DISCONNECTED)
	{
		// Give it another go
		m_networkState = NetworkState::INITIALIZED;
	}
}

void NetworkManager::update(float delta)
{
	// Remove this f*cker
	if (!m_loadBalancingClient) return;

	m_loadBalancingClient->service();

	switch (m_networkState)
	{
	case NetworkState::UNINITIALIZED:
		LOG_ME("Waiting initialization...");
		break;
	case NetworkState::INITIALIZED:
		m_networkState = (m_loadBalancingClient->connect()) ? NetworkState::CONNECTING_TO_MASTER_SERVER : NetworkState::CONNECTION_FAILED;
		break;
	case NetworkState::CONNECTING_TO_MASTER_SERVER:
	case NetworkState::CONNECTING_TO_ROOM:
		LOG_ME("Connecting to Photon server...");
		break;
	case NetworkState::CONNECTED_TO_MASTER_SERVER:
		LOG_ME("Connected to Photon server");
		onLocalClientAuthentication();
		break;
	case NetworkState::CONNECTED_TO_ROOM:
		LOG_ME("Connected to game room");
		break;
	case NetworkState::CONNECTED_TO_GAME:
		LOG_ME("In-game network update");
		broadcastInGameData();
		break;
	case NetworkState::CONNECTION_FAILED:
		LOG_ME("Connection failed!");
		break;
	case NetworkState::DISCONNECTING:
		LOG_ME("Disconnecting from Photon server...");
		break;
	case NetworkState::DISCONNECTED:
		LOG_ME("Disconnected from Photon server!");
		break;
	default:
		LOG_ME("Default State");
		break;
	}
}

void NetworkManager::broadcastInGameData()
{
	// Send local input data to Photon
	Common::Hashtable localInput;
	localInput.put(NetworkTypenames::PARAMS_CLIENT_KEY_FLAGS, (nByte)InputMap::getKeyFlags());
	broadcastGameData(false, NetworkTypenames::EV_GAME_CLIENT_INPUT_UPDATE, localInput);
}

void NetworkManager::broadcastGameData(bool reliable, nByte eventType, const ExitGames::Common::Hashtable& data)
{
	m_loadBalancingClient->opRaiseEvent(reliable, data, eventType);
}

void NetworkManager::broadcastGameEvent(bool reliable, nByte eventType)
{
	m_loadBalancingClient->opRaiseEvent(reliable, NULL, eventType);
}

void NetworkManager::onLocalClientAuthentication()
{
	// Join game lobby if not autojoined
	if (!m_loadBalancingClient->getIsInLobby())
	{
		m_loadBalancingClient->opJoinLobby();
		m_networkState = NetworkState::CONNECTING_TO_MASTER_SERVER;
	}
	else
	{
		m_networkState = NetworkState::CONNECTING_TO_ROOM;
		EventManager::getSingleton().notify(EventType::NET_READY_IN_LOBBY);
	}
}

void NetworkManager::joinRandomRoom()
{
	if (m_networkState == NetworkState::CONNECTED_TO_GAME) return;

	//if (THERE_IS_NO_ROOM_YET) LET_PLAYERS_KNOW !!!

	Common::Hashtable roomProps;
	roomProps.put("track_name", RaceManager::getSingleton().getTrackData()->niceName.c_str());

	m_loadBalancingClient->opJoinRandomRoom(roomProps);
}

void NetworkManager::joinRoom(int index)
{
	if (m_networkState == NetworkState::CONNECTED_TO_GAME) return;
}

void NetworkManager::createRoom()
{
	if (m_networkState == NetworkState::CONNECTED_TO_GAME) return;

	Common::Hashtable roomProps;
	roomProps.put("track_name", RaceManager::getSingleton().getTrackData()->niceName.c_str());

	// Joining to be filtered by Photon based on track_name
	Common::JVector<Common::JString> roomFilter;
	roomFilter.addElement("track_name");

	Player* local = PlayerManager::getSingleton().getLocalPlayer();
	const std::string& playerName = local->getName();
	m_networkState = NetworkState::CONNECTING_TO_ROOM;
	if (!m_loadBalancingClient->opCreateRoom((playerName + "'s").c_str(), true, true, MAX_NETWORKED_GAME_CLIENT, roomProps, roomFilter))
		m_networkState = NetworkState::CONNECTION_FAILED;
}

const Common::JVector<LoadBalancing::Room*>& NetworkManager::fetchRoomList()
{
	assert(m_networkState != NetworkState::CONNECTION_FAILED && "Connection to Photon server has failed!");

	LOG_ME("Fetching room list...");

	return (m_loadBalancingClient->getRoomList());
}

GameClient* NetworkManager::getGameClientByPlayer(Player* const p) const
{
	for (GameClient* client : m_gameClients)
	{
		if (client->getPlayerRepresented() == p)
		{
			return client;
		}
	}

	return NULL;
}

GameClient* NetworkManager::getGameClientByID(NBID* const id) const
{
	GameClient* gameClient = NULL;
	for (GameClient* client : m_gameClients)
	{
		if (client->getPlayerRepresented()->getID() == id)
		{
			return client;
		}
	}

	return NULL;
}

void NetworkManager::debugReturn(Common::DebugLevel::DebugLevel debugLevel, const Common::JString& string)
{
}

void NetworkManager::connectionErrorReturn(int errorCode)
{
}

void NetworkManager::clientErrorReturn(int errorCode)
{
}

void NetworkManager::warningReturn(int warningCode)
{
}

void NetworkManager::serverErrorReturn(int errorCode)
{
}

void NetworkManager::joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const LoadBalancing::Player& player)
{
	LoadBalancing::MutableRoom& gameRoom = m_loadBalancingClient->getCurrentlyJoinedRoom();
	if (gameRoom.getPlayerCount() == MAX_NETWORKED_GAME_CLIENT)
	{
		const Common::JVector<LoadBalancing::Player*>& gamePlayers = gameRoom.getPlayers();
		for (size_t i = 0; i < gamePlayers.getSize(); i++)
		{
			LoadBalancing::Player* player = gamePlayers.getElementAt(i);
			if (m_loadBalancingClient->getLocalPlayer() == *player) // local player
			{
				// Add local player as GameClient's player when it's me connected to the room
				GameClient* localClient = new GameClient(player->getNumber(), PlayerManager::getSingleton().getLocalPlayer());
				localClient->setClientState(ClientState::CONNECTED);
				m_gameClients.push_back(localClient);
			}
			else
			{
				const Common::JString& concatName = player->getName();
				const int indexOfSeparator = concatName.indexOf("&&");
				const Common::JString playerName = concatName.substring(0, indexOfSeparator);
				const Common::JString playerPass = concatName.substring(indexOfSeparator + 2);

				Player* remotePlayer = new Player(playerName.UTF8Representation().cstr(), playerPass.UTF8Representation().cstr());
				GameClient* remoteClient = new GameClient(player->getNumber(), remotePlayer);
				const Common::Hashtable& playerProps = player->getCustomProperties();
				const Common::JString playerVehicleName = Common::ValueObject<Common::JString>(playerProps.getValue("player_vehicle")).getDataCopy(); // Fetch player's vehicle name
				remotePlayer->setSelectedVehicleName(playerVehicleName.UTF8Representation().cstr()); // Assign to remote player
				remoteClient->setClientState(ClientState::CONNECTED);
				m_gameClients.push_back(remoteClient);
			}
		}

		// Do we need to keep room asleep or what ?!
		//gameRoom.setIsVisible(false);
		//gameRoom.setIsOpen(false);

		m_networkState = NetworkState::CONNECTED_TO_GAME;
		EventManager::getSingleton().notify(EventType::NET_READY_TO_JOIN_GAME);
	}
}

void NetworkManager::disconnectEventAction(int playerNr)
{
	GameClient* remoteClient = getGameClientByPhotonID(playerNr);
	remoteClient->setClientState(ClientState::DISCONNECTED);
}

void NetworkManager::leaveRoomEventAction(int playerNr, bool isInactive)
{
}

void NetworkManager::customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContent)
{
	switch (eventCode)
	{
	case NetworkTypenames::EV_GAME_CLIENT_STATE_CHANGE:
		onGameClientStateChange(playerNr, eventContent);
		break;
	case NetworkTypenames::EV_GAME_CLIENT_INPUT_UPDATE:
		onGameClientInputReceive(playerNr, eventContent);
		break;
	case NetworkTypenames::EV_GAME_CLIENT_POST_LOADING:
		onGameClientPostInit(playerNr);
		break;
	default:
		break;
	}
}

void NetworkManager::onRoomPropertiesChange(const Common::Hashtable& props)
{
	printf("%s\n", props.toString());
}

void NetworkManager::onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes)
{
	printf("Player &d updated custom properties", playerNr);
}

void NetworkManager::connectReturn(int errorCode, const Common::JString& errorString)
{
	if (errorCode != LoadBalancing::ErrorCode::OK)
	{
		LOG_ME(errorString);

		EventManager::getSingleton().notify(EventType::NET_CONNECTION_FAILED);

		m_networkState = NetworkState::CONNECTION_FAILED;

		return;
	}

	m_networkState = NetworkState::CONNECTED_TO_MASTER_SERVER;
}

void NetworkManager::disconnectReturn()
{
	m_networkState = NetworkState::DISCONNECTED;
}

void NetworkManager::createRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode != LoadBalancing::ErrorCode::OK)
	{
		LOG_ME(errorString);

		EventManager::getSingleton().notify(EventType::NET_CONNECTION_FAILED);

		m_networkState = NetworkState::CONNECTION_FAILED;

		return;
	}

	m_networkState = NetworkState::CONNECTED_TO_ROOM;
	EventManager::getSingleton().notify(EventType::NET_WAITING_IN_ROOM);
}

void NetworkManager::joinRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode != LoadBalancing::ErrorCode::OK)
	{
		LOG_ME(errorString);

		EventManager::getSingleton().notify(EventType::NET_CONNECTION_FAILED);

		m_networkState = NetworkState::CONNECTION_FAILED;

		return;
	}

	m_networkState = NetworkState::CONNECTED_TO_ROOM;
	EventManager::getSingleton().notify(EventType::NET_WAITING_IN_ROOM);
}

void NetworkManager::joinRandomRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode != LoadBalancing::ErrorCode::OK)
	{
		LOG_ME(errorString);

		EventManager::getSingleton().notify(EventType::NET_CONNECTION_FAILED);

		m_networkState = NetworkState::CONNECTION_FAILED;

		return;
	}

	m_networkState = NetworkState::CONNECTED_TO_ROOM;
	EventManager::getSingleton().notify(EventType::NET_WAITING_IN_ROOM);
}

void NetworkManager::leaveRoomReturn(int errorCode, const Common::JString& errorString)
{
	if (errorCode != LoadBalancing::ErrorCode::OK)
	{
		LOG_ME(errorString);

		m_networkState = NetworkState::CONNECTION_FAILED;

		return;
	}

	m_networkState = NetworkState::CONNECTED_TO_MASTER_SERVER;
}

void NetworkManager::joinLobbyReturn()
{
	m_networkState = NetworkState::CONNECTED_TO_MASTER_SERVER;
}

void NetworkManager::leaveLobbyReturn()
{
}

void NetworkManager::onGameClientStateChange(int playerNumber, const Common::Object& eventContent)
{
	const Common::Hashtable& data = Common::ValueObject<Common::Hashtable>(eventContent).getDataCopy();
	const int newState = (int) data.getValue(NetworkTypenames::PARAMS_GAME_CLIENT_STATE);
	GameClient* client = getGameClientByPhotonID(playerNumber);
	client->setClientState(ClientState(newState));
}

void NetworkManager::onGameClientInputReceive(int playerNumber, const ExitGames::Common::Object& eventContent)
{
	GameClient* remoteClient = getGameClientByPhotonID(playerNumber);
	const unsigned int kf = static_cast<unsigned int> (Common::ValueObject<int>(eventContent).getDataCopy());
	remoteClient->setKeyFlags(kf);
}

void NetworkManager::onGameClientPostInit(int playerNumber)
{
	GameClient* client = getGameClientByPhotonID(playerNumber);
	client->setClientState(ClientState::READY_FOR_COUNTDOWN);

	LOG_ME("Client " + client->getPlayerRepresented()->getName() + " ready for countdown!");
}

bool NetworkManager::isReadyForCountDown()
{
	for (const GameClient* client : m_gameClients)
	{
		if (client->getClientState() != ClientState::READY_FOR_COUNTDOWN)
			return false;
	}

	return true;
}

GameClient* NetworkManager::getGameClientByPhotonID(int number) const
{
	for (GameClient* client : m_gameClients)
	{
		if (client->getPhotonID() == number)
			return client;
	}

	return NULL;
}