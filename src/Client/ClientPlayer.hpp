#pragma once

#include <Shared/Player.hpp>
#include <Shared/PlayerInputs.hpp>

class ClientPlayer : public Player
{
	public:
		ClientPlayer() = default;
		ClientPlayer(const ClientPlayer&) = delete;
		ClientPlayer(ClientPlayer&&) = delete;
		~ClientPlayer() = default;

		PlayerInputs PollInputs();

		ClientPlayer& operator=(const ClientPlayer&) = delete;
		ClientPlayer& operator=(ClientPlayer&&) = delete;
};