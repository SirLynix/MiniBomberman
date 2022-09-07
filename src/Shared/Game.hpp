#pragma once

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Network/ENetHost.hpp>
#include <entt/entt.hpp>
#include <memory>

class Game
{
	public:
		Game();
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game() = default;

		template<typename T> Nz::ENetPacketRef BuildPacket(const T& packet);

		int Run();

		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;

	protected:
		entt::registry& GetRegistry();
		Nz::SystemGraph& GetSystemGraph();

		virtual void OnTick(bool lastTick) = 0;
		virtual bool OnUpdate(float elapsedTime) = 0;
		virtual void OnUpsUpdate(unsigned int ups) = 0;

		Nz::ENetHost m_networkHost;

	private:
		entt::registry m_registry;
		Nz::SystemGraph m_systemGraph;
		Nz::UInt64 m_tickCounter;
		unsigned int m_upsCounter;
};

#include <Shared/Game.inl>