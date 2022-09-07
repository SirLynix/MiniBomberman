#include <Client/ClientGame.hpp>
#include <Shared/NetCode.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Utility/Components.hpp>
#include <iostream>
#include <stdexcept>

ClientGame::ClientGame(const Nz::IpAddress& serverAddress)
{
	if (!m_networkHost.Create(Nz::NetProtocol::IPv6, 0, 1, 2))
		throw std::runtime_error("failed to create network host");

	// Connect to server
	m_serverPeer = m_networkHost.Connect(serverAddress);
	WaitUntilConnected();

	std::shared_ptr<Nz::RenderDevice> renderDevice = Nz::Graphics::Instance()->GetRenderDevice();

	auto& renderSystem = GetSystemGraph().AddSystem<Nz::RenderSystem>();
	m_window = &renderSystem.CreateWindow(renderDevice, Nz::VideoMode(1920, 1080), "Bomberman");

	Nz::Mouse::SetRelativeMouseMode(true);

	SetupCamera();

	m_map = std::make_unique<ClientMap>(GetRegistry(), m_resources);

	SetupPlayerEntity();
}

void ClientGame::SendPacket(Nz::UInt8 channelId, Nz::ENetPacketRef packet)
{
	m_serverPeer->Send(channelId, std::move(packet));
}

void ClientGame::OnTick(bool lastTick)
{
	PlayerInputs inputs = m_player.PollInputs();

	PlayerInputPacket inputPacket;
	inputPacket.inputs = inputs;

	SendPacket(inputPacket);
}

bool ClientGame::OnUpdate(float elapsedTime)
{
	if (!m_window->IsOpen())
		return false;
	
	Nz::ENetEvent event;
	if (m_networkHost.Service(&event, 0) > 0)
	{
		do
		{
			switch (event.type)
			{
				case Nz::ENetEventType::None:
				case Nz::ENetEventType::IncomingConnect:
					break;

				case Nz::ENetEventType::Disconnect:
				{
					assert(event.peer == m_serverPeer);
					std::cout << "Disconnected from server" << std::endl;
					break;
				}

				case Nz::ENetEventType::OutgoingConnect:
				{
					assert(event.peer == m_serverPeer);
					std::cout << "Connected to server" << std::endl;
					break;
				}

				case Nz::ENetEventType::Receive:
				{
					assert(event.peer == m_serverPeer);

					Nz::NetPacket& netPacket = event.packet->data;

					Nz::UInt16 netCode;
					netPacket >> netCode;

					PacketOpcode opcode = Nz::SafeCast<PacketOpcode>(netCode);

					if (opcode == PacketOpcode::S_InitialData)
					{
						InitialDataPacket initialData = InitialDataPacket::Unserialize(netPacket);

						std::cout << "Received map" << std::endl;

						for (std::size_t y = 0; y < initialData.mapHeight; ++y)
						{
							for (std::size_t x = 0; x < initialData.mapWidth; ++x)
							{
								m_map->UpdateCell(x, y, initialData.mapCells[y * initialData.mapWidth + x]);
							}
						}
					}
					else if (opcode == PacketOpcode::S_PlayerPositions)
					{
						PlayerPositionsPacket playerPositions = PlayerPositionsPacket::Unserialize(netPacket);
						for (const auto& playerData : playerPositions.playerPos)
						{
							auto& playerNode = GetRegistry().get<Nz::NodeComponent>(m_debugPlayerEntity);
							playerNode.SetPosition({ playerData.position.x, 0.f, playerData.position.y });
						}
					}
					else if (opcode == PacketOpcode::S_BombSpawn)
					{
						BombSpawnPacket bombSpawn = BombSpawnPacket::Unserialize(netPacket);

						CreateBomb(bombSpawn.position);
					}
					break;
				}
			}
		}
		while (m_networkHost.CheckEvents(&event));
	}

	m_window->ProcessEvents();

	//auto& renderSystem = GetSystemGraph().GetSystem<Nz::RenderSystem>();
	//renderSystem.GetFramePipeline().GetDebugDrawer().DrawLine(playerPos, playerPos + Nz::Vector3f::Up(), Nz::Color::Blue);

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

void ClientGame::CreateBomb(const Nz::Vector3f& position)
{
	entt::entity bombEntity = GetRegistry().create();
	auto& bombNode = GetRegistry().emplace<Nz::NodeComponent>(bombEntity);
	bombNode.SetPosition(position);

	auto& bombGfx = GetRegistry().emplace<Nz::GraphicsComponent>(bombEntity);
	bombGfx.AttachRenderable(m_resources.bombModel, 0xFFFFFFFF);
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
}

void ClientGame::WaitUntilConnected()
{
	Nz::Clock timeoutClock;
	for (;;)
	{
		Nz::ENetEvent event;
		if (m_networkHost.Service(&event, 100) > 0)
		{
			do
			{
				switch (event.type)
				{
					case Nz::ENetEventType::OutgoingConnect:
					{
						assert(event.peer == m_serverPeer);
						std::cout << "Connected to server" << std::endl;
						return;
					}
				}
			}
			while (m_networkHost.CheckEvents(&event));
		}

		if (timeoutClock.GetSeconds() > 5.f)
			throw std::runtime_error("failed to connect to server");
	}
}
