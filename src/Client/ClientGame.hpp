#pragma once

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Client/ClientMap.hpp>
#include <Client/ClientPlayer.hpp>
#include <Client/ClientResources.hpp>
#include <Shared/Game.hpp>
#include <entt/entt.hpp>
#include <memory>

class ClientGame : public Game
{
	public:
		ClientGame();
		ClientGame(const ClientGame&) = delete;
		ClientGame(ClientGame&&) = delete;
		~ClientGame() = default;

		ClientGame& operator=(const ClientGame&) = delete;
		ClientGame& operator=(ClientGame&&) = delete;

	private:
		bool OnUpdate(float elapsedTime) override;
		void OnUpsUpdate(unsigned int ups) override;
		
		void SetupCamera();
		void SetupPlayerEntity();

		entt::entity m_cameraEntity;
		entt::entity m_debugPlayerEntity;
		Nz::EulerAnglesf m_cameraRotation;
		Nz::RenderWindow* m_window;
		ClientResources m_resources;
		std::unique_ptr<ClientMap> m_map;
		ClientPlayer m_player;
};