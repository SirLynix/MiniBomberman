#pragma once

#include <Shared/Map.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <array>

class ServerMap final : public Map
{
	public:
		ServerMap();
		ServerMap(const ServerMap&) = delete;
		ServerMap(ServerMap&&) = delete;
		~ServerMap() = default;

		CellType GetCellContent(std::size_t x, std::size_t y) const;

		void UpdateCell(std::size_t x, std::size_t y, CellType cellType) override;

		ServerMap& operator=(const ServerMap&) = delete;
		ServerMap& operator=(ServerMap&&) = delete;

		static constexpr std::size_t Height = 21;
		static constexpr std::size_t Width = 21;

		NazaraSignal(OnCellCleared, std::size_t /*x*/, std::size_t /*y*/);

	private:
		std::array<CellType, Width * Height> m_cells;
};
