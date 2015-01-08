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
