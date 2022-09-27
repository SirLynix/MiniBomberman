#include <Server/ServerMap.hpp>

ServerMap::ServerMap()
{
	for (std::size_t y = 0; y < Height; ++y)
	{
		for (std::size_t x = 0; x < Width; ++x)
		{
			if (y % 2 == 1 && x % 2 == 1)
				m_cells[y * Width + x] = CellType::Stone;
			else
				m_cells[y * Width + x] = CellType::Box;
		}
	}
}

auto ServerMap::GetCellContent(std::size_t x, std::size_t y) const -> CellType
{
	assert(x < Width);
	assert(y < Height);
	return m_cells[y * Width + x];
}

void ServerMap::UpdateCell(std::size_t x, std::size_t y, CellType cellType)
{
	assert(x < Width);
	assert(y < Height);
	m_cells[y * Width + x] = cellType;

	if (cellType == Map::CellType::Empty)
		OnCellCleared(x, y);
}
