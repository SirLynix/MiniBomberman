#include <Server/Systems/NetworkSystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Server/ServerGame.hpp>
#include <Server/ServerPlayer.hpp>
#include <Server/Components/NetworkComponent.hpp>

NetworkSystem::NetworkSystem(entt::registry& registry, ServerGame& serverGame) :
m_serverGame(serverGame),
m_registry(registry),
m_networkConstructObserver(registry, entt::collector.group<Nz::NodeComponent, NetworkComponent>())
{
	m_networkDestroyConnection = m_registry.on_destroy<NetworkComponent>().connect<&NetworkSystem::OnNetworkDestroy>(this);
}

NetworkSystem::~NetworkSystem()
{
	m_networkConstructObserver.disconnect();
}

void NetworkSystem::CreateAllEntities(ServerPlayer& player)
{
	auto view = m_registry.view<const Nz::NodeComponent, NetworkComponent>();
	for (entt::entity entity : view)
	{
		auto& entityNode = m_registry.get<const Nz::NodeComponent>(entity);
		auto& entityNetwork = m_registry.get<NetworkComponent>(entity);

		NetCode::EntitySpawnPacket entitySpawn;
		entitySpawn.type = entityNetwork.GetEntityType();
		entitySpawn.networkId = entityNetwork.GetNetworkId();
		entitySpawn.position = entityNode.GetPosition();
		entitySpawn.rotation = entityNode.GetRotation();

		player.SendPacket(entitySpawn);
	}
}

void NetworkSystem::Update(float elapsedTime)
{
	for (Nz::UInt32 networkId : m_destroyedEntities)
	{
		m_freeEntityIds.Set(networkId, true);

		NetCode::EntityDeletePacket entityDeletePacket;
		entityDeletePacket.networkId = networkId;

		m_serverGame.BroadcastPacket(entityDeletePacket);
	}
	m_destroyedEntities.clear();

	for (entt::entity entity : m_networkConstructObserver)
	{
		auto& entityNode = m_registry.get<const Nz::NodeComponent>(entity);
		auto& entityNetwork = m_registry.get<NetworkComponent>(entity);

		std::size_t entityId = m_freeEntityIds.FindFirst();
		if (entityId == m_freeEntityIds.npos)
		{
			entityId = m_freeEntityIds.GetSize();
			m_freeEntityIds.Resize(m_freeEntityIds.GetSize() + 1024, true);
		}
		m_freeEntityIds.Set(entityId, false);

		entityNetwork.AssignNetworkId(Nz::SafeCast<Nz::UInt32>(entityId));

		NetCode::EntitySpawnPacket entitySpawn;
		entitySpawn.type = entityNetwork.GetEntityType();
		entitySpawn.networkId = entityNetwork.GetNetworkId();
		entitySpawn.position = entityNode.GetPosition();
		entitySpawn.rotation = entityNode.GetRotation();

		m_serverGame.BroadcastPacket(entitySpawn);
	}
	m_networkConstructObserver.clear();

	/*auto view = m_registry.view<const Nz::NodeComponent, NetworkComponent>();
	for (entt::entity entity : view)
	{
		auto& entityNode = view.get<const Nz::NodeComponent>(entity);
		auto& entityNetwork = view.get<NetworkComponent>(entity);
	}*/
}

void NetworkSystem::OnNetworkDestroy(entt::registry& registry, entt::entity entity)
{
	auto& entityNetwork = m_registry.get<NetworkComponent>(entity);
	
	if (Nz::UInt32 networkId = entityNetwork.GetNetworkId(); networkId != NetworkComponent::NotNetworked)
		m_destroyedEntities.push_back(networkId);
}
