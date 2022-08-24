#include <Shared/Map.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Utility/Components.hpp>

void Map::ClearCell(std::size_t x, std::size_t y)
{
	UpdateCell(x, y, CellType::Empty);
}

Nz::Vector3f Map::GetCellCenter(std::size_t x, std::size_t y) const
{
	Nz::Vector3f startVec(Width * CellSize * -0.5f, 0.f, Height * CellSize * -0.5f);
	return startVec + Nz::Vector3f(x * CellSize + CellSize * 0.5f, 0.f, y * CellSize + CellSize * 0.5f);
}
