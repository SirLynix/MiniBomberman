#pragma once

#include <PlayerInputs.hpp>

class Player
{
	public:
		Player() = default;
		Player(const Player&) = delete;
		Player(Player&&) = delete;
		~Player() = default;

		PlayerInputs PollInputs();

		Player& operator=(const Player&) = delete;
		Player& operator=(Player&&) = delete;
};