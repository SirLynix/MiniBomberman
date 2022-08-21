#include <Game.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Utility/Components.hpp>

Game::Game() :
m_systemGraph(m_registry)
{
	std::shared_ptr<Nz::RenderDevice> renderDevice = Nz::Graphics::Instance()->GetRenderDevice();

	auto& renderSystem = m_systemGraph.AddSystem<Nz::RenderSystem>();
	m_window = &renderSystem.CreateWindow(renderDevice, Nz::VideoMode(1920, 1080), "Bomberman");

	Nz::Mouse::SetRelativeMouseMode(true);

	SetupCamera();

	m_map = std::make_unique<Map>(m_registry, m_resources);

	SetupPlayerEntity();
}

int Game::Run()
{
	m_fpsCounter = 0;

	Nz::Clock secondClock;
	Nz::Clock updateClock;
	while (m_window->IsOpen())
	{
		m_window->ProcessEvents();

		float deltaTime = updateClock.Restart() / 1'000'000.f;

		auto& playerNode = m_registry.get<Nz::NodeComponent>(m_debugPlayerEntity);

		constexpr float playerSpeed = 1.f;

		PlayerInputs inputs = m_player.PollInputs();
		if (inputs.moveDown)
			playerNode.Move(Nz::Vector3f::Backward() * deltaTime * playerSpeed, Nz::CoordSys::Global);

		if (inputs.moveLeft)
			playerNode.Move(Nz::Vector3f::Left() * deltaTime * playerSpeed, Nz::CoordSys::Global);

		if (inputs.moveRight)
			playerNode.Move(Nz::Vector3f::Right() * deltaTime * playerSpeed, Nz::CoordSys::Global);

		if (inputs.moveUp)
			playerNode.Move(Nz::Vector3f::Forward() * deltaTime * playerSpeed, Nz::CoordSys::Global);

		Nz::Vector3f playerPos = playerNode.GetPosition();

		auto& renderSystem = m_systemGraph.GetSystem<Nz::RenderSystem>();
		renderSystem.GetFramePipeline().GetDebugDrawer().DrawLine(playerPos, playerPos + Nz::Vector3f::Up(), Nz::Color::Blue);

		constexpr float cameraSpeed = 5.f;

		auto& cameraNode = m_registry.get<Nz::NodeComponent>(m_cameraEntity);
		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
			cameraNode.Move(Nz::Vector3f::Forward() * deltaTime * cameraSpeed);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
			cameraNode.Move(Nz::Vector3f::Left() * deltaTime * cameraSpeed);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
			cameraNode.Move(Nz::Vector3f::Right() * deltaTime * cameraSpeed);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
			cameraNode.Move(Nz::Vector3f::Backward() * deltaTime * cameraSpeed);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift))
			cameraNode.Move(Nz::Vector3f::Up() * deltaTime * cameraSpeed);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl))
			cameraNode.Move(Nz::Vector3f::Down() * deltaTime * cameraSpeed);

		m_systemGraph.Update();

		m_fpsCounter++;
		if (secondClock.GetSeconds() >= 1.f)
		{
			secondClock.Restart();
			m_window->SetTitle("Bomberman - " + std::to_string(m_fpsCounter) + " FPS");
			m_fpsCounter = 0;
		}
	}

	return 0;
}

void Game::SetupCamera()
{
	m_cameraRotation = Nz::EulerAnglesf(-90.f, 0.f, 0.f);

	m_cameraEntity = m_registry.create();
	auto& cameraNode = m_registry.emplace<Nz::NodeComponent>(m_cameraEntity);
	cameraNode.SetPosition(Nz::Vector3f::Up() * 20.f);
	cameraNode.SetRotation(m_cameraRotation);

	auto& cameraComponent = m_registry.emplace<Nz::CameraComponent>(m_cameraEntity, m_window->GetRenderTarget());
	cameraComponent.UpdateZNear(0.01f);
	cameraComponent.UpdateClearColor(Nz::Color::FromRGB8(100, 100, 255));

	Nz::EventHandler& windowEvents = m_window->GetEventHandler();
	windowEvents.OnMouseMoved.Connect([&](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& mouseEvent)
	{
		float sensibility = 0.2f;

		m_cameraRotation.pitch -= mouseEvent.deltaY * sensibility;
		m_cameraRotation.yaw -= mouseEvent.deltaX * sensibility;

		auto& cameraNode = m_registry.get<Nz::NodeComponent>(m_cameraEntity);
		cameraNode.SetRotation(m_cameraRotation);
	});
}

void Game::SetupPlayerEntity()
{
	Nz::Vector3f playerPosition = m_map->GetCellCenter(0, 0);

	m_debugPlayerEntity = m_registry.create();
	auto& playerNode = m_registry.emplace<Nz::NodeComponent>(m_debugPlayerEntity);
	playerNode.SetPosition(playerPosition);

	auto& playerGfx = m_registry.emplace<Nz::GraphicsComponent>(m_debugPlayerEntity);
	playerGfx.AttachRenderable(m_resources.playerModel, 0xFFFFFFFF);

	m_map->ClearCell(0, 0);
}
