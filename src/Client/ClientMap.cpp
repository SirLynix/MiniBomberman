#include <Client/ClientMap.hpp>
#include <Client/ClientResources.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Utility/Components.hpp>
#include <cassert>

ClientMap::ClientMap(entt::registry& registry, const ClientResources& resources) :
m_registry(registry),
m_resources(resources)
{
	entt::entity floor = registry.create();
	auto& floorNode = registry.emplace<Nz::NodeComponent>(floor);
	floorNode.SetScale(Width * CellSize, 1.f, Height * CellSize);

	auto& floorGfx = registry.emplace<Nz::GraphicsComponent>(floor);
	floorGfx.AttachRenderable(resources.floorModel, 0xFFFFFFFF);

	UpdateCell(5, 8, CellType::Box);
	UpdateCell(3, 2, CellType::Stone);
}

void ClientMap::UpdateCell(std::size_t x, std::size_t y, CellType cellType)
{
	if (cellType == CellType::Empty)
	{
		if (entt::handle entity = m_cells[y * Width + x])
			entity.destroy();
	}
	else
	{
		Nz::Vector3f cellCenter = GetCellCenter(x, y);

		if (cellType == CellType::Box)
		{
			cellCenter.y += CellSize * 0.5f + 0.01f;

			entt::entity box = m_registry.create();
			auto& cubeNode = m_registry.emplace<Nz::NodeComponent>(box);
			cubeNode.SetPosition(cellCenter);

			auto& cubeGfx = m_registry.emplace<Nz::GraphicsComponent>(box);
			cubeGfx.AttachRenderable(m_resources.boxModel, 0xFFFFFFFF);

			m_cells[y * Width + x] = entt::handle(m_registry, box);
		}
		else
		{
			assert(cellType == Map::CellType::Stone);

			cellCenter.y += CellSize * 0.5 * 0.9f + 0.01f;

			entt::entity cube = m_registry.create();
			auto& cubeNode = m_registry.emplace<Nz::NodeComponent>(cube);
			cubeNode.SetPosition(cellCenter);
			cubeNode.SetScale(0.9f);

			auto& cubeGfx = m_registry.emplace<Nz::GraphicsComponent>(cube);
			cubeGfx.AttachRenderable(m_resources.stoneModel, 0xFFFFFFFF);

			m_cells[y * Width + x] = entt::handle(m_registry, cube);
		}
	}
}
