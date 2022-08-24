#include <Shared/Game.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Utility/Components.hpp>

Game::Game() :
m_systemGraph(m_registry),
m_upsCounter(0)
{
}

int Game::Run()
{
	m_upsCounter = 0;

	Nz::Clock secondClock;
	Nz::Clock updateClock;
	for (;;)
	{
		if (!OnUpdate(updateClock.Restart() / 1'000'000.f))
			break;

		m_systemGraph.Update();

		m_upsCounter++;
		if (secondClock.GetSeconds() >= 1.f)
		{
			secondClock.Restart();
			OnUpsUpdate(m_upsCounter);
			m_upsCounter = 0;
		}
	}

	return 0;
}

entt::registry& Game::GetRegistry()
{
	return m_registry;
}

Nz::SystemGraph& Game::GetSystemGraph()
{
	return m_systemGraph;
}
