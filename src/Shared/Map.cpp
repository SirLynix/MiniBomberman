#include <Shared/Map.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Utility/Components.hpp>

void Map::ClearCell(std::size_t x, std::size_t y)
{
	UpdateCell(x, y, CellType::Empty);
}

std::pair<std::size_t, std::size_t> Map::GetCell(const Nz::Vector3f& position) const
{
	Nz::Vector3f offset = position + Nz::Vector3f(Width * CellSize * 0.5f - CellSize * 0.5f, 0.f, Height * CellSize * 0.5f - CellSize * 0.5f);
	offset.x = std::max(0.f, offset.x);
	offset.z = std::max(0.f, offset.z);

	std::size_t cellX = std::min(static_cast<std::size_t>(std::round(offset.x / CellSize)), Width - 1);
	std::size_t cellY = std::min(static_cast<std::size_t>(std::round(offset.z / CellSize)), Height - 1);

	return { cellX, cellY };
}

Nz::Vector3f Map::GetCellCenter(std::size_t x, std::size_t y) const
{
	Nz::Vector3f startVec(Width * CellSize * -0.5f, 0.f, Height * CellSize * -0.5f);
	return startVec + Nz::Vector3f(x * CellSize + CellSize * 0.5f, 0.f, y * CellSize + CellSize * 0.5f);
}
