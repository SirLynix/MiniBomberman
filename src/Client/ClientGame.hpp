#pragma once

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Network/ENetHost.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Client/ClientMap.hpp>
#include <Client/ClientPlayer.hpp>
#include <Client/ClientResources.hpp>
#include <Shared/Game.hpp>
#include <Shared/NetCode.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

class ClientGame : public Game
{
	public:
		ClientGame(const Nz::IpAddress& serverAddress);
		ClientGame(const ClientGame&) = delete;
		ClientGame(ClientGame&&) = delete;
		~ClientGame() = default;

		template<typename T> void SendPacket(const T& packet);
		void SendPacket(Nz::UInt8 channelId, Nz::ENetPacketRef packet);

		ClientGame& operator=(const ClientGame&) = delete;
		ClientGame& operator=(ClientGame&&) = delete;

	private:
		void OnTick(bool lastTick) override;
		bool OnUpdate(float elapsedTime) override;
		void OnUpsUpdate(unsigned int ups) override;
		
		entt::entity CreateBomb(const Nz::Vector3f& position);
		void CreatePlayer(NetCode::PlayerInfo&& netPlayerInfo);
		void DestroyPlayer(Nz::UInt8 playerIndex);
		void SetupCamera();
		void WaitUntilConnected();

		struct PlayerInfo
		{
			entt::entity playerEntity;
			std::string name;
		};

		entt::entity m_cameraEntity;
		Nz::ENetHost m_networkHost;
		Nz::ENetPeer* m_serverPeer;
		Nz::EulerAnglesf m_cameraRotation;
		Nz::RenderWindow* m_window;
		ClientResources m_resources;
		std::unique_ptr<ClientMap> m_map;
		std::vector<entt::entity> m_networkedEntities;
		std::vector<std::optional<PlayerInfo>> m_playerInfo;
		ClientPlayer m_player;
		State m_currentState;
};

#include <Client/ClientGame.inl>