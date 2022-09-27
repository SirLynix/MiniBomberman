
#include <Server/Systems/BombSystem.hpp>
#include <Server/ServerMap.hpp>
#include <Server/Components/BombComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>

void BombSystem::Update(float elapsedTime)
{
	auto view = m_registry.view<const Nz::NodeComponent, BombComponent>();
	for (entt::entity entity : view)
	{
		auto& bombNode = view.get<const Nz::NodeComponent>(entity);
		auto& bombComponent = view.get<BombComponent>(entity);

		if (bombComponent.DecreaseDuration(elapsedTime))
		{
			auto [cellX, cellY] = m_map.GetCell(bombNode.GetPosition());

			m_map.ClearCell(cellX, cellY);
			if (cellX > 0)
				m_map.ClearCell(cellX - 1, cellY);
			if (cellY > 0)
				m_map.ClearCell(cellX, cellY - 1);
			if (cellX < Map::Width - 1)
				m_map.ClearCell(cellX + 1, cellY);
			if (cellY < Map::Height - 1)
				m_map.ClearCell(cellX, cellY + 1);

			m_registry.destroy(entity);
		}
	}
}
