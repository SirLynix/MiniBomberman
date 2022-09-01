#pragma once

#include <Shared/Player.hpp>
#include <Shared/PlayerInputs.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Network/ENetPeer.hpp>

class ServerPlayer : public Player
{
	friend class ServerGame;

	public:
		ServerPlayer(ServerGame& game, Nz::ENetPeer* peer);
		ServerPlayer(const ServerPlayer&) = delete;
		ServerPlayer(ServerPlayer&&) = delete;
		~ServerPlayer() = default;

		std::size_t GetIndex() const;
		const PlayerInputs& GetLastInputs() const;
		Nz::ENetPeer* GetPeer() const;
		const Nz::Vector2f& GetPosition() const;

		void HandlePlaceBomb();

		void UpdateInputs(const PlayerInputs& inputs);
		void UpdatePosition(const Nz::Vector2f& position);

		ServerPlayer& operator=(const ServerPlayer&) = delete;
		ServerPlayer& operator=(ServerPlayer&&) = delete;

	private:
		void UpdateIndex(std::size_t index);

		std::size_t m_index;
		Nz::ENetPeer* m_peer;
		Nz::Vector2f m_position;
		ServerGame& m_game;
		PlayerInputs m_lastInputs;
};
