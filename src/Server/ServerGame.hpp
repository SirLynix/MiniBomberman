#pragma once

#include <Shared/Game.hpp>
#include <Server/ServerMap.hpp>
#include <memory>

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

		std::unique_ptr<ServerMap> m_map;
};