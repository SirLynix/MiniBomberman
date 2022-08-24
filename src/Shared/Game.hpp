#pragma once

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <entt/entt.hpp>
#include <memory>

class Game
{
	public:
		Game();
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game() = default;

		int Run();

		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;

	protected:
		entt::registry& GetRegistry();
		Nz::SystemGraph& GetSystemGraph();

		virtual bool OnUpdate(float elapsedTime) = 0;
		virtual void OnUpsUpdate(unsigned int ups) = 0;

	private:
		entt::registry m_registry;
		Nz::SystemGraph m_systemGraph;
		unsigned int m_upsCounter;
};