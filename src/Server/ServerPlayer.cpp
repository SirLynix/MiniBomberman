
#include <Server/ServerPlayer.hpp>
#include <Server/ServerGame.hpp>
#include <iostream>

ServerPlayer::ServerPlayer(ServerGame& game, Nz::ENetPeer* peer) :
m_peer(peer),
m_game(game)
{
}

std::size_t ServerPlayer::GetIndex() const
{
	return m_index;
}

const PlayerInputs& ServerPlayer::GetLastInputs() const
{
	return m_lastInputs;
}

Nz::ENetPeer* ServerPlayer::GetPeer() const
{
	return m_peer;
}

const Nz::Vector2f& ServerPlayer::GetPosition() const
{
	return m_position;
}

void ServerPlayer::HandlePlaceBomb()
{
	auto [cellX, cellY] = m_game.GetMap().GetCell({ m_position.x, 0.f, m_position.y });

	std::cout << "Le joueur #" << m_index << " pose une bombe sur la cellule " << cellX << ", " << cellY << "!" << std::endl;

	Nz::Vector3f bombPos = m_game.GetMap().GetCellCenter(cellX, cellY);

	Nz::ENetPacketRef enetPacket = m_game.GetENetHost().AllocatePacket(Nz::ENetPacketFlag_Reliable);
	Nz::NetPacket& packet = enetPacket->data;

	Nz::UInt16 netCode = 5;
	packet << netCode;
	packet << bombPos;

	m_game.BroadcastPacket(enetPacket);
}

void ServerPlayer::UpdateInputs(const PlayerInputs& inputs)
{
	m_lastInputs = inputs;
}

void ServerPlayer::UpdatePosition(const Nz::Vector2f& position)
{
	m_position = position;
}

void ServerPlayer::UpdateIndex(std::size_t index)
{
	m_index = index;
}
