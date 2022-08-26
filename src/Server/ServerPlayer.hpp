#pragma once

#include <Shared/Player.hpp>
#include <Nazara/Network/ENetPeer.hpp>

class ServerPlayer : public Player
{
	public:
		ServerPlayer(Nz::ENetPeer* peer);
		ServerPlayer(const ServerPlayer&) = delete;
		ServerPlayer(ServerPlayer&&) = delete;
		~ServerPlayer() = default;

		ServerPlayer& operator=(const ServerPlayer&) = delete;
		ServerPlayer& operator=(ServerPlayer&&) = delete;

	private:
		Nz::ENetPeer* m_peer;
};
