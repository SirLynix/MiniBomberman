#pragma once

#include <Shared/Game.hpp>
#include <Shared/NetCode.hpp>
#include <Server/ServerMap.hpp>
#include <Server/ServerPlayer.hpp>
#include <Nazara/Core/Clock.hpp>
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

		template<typename T> void BroadcastPacket(const T& packet, ServerPlayer* except = nullptr);
		void BroadcastPacket(Nz::UInt8 channelId, Nz::ENetPacketRef packet, ServerPlayer* except = nullptr);

		ServerMap& GetMap();

		ServerGame& operator=(const ServerGame&) = delete;
		ServerGame& operator=(ServerGame&&) = delete;

		static constexpr std::size_t MinPlayerCount = 1;
		static constexpr Nz::UInt64 MaxMatchDuration = 10 * 60 * 1000;
		static constexpr Nz::UInt64 RestartTime = 5 * 1000;
		static constexpr Nz::UInt64 StartingTime = 5 * 1000;

	private:
		void OnTick(bool lastTick) override;
		bool OnUpdate(float elapsedTime) override;
		void OnUpsUpdate(unsigned int ups) override;

		void BroadcastMapUpdate();
		void BroadcastPlayerPosition();
		void HandlePlayerBomb(ServerPlayer& player);
		void HandlePlayerInputs();
		void SwitchToState(Game::State state);

		std::unordered_map<std::size_t /* peerId */, std::size_t /* playerIndex */> m_peerIdToPlayerIndex;
		std::unique_ptr<ServerMap> m_map;
		Nz::Clock m_stateClock;
		Nz::MemoryPool<ServerPlayer> m_playerPool;
		State m_state;
		NetCode::MapClearCellsPacket m_mapClearCells;
};

#include <Server/ServerGame.inl>