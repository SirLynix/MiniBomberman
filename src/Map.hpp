#pragma once

#include <Nazara/Math/Vector3.hpp>
#include <entt/entt.hpp>
#include <array>

class Resources;

class Map
{
	public:
		Map(entt::registry& registry, const Resources& resources);
		Map(const Map&) = delete;
		Map(Map&&) = delete;
		~Map() = default;

		void ClearCell(std::size_t x, std::size_t y);

		Nz::Vector3f GetCellCenter(std::size_t x, std::size_t y) const;

		Map& operator=(const Map&) = delete;
		Map& operator=(Map&&) = delete;

		static constexpr std::size_t Height = 21;
		static constexpr std::size_t Width = 21;

	private:
		std::array<entt::handle, Width* Height> m_cells;
		entt::registry& m_registry;
};