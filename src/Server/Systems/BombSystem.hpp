#pragma once

#include <Nazara/Utils/TypeList.hpp>
#include <entt/entt.hpp>

class ServerMap;

namespace Nz
{
	class NodeComponent;
}

class BombSystem
{
	public:
		static constexpr bool AllowConcurrent = false;
		using Components = Nz::TypeList<const Nz::NodeComponent, class BombComponent>;

		inline BombSystem(entt::registry& registry, ServerMap& map);
		BombSystem(const BombSystem&) = delete;
		BombSystem(BombSystem&&) = delete;
		~BombSystem() = default;

		void Update(float elapsedTime);

		BombSystem& operator=(const BombSystem&) = delete;
		BombSystem& operator=(BombSystem&&) = delete;

	private:
		entt::registry& m_registry;
		ServerMap& m_map;
};

#include <Server/Systems/BombSystem.inl>
