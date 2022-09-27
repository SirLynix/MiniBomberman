#include <Server/Components/BombComponent.hpp>

inline BombComponent::BombComponent(float duration) :
m_duration(duration)
{
}

inline bool BombComponent::DecreaseDuration(float elapsedTime)
{
	m_duration -= elapsedTime;
	return m_duration < 0.f;
}

inline float BombComponent::GetRemainingDuration() const
{
	return m_duration;
}