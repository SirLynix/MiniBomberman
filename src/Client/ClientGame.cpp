#include <Client/ClientGame.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Utility/Components.hpp>

ClientGame::ClientGame()
{
	std::shared_ptr<Nz::RenderDevice> renderDevice = Nz::Graphics::Instance()->GetRenderDevice();

	auto& renderSystem = GetSystemGraph().AddSystem<Nz::RenderSystem>();
	m_window = &renderSystem.CreateWindow(renderDevice, Nz::VideoMode(1920, 1080), "Bomberman");

	Nz::Mouse::SetRelativeMouseMode(true);

	SetupCamera();

	m_map = std::make_unique<ClientMap>(GetRegistry(), m_resources);

	SetupPlayerEntity();
}

bool ClientGame::OnUpdate(float elapsedTime)
{
	if (!m_window->IsOpen())
		return false;
	
	m_window->ProcessEvents();

	auto& playerNode = GetRegistry().get<Nz::NodeComponent>(m_debugPlayerEntity);

	constexpr float playerSpeed = 1.f;

	PlayerInputs inputs = m_player.PollInputs();
	if (inputs.moveDown)
		playerNode.Move(Nz::Vector3f::Backward() * elapsedTime * playerSpeed, Nz::CoordSys::Global);

	if (inputs.moveLeft)
		playerNode.Move(Nz::Vector3f::Left() * elapsedTime * playerSpeed, Nz::CoordSys::Global);

	if (inputs.moveRight)
		playerNode.Move(Nz::Vector3f::Right() * elapsedTime * playerSpeed, Nz::CoordSys::Global);

	if (inputs.moveUp)
		playerNode.Move(Nz::Vector3f::Forward() * elapsedTime * playerSpeed, Nz::CoordSys::Global);

	Nz::Vector3f playerPos = playerNode.GetPosition();

	auto& renderSystem = GetSystemGraph().GetSystem<Nz::RenderSystem>();
	renderSystem.GetFramePipeline().GetDebugDrawer().DrawLine(playerPos, playerPos + Nz::Vector3f::Up(), Nz::Color::Blue);

	constexpr float cameraSpeed = 5.f;

	auto& cameraNode = GetRegistry().get<Nz::NodeComponent>(m_cameraEntity);
	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
		cameraNode.Move(Nz::Vector3f::Forward() * elapsedTime * cameraSpeed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
		cameraNode.Move(Nz::Vector3f::Left() * elapsedTime * cameraSpeed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
		cameraNode.Move(Nz::Vector3f::Right() * elapsedTime * cameraSpeed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
		cameraNode.Move(Nz::Vector3f::Backward() * elapsedTime * cameraSpeed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift))
		cameraNode.Move(Nz::Vector3f::Up() * elapsedTime * cameraSpeed);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl))
		cameraNode.Move(Nz::Vector3f::Down() * elapsedTime * cameraSpeed);

	return true;
}

void ClientGame::OnUpsUpdate(unsigned int ups)
{
	m_window->SetTitle("Bomberman - " + std::to_string(ups) + " FPS");
}

void ClientGame::SetupCamera()
{
	m_cameraRotation = Nz::EulerAnglesf(-90.f, 0.f, 0.f);

	m_cameraEntity = GetRegistry().create();
	auto& cameraNode = GetRegistry().emplace<Nz::NodeComponent>(m_cameraEntity);
	cameraNode.SetPosition(Nz::Vector3f::Up() * 20.f);
	cameraNode.SetRotation(m_cameraRotation);

	auto& cameraComponent = GetRegistry().emplace<Nz::CameraComponent>(m_cameraEntity, m_window->GetRenderTarget());
	cameraComponent.UpdateZNear(0.01f);
	cameraComponent.UpdateClearColor(Nz::Color::FromRGB8(100, 100, 255));

	Nz::EventHandler& windowEvents = m_window->GetEventHandler();
	windowEvents.OnMouseMoved.Connect([&](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& mouseEvent)
	{
		float sensibility = 0.2f;

		m_cameraRotation.pitch -= mouseEvent.deltaY * sensibility;
		m_cameraRotation.yaw -= mouseEvent.deltaX * sensibility;

		auto& cameraNode = GetRegistry().get<Nz::NodeComponent>(m_cameraEntity);
		cameraNode.SetRotation(m_cameraRotation);
	});
}

void ClientGame::SetupPlayerEntity()
{
	Nz::Vector3f playerPosition = m_map->GetCellCenter(0, 0);

	m_debugPlayerEntity = GetRegistry().create();
	auto& playerNode = GetRegistry().emplace<Nz::NodeComponent>(m_debugPlayerEntity);
	playerNode.SetPosition(playerPosition);

	auto& playerGfx = GetRegistry().emplace<Nz::GraphicsComponent>(m_debugPlayerEntity);
	playerGfx.AttachRenderable(m_resources.playerModel, 0xFFFFFFFF);

	m_map->ClearCell(0, 0);
}
