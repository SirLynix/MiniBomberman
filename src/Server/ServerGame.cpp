#include <Server/ServerGame.hpp>
#include <Shared/Constants.hpp>
#include <Shared/NetCode.hpp>
#include <Shared/PlayerInputs.hpp>
#include <Server/Components/BombComponent.hpp>
#include <Server/Systems/BombSystem.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <fmt/core.h>
#include <array>
#include <stdexcept>

ServerGame::ServerGame() :
m_playerPool(64),
m_state(State::Waiting)
{
	if (!m_networkHost.Create(Nz::NetProtocol::Any, 14642, 64, 2))
		throw std::runtime_error("failed to create host");

	m_map = std::make_unique<ServerMap>();
	m_map->OnCellCleared.Connect([this](std::size_t x, std::size_t y)
	{
		m_mapClearCells.cellIds.push_back(Nz::SafeCast<Nz::UInt16>(y * Map::Width + x));
	});

	GetSystemGraph().AddSystem<BombSystem>(*m_map);
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
	switch (m_state)
	{
		case Game::State::Waiting:
		{
			if (m_playerPool.size() >= MinPlayerCount)
			{
				SwitchToState(Game::State::Starting);
				return;
			}

			break;
		}

		case Game::State::Starting:
		{
			if (m_stateClock.GetMilliseconds() >= StartingTime)
			{
				SwitchToState(Game::State::Playing);
				return;
			}

			BroadcastMapUpdate();
			BroadcastPlayerPosition();
			break;
		}

		case Game::State::Playing:
		{
			if (m_stateClock.GetMilliseconds() >= MaxMatchDuration)
			{
				SwitchToState(Game::State::Finished);
				return;
			}

			HandlePlayerInputs();
			BroadcastMapUpdate();
			BroadcastPlayerPosition();
			break;
		}

		case Game::State::Finished:
		{
			if (m_stateClock.GetMilliseconds() >= RestartTime)
			{
				SwitchToState(Game::State::Waiting);
				return;
			}

			HandlePlayerInputs();
			BroadcastMapUpdate();
			BroadcastPlayerPosition();
			break;
		}
	}
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

				if (m_state == Game::State::Starting && m_playerPool.size() < MinPlayerCount)
				{
					fmt::print("Not enough players to start game, resetting\n");
					SwitchToState(Game::State::Waiting);
				}
				break;
			}

			case Nz::ENetEventType::IncomingConnect:
			{
				std::size_t playerIndex;
				ServerPlayer* player = m_playerPool.Allocate(playerIndex, *this, event.peer, "Mingebag");
				player->UpdateIndex(playerIndex);

				m_peerIdToPlayerIndex[event.peer->GetPeerId()] = playerIndex;

				fmt::print("A new player logged in (player #{})\n", playerIndex);

				NetCode::InitialDataPacket initialData;
				initialData.gameState = m_state;
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

void ServerGame::BroadcastMapUpdate()
{
	if (!m_mapClearCells.cellIds.empty())
	{
		BroadcastPacket(m_mapClearCells);
		m_mapClearCells.cellIds.clear();
	}
}

void ServerGame::BroadcastPlayerPosition()
{
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

void ServerGame::HandlePlayerBomb(ServerPlayer& player)
{
	auto [cellX, cellY] = m_map->GetCell(player.GetPosition());

	Nz::Vector3f spawnPos = m_map->GetCellCenter(cellX, cellY);
	spawnPos += Nz::Vector3f::Up() * 0.5f;

	fmt::print("Player #{} placed a bomb on cell X={}, Y={}\n", player.GetIndex(), cellX, cellY);

	entt::registry& registry = GetRegistry();
	entt::entity bombEntity = registry.create();

	auto& bombNode = registry.emplace<Nz::NodeComponent>(bombEntity);
	bombNode.SetPosition(spawnPos);

	registry.emplace<BombComponent>(bombEntity, 3.f);

	NetCode::BombSpawnPacket bombSpawn;
	bombSpawn.position = spawnPos;

	BroadcastPacket(bombSpawn);
}

void ServerGame::HandlePlayerInputs()
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

		if (m_state == Game::State::Playing && lastInputs.placeBomb)
			HandlePlayerBomb(player);

		player.UpdatePosition(playerPos);
	}
}

void ServerGame::SwitchToState(Game::State state)
{
	switch (state)
	{
		case Game::State::Waiting:
			fmt::print("Switching to Waiting state\n");
			break;

		case Game::State::Starting:
		{
			// TODO: Compute position dynamically
			std::array<Nz::Vector2<std::size_t>, 4> StartingPos = {
				{
					{ 0, 0 },
					{ Map::Width - 1, 0 },
					{ 0, Map::Height - 1 },
					{ Map::Width - 1, Map::Height - 1 },
				}
			};

			std::size_t index = 0;
			for (ServerPlayer& player : m_playerPool)
			{
				if (index >= StartingPos.size())
				{
					fmt::print("Too many players\n");
					break;
				}

				Nz::Vector2<std::size_t> startingPos = StartingPos[index++];

				m_map->ClearCell(startingPos.x, startingPos.y);
				if (startingPos.x > 0)
					m_map->ClearCell(startingPos.x - 1, startingPos.y);
				if (startingPos.y > 0)
					m_map->ClearCell(startingPos.x, startingPos.y - 1);
				if (startingPos.x < Map::Width - 1)
					m_map->ClearCell(startingPos.x + 1, startingPos.y);
				if (startingPos.y < Map::Height - 1)
					m_map->ClearCell(startingPos.x, startingPos.y + 1);

				player.UpdatePosition(m_map->GetCellCenter(startingPos.x, startingPos.y));
			}

			fmt::print("Switching to Starting state\n");
			break;
		}

		case Game::State::Playing:  fmt::print("Switching to Playing state\n");
			break;

		case Game::State::Finished: fmt::print("Switching to Finished state\n");
			break;
	}

	m_state = state;
	m_stateClock.Restart();

	NetCode::GameStateUpdatePacket stateUpdate;
	stateUpdate.newState = m_state;

	BroadcastPacket(stateUpdate);
}
