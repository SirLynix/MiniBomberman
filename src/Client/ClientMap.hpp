#pragma once

#include <Shared/Map.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <entt/entt.hpp>
#include <array>

class ClientResources;

class ClientMap final : public Map
{
	public:
		ClientMap(entt::registry& registry, const ClientResources& resources);
		ClientMap(const ClientMap&) = delete;
		ClientMap(ClientMap&&) = delete;
		~ClientMap() = default;

		void UpdateCell(std::size_t x, std::size_t y, CellType cellType) override;

		ClientMap& operator=(const ClientMap&) = delete;
		ClientMap& operator=(ClientMap&&) = delete;

		static constexpr std::size_t Height = 21;
		static constexpr std::size_t Width = 21;

	private:
		std::array<entt::handle, Width* Height> m_cells;
		entt::registry& m_registry;
		const ClientResources& m_resources;
};