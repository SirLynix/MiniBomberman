#include <Server/ServerGame.hpp>
#include <Shared/Constants.hpp>
#include <Shared/PlayerInputs.hpp>
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

void ServerGame::BroadcastPacket(Nz::ENetPacketRef packet)
{
	for (ServerPlayer& player : m_playerPool)
		player.GetPeer()->Send(0, packet);
}

Nz::ENetHost& ServerGame::GetENetHost()
{
	return m_networkHost;
}

ServerMap& ServerGame::GetMap()
{
	return *m_map;
}

void ServerGame::OnTick(bool lastTick)
{
	for (ServerPlayer& player : m_playerPool)
	{
		const PlayerInputs& lastInputs = player.GetLastInputs();

		constexpr float playerSpeed = 5.f;
		constexpr float elapsedTime = TickDuration / 1'000'000.f;

		Nz::Vector2f playerPos = player.GetPosition();
		Nz::Vector2f playerDir = Nz::Vector2f::Zero();

		if (lastInputs.moveDown)
			playerDir += Nz::Vector2f(0.f, 1.f);

		if (lastInputs.moveUp)
			playerDir += Nz::Vector2f(0.f, -1.f);

		if (lastInputs.moveLeft)
			playerDir += Nz::Vector2f(1.f, 0.f);

		if (lastInputs.moveRight)
			playerDir += Nz::Vector2f(-1.f, 0.f);

		if (playerDir != Nz::Vector2f::Zero())
			playerPos += playerDir.GetNormal() * elapsedTime * playerSpeed;

		if (lastInputs.placeBomb)		
			player.HandlePlaceBomb();

		player.UpdatePosition(playerPos);
	}


	Nz::ENetPacketRef enetPacket = m_networkHost.AllocatePacket(Nz::ENetPacketFlag_Reliable);
	Nz::NetPacket& packet = enetPacket->data;

	Nz::UInt16 netCode = 3;
	packet << netCode;

	Nz::UInt8 playerCount = Nz::SafeCast<Nz::UInt8>(m_playerPool.size());
	packet << playerCount;

	for (ServerPlayer& player : m_playerPool)
	{
		Nz::UInt8 playerIndex = Nz::SafeCast<Nz::UInt8>(player.GetIndex());
		packet << playerIndex;

		Nz::Vector2f playerPos = player.GetPosition();
		packet << playerPos;
	}

	BroadcastPacket(enetPacket);
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
				ServerPlayer* player = m_playerPool.Allocate(playerIndex, *this, event.peer);
				player->UpdateIndex(playerIndex);

				m_peerIdToPlayerIndex[event.peer->GetPeerId()] = playerIndex;

				std::size_t cellX = std::rand() % ServerMap::Width;
				std::size_t cellY = std::rand() % ServerMap::Height;
				m_map->ClearCell(cellX, cellY);
				Nz::Vector3f cellCenter = m_map->GetCellCenter(cellX, cellY);
				player->UpdatePosition({ cellCenter.x, cellCenter.z });

				std::cout << "A new player logged in (player #" << playerIndex << ")" << std::endl;

				Nz::NetPacket packet;

				Nz::UInt16 netCode = 1;
				packet << netCode;
				packet << Nz::SafeCast<Nz::UInt8>(ServerMap::Width) << Nz::SafeCast<Nz::UInt8>(ServerMap::Height);
				for (std::size_t y = 0; y < ServerMap::Height; ++y)
				{
					for (std::size_t x = 0; x < ServerMap::Width; ++x)
						packet << Nz::SafeCast<Nz::UInt8>(m_map->GetCellContent(x, y));
				}

				event.peer->Send(0, Nz::ENetPacketFlag_Reliable, std::move(packet));
				break;
			}

			case Nz::ENetEventType::Receive:
			{
				std::size_t peerId = event.peer->GetPeerId();
				std::size_t playerIndex = Nz::Retrieve(m_peerIdToPlayerIndex, peerId);

				ServerPlayer* serverPlayer = m_playerPool.RetrieveFromIndex(playerIndex);
				
				Nz::NetPacket& packet = event.packet->data;

				Nz::UInt16 netCode;
				packet >> netCode;

				switch (netCode)
				{
					case 2:
					{
						// player inputs
						PlayerInputs inputs;
						packet >> inputs.moveDown >> inputs.moveLeft >> inputs.moveRight >> inputs.moveUp >> inputs.placeBomb;

						serverPlayer->UpdateInputs(inputs);
						break;
					}
				}
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
