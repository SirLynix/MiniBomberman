#pragma once

#include <Nazara/Network/ENetHost.hpp>
#include <Shared/Game.hpp>
#include <Server/ServerMap.hpp>
#include <Server/ServerPlayer.hpp>
#include <Nazara/Utils/MemoryPool.hpp>
#include <memory>
#include <unordered_map>

class ServerGame : public Game
{
	public:
		ServerGame();
		ServerGame(const ServerGame&) = delete;
		ServerGame(ServerGame&&) = delete;
		~ServerGame() = default;

		ServerGame& operator=(const ServerGame&) = delete;
		ServerGame& operator=(ServerGame&&) = delete;

	private:
		bool OnUpdate(float elapsedTime) override;
		void OnUpsUpdate(unsigned int ups) override;

		std::unordered_map<std::size_t /* peerId */, std::size_t /* playerIndex */> m_peerIdToPlayerIndex;
		std::unique_ptr<ServerMap> m_map;
		Nz::ENetHost m_networkHost;
		Nz::MemoryPool<ServerPlayer> m_playerPool;
};