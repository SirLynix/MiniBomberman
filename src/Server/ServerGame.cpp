#include <Server/ServerGame.hpp>
#include <Shared/Constants.hpp>
#include <Shared/NetCode.hpp>
#include <Shared/PlayerInputs.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <fmt/core.h>
#include <stdexcept>

ServerGame::ServerGame() :
m_playerPool(64)
{
	if (!m_networkHost.Create(Nz::NetProtocol::Any, 14642, 64, 2))
		throw std::runtime_error("failed to create host");

	m_map = std::make_unique<ServerMap>();
}

void ServerGame::BroadcastPacket(Nz::UInt8 channelId, Nz::ENetPacketRef packet, ServerPlayer* except)
{
	for (ServerPlayer& player : m_playerPool)
	{
		if (&player != except)
			player.GetPeer()->Send(channelId, packet);
	}
}

ServerMap& ServerGame::GetMap()
{
	return *m_map;
}

void ServerGame::OnTick(bool /*lastTick*/)
{
	for (ServerPlayer& player : m_playerPool)
	{
		const PlayerInputs& lastInputs = player.GetLastInputs();

		constexpr float playerSpeed = 5.f;
		constexpr float elapsedTime = TickDuration / 1'000'000.f;

		Nz::Vector3f playerPos = player.GetPosition();
		Nz::Vector3f playerDir = Nz::Vector3f::Zero();

		if (lastInputs.moveDown)
			playerDir += Nz::Vector3f::Backward();

		if (lastInputs.moveUp)
			playerDir += Nz::Vector3f::Forward();

		if (lastInputs.moveLeft)
			playerDir += Nz::Vector3f::Left();

		if (lastInputs.moveRight)
			playerDir += Nz::Vector3f::Right();

		if (playerDir != Nz::Vector3f::Zero())
			playerPos += playerDir.GetNormal() * elapsedTime * playerSpeed;

		if (lastInputs.placeBomb)
			player.HandlePlaceBomb();

		player.UpdatePosition(playerPos);
	}

	NetCode::PlayerPositionsPacket playerPositions;
	playerPositions.playerPos.reserve(m_playerPool.size());
	for (ServerPlayer& player : m_playerPool)
	{
		auto& playerPos = playerPositions.playerPos.emplace_back();
		playerPos.playerIndex = Nz::SafeCast<Nz::UInt8>(player.GetIndex());
		playerPos.position = player.GetPosition();
	}

	BroadcastPacket(playerPositions);
}

bool ServerGame::OnUpdate(float /*elapsedTime*/)
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

				std::size_t playerIndex = it->second;

				fmt::print("Player #{} disconnected\n", playerIndex);

				m_playerPool.Free(playerIndex);
				m_peerIdToPlayerIndex.erase(it);

				NetCode::PlayerDisconnectedPacket disconnectedPacket;
				disconnectedPacket.playerIndex = Nz::SafeCast<Nz::UInt8>(playerIndex);

				BroadcastPacket(disconnectedPacket);
				break;
			}

			case Nz::ENetEventType::IncomingConnect:
			{
				std::size_t playerIndex;
				ServerPlayer* player = m_playerPool.Allocate(playerIndex, *this, event.peer, "Mingebag");
				player->UpdateIndex(playerIndex);

				m_peerIdToPlayerIndex[event.peer->GetPeerId()] = playerIndex;

				std::size_t cellX = std::rand() % ServerMap::Width;
				std::size_t cellY = std::rand() % ServerMap::Height;
				m_map->ClearCell(cellX, cellY);
				Nz::Vector3f cellCenter = m_map->GetCellCenter(cellX, cellY);
				player->UpdatePosition(cellCenter);

				fmt::print("A new player logged in (player #{})\n", playerIndex);

				NetCode::InitialDataPacket initialData;
				initialData.mapWidth = Nz::SafeCast<Nz::UInt8>(ServerMap::Width);
				initialData.mapHeight = Nz::SafeCast<Nz::UInt8>(ServerMap::Height);
				initialData.mapCells.reserve(ServerMap::Width * ServerMap::Height);
				for (std::size_t y = 0; y < ServerMap::Height; ++y)
				{
					for (std::size_t x = 0; x < ServerMap::Width; ++x)
						initialData.mapCells.push_back(m_map->GetCellContent(x, y));
				}

				initialData.playerIndex = Nz::SafeCast<Nz::UInt8>(playerIndex);
				for (const ServerPlayer& serverPlayer : m_playerPool)
				{
					auto& playerInfo = initialData.players.emplace_back();
					playerInfo.index = Nz::SafeCast<Nz::UInt8>(serverPlayer.GetIndex());
					playerInfo.name  = serverPlayer.GetName();
					playerInfo.position = serverPlayer.GetPosition();
				}

				player->SendPacket(initialData);

				NetCode::PlayerConnectedPacket connectedPacket;
				connectedPacket.playerInfo.index = Nz::SafeCast<Nz::UInt8>(playerIndex);
				connectedPacket.playerInfo.name = player->GetName();
				connectedPacket.playerInfo.position = player->GetPosition();

				BroadcastPacket(connectedPacket, player);
				break;
			}

			case Nz::ENetEventType::Receive:
			{
				std::size_t peerId = event.peer->GetPeerId();
				std::size_t playerIndex = Nz::Retrieve(m_peerIdToPlayerIndex, peerId);

				ServerPlayer* serverPlayer = m_playerPool.RetrieveFromIndex(playerIndex);
				
				Nz::NetPacket& netPacket = event.packet->data;

				Nz::UInt16 netCode;
				netPacket >> netCode;

				NetCode::Opcode opcode = Nz::SafeCast<NetCode::Opcode>(netCode);

				switch (opcode)
				{
					case NetCode::Opcode::C_PlayerInput:
					{
						NetCode::PlayerInputPacket inputPacket = NetCode::PlayerInputPacket::Unserialize(netPacket);

						serverPlayer->UpdateInputs(inputPacket.inputs);
						break;
					}

					default:
						fmt::print(stderr, "unhandled packet opcode {}\n", netCode);
				}
				break;
			}
		}
	}

	return true;
}

void ServerGame::OnUpsUpdate(unsigned int ups)
{
	fmt::print("{} ups\n", ups);
}
