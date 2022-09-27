#include <Server/Systems/BombSystem.hpp>

inline BombSystem::BombSystem(entt::registry& registry, ServerMap& map) :
m_registry(registry),
m_map(map)
{
}
