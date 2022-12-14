#pragma once

#include <Nazara/Math/Vector3.hpp>

class Map
{
	public:
		enum class CellType
		{
			Box   = 1,
			Empty = 0,
			Stone = 2
		};

		Map() = default;
		Map(const Map&) = delete;
		Map(Map&&) = delete;
		~Map() = default;

		void ClearCell(std::size_t x, std::size_t y);

		std::pair<std::size_t, std::size_t> GetCell(const Nz::Vector3f& position) const;
		Nz::Vector3f GetCellCenter(std::size_t x, std::size_t y) const;

		virtual void UpdateCell(std::size_t x, std::size_t y, CellType cellType) = 0;

		Map& operator=(const Map&) = delete;
		Map& operator=(Map&&) = delete;

		static constexpr float CellSize = 1.5f;
		static constexpr std::size_t Height = 21;
		static constexpr std::size_t Width = 21;
};