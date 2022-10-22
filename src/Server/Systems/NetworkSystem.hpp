#pragma once

#include <Nazara/Utils/Bitset.hpp>
#include <Nazara/Utils/TypeList.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace Nz
{
	class NodeComponent;
}

class ServerGame;
class ServerPlayer;

class NetworkSystem
{
	public:
		static constexpr bool AllowConcurrent = false;
		using Components = Nz::TypeList<const Nz::NodeComponent, class NetworkComponent>;

		NetworkSystem(entt::registry& registry, ServerGame& serverGame);
		NetworkSystem(const NetworkSystem&) = delete;
		NetworkSystem(NetworkSystem&&) = delete;
		~NetworkSystem();

		void CreateAllEntities(ServerPlayer& player);

		void Update(float elapsedTime);

		NetworkSystem& operator=(const NetworkSystem&) = delete;
		NetworkSystem& operator=(NetworkSystem&&) = delete;

	private:
		void OnNetworkDestroy(entt::registry& registry, entt::entity entity);

		ServerGame& m_serverGame;
		entt::registry& m_registry;
		entt::observer m_networkConstructObserver;
		entt::scoped_connection m_networkDestroyConnection;
		Nz::Bitset<Nz::UInt64> m_freeEntityIds;
		std::vector<Nz::UInt32> m_destroyedEntities;
};

#include <Server/Systems/NetworkSystem.inl>
