#pragma once

class BombComponent
{
	public:
		inline BombComponent(float duration);
		BombComponent(const BombComponent&) = delete;
		BombComponent(BombComponent&&) = default;
		~BombComponent() = default;

		inline bool DecreaseDuration(float elapsedTime);

		inline float GetRemainingDuration() const;

		BombComponent& operator=(const BombComponent&) = delete;
		BombComponent& operator=(BombComponent&&) = default;

	private:
		float m_duration;
};

#include <Server/Components/BombComponent.inl>
