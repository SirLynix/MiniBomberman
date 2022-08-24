#pragma once

class Player
{
	public:
		Player() = default;
		Player(const Player&) = delete;
		Player(Player&&) = delete;
		~Player() = default;

		Player& operator=(const Player&) = delete;
		Player& operator=(Player&&) = delete;
};