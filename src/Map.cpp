#include <Map.hpp>
#include <Resources.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Utility/Components.hpp>

Map::Map(entt::registry& registry, const Resources& resources) :
m_registry(registry)
{
	entt::entity floor = registry.create();
	auto& floorNode = registry.emplace<Nz::NodeComponent>(floor);
	floorNode.SetScale(Width * 1.5f, 1.f, Height * 1.5f);

	auto& floorGfx = registry.emplace<Nz::GraphicsComponent>(floor);
	floorGfx.AttachRenderable(resources.floorModel, 0xFFFFFFFF);

	for (std::size_t y = 0; y < Height; ++y)
	{
		for (std::size_t x = 0; x < Width; ++x)
		{
			Nz::Vector3f cellCenter = GetCellCenter(x, y);

			if (y % 2 == 1 && x % 2 == 1)
			{
				cellCenter.y += 0.75f * 0.9f + 0.01f;

				entt::entity cube = registry.create();
				auto& cubeNode = registry.emplace<Nz::NodeComponent>(cube);
				cubeNode.SetPosition(cellCenter);
				cubeNode.SetScale(0.9f);

				auto& cubeGfx = registry.emplace<Nz::GraphicsComponent>(cube);
				cubeGfx.AttachRenderable(resources.stoneModel, 0xFFFFFFFF);

				m_cells[y * Width + x] = entt::handle(m_registry, cube);
			}
			else
			{
				cellCenter.y += 0.75f + 0.01f;

				entt::entity box = registry.create();
				auto& cubeNode = registry.emplace<Nz::NodeComponent>(box);
				cubeNode.SetPosition(cellCenter);

				auto& cubeGfx = registry.emplace<Nz::GraphicsComponent>(box);
				cubeGfx.AttachRenderable(resources.boxModel, 0xFFFFFFFF);

				m_cells[y * Width + x] = entt::handle(m_registry, box);
			}
		}
	}
}

void Map::ClearCell(std::size_t x, std::size_t y)
{
	if (entt::handle entity = m_cells[y * Width + x])
		entity.destroy();
}

Nz::Vector3f Map::GetCellCenter(std::size_t x, std::size_t y) const
{
	Nz::Vector3f startVec(Width * -0.75f, 0.f, Height * -0.75f);
	return startVec + Nz::Vector3f(x * 1.5f + 0.75f, 0.f, y * 1.5f + 0.75f);
}
