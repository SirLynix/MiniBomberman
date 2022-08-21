#pragma once

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Map.hpp>
#include <Player.hpp>
#include <Resources.hpp>
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

	private:
		void SetupCamera();
		void SetupPlayerEntity();

		entt::entity m_cameraEntity;
		entt::entity m_debugPlayerEntity;
		entt::registry m_registry;
		Nz::EulerAnglesf m_cameraRotation;
		Nz::SystemGraph m_systemGraph;
		Nz::RenderWindow* m_window;
		Resources m_resources;
		std::unique_ptr<Map> m_map;
		Player m_player;
		unsigned int m_fpsCounter;
};