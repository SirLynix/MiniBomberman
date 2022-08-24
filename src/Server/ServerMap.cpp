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

void ServerMap::UpdateCell(std::size_t x, std::size_t y, CellType cellType)
{
	m_cells[y * Width + x] = cellType;
}
