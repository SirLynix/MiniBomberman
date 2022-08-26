#include <Server/ServerGame.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <iostream>
#include <stdexcept>

ServerGame::ServerGame() :
m_playerPool(64)
{
	if (!m_networkHost.Create(Nz::NetProtocol::Any, 14642, 64))
		throw std::runtime_error("failed to create host");

	m_map = std::make_unique<ServerMap>();
}

bool ServerGame::OnUpdate(float elapsedTime)
{
	m_networkHost.Service(nullptr, 0);

	Nz::ENetEvent event;
	while (m_networkHost.CheckEvents(&event))
	{
		switch (event.type)
		{
			case Nz::ENetEventType::None:
			case Nz::ENetEventType::OutgoingConnect:
				break;

			case Nz::ENetEventType::Disconnect:
			{
				auto it = m_peerIdToPlayerIndex.find(event.peer->GetPeerId());
				assert(it != m_peerIdToPlayerIndex.end());

				std::cout << "Player #" << it->second << " disconnected" << std::endl;

				m_playerPool.Free(it->second);
				m_peerIdToPlayerIndex.erase(it);
				break;
			}

			case Nz::ENetEventType::IncomingConnect:
			{
				std::size_t playerIndex;
				ServerPlayer* player = m_playerPool.Allocate(playerIndex, event.peer);
				m_peerIdToPlayerIndex[event.peer->GetPeerId()] = playerIndex;

				std::cout << "A new player logged in (player #" << playerIndex << ")" << std::endl;

				Nz::NetPacket packet;

				Nz::UInt16 netCode = 1;
				packet << netCode;
				packet << Nz::SafeCast<Nz::UInt8>(ServerMap::Width) << Nz::SafeCast<Nz::UInt8>(ServerMap::Height);
				for (std::size_t y = 0; y < ServerMap::Height; ++y)
				{
					for (std::size_t x = 0; x < ServerMap::Width; ++x)
						packet << Nz::SafeCast<Nz::UInt8>(m_map->GetCell(x, y));
				}

				event.peer->Send(0, Nz::ENetPacketFlag_Reliable, std::move(packet));
				break;
			}

			case Nz::ENetEventType::Receive:
			{
				std::size_t peerId = event.peer->GetPeerId();
				std::size_t playerIndex = Nz::Retrieve(m_peerIdToPlayerIndex, peerId);
				break;
			}
		}
	}

	return true;
}

void ServerGame::OnUpsUpdate(unsigned int ups)
{
	std::cout << ups << " ups" << std::endl;
}
