#pragma once

#include <Shared/Player.hpp>
#include <Shared/PlayerInputs.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/ENetPeer.hpp>

class ServerPlayer : public Player
{
	friend class ServerGame;

	public:
		ServerPlayer(ServerGame& game, Nz::ENetPeer* peer, std::string name);
		ServerPlayer(const ServerPlayer&) = delete;
		ServerPlayer(ServerPlayer&&) = delete;
		~ServerPlayer() = default;

		std::size_t GetIndex() const;
		const PlayerInputs& GetLastInputs() const;
		const std::string& GetName() const;
		Nz::ENetPeer* GetPeer() const;
		const Nz::Vector3f& GetPosition() const;

		void HandlePlaceBomb();

		void SendPacket(Nz::UInt8 channelId, Nz::ENetPacketRef packet);
		template<typename T> void SendPacket(const T& packet);

		void UpdateInputs(const PlayerInputs& inputs);
		void UpdatePosition(const Nz::Vector3f& position);

		ServerPlayer& operator=(const ServerPlayer&) = delete;
		ServerPlayer& operator=(ServerPlayer&&) = delete;

	private:
		void UpdateIndex(std::size_t index);

		std::size_t m_index;
		std::string m_name;
		Nz::ENetPeer* m_peer;
		Nz::Vector3f m_position;
		ServerGame& m_game;
		PlayerInputs m_lastInputs;
};

#include <Server/ServerPlayer.inl>
