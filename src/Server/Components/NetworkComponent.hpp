#pragma once

#include <Nazara/Prerequisites.hpp>
#include <Shared/NetCode.hpp>
#include <limits>

class NetworkComponent
{
	friend class NetworkSystem;

	public:
		inline NetworkComponent(NetCode::EntityType entityType);
		NetworkComponent(const NetworkComponent&) = delete;
		NetworkComponent(NetworkComponent&&) = default;
		~NetworkComponent() = default;

		inline NetCode::EntityType GetEntityType() const;
		inline Nz::UInt32 GetNetworkId() const;

		NetworkComponent& operator=(const NetworkComponent&) = delete;
		NetworkComponent& operator=(NetworkComponent&&) = default;

		static constexpr Nz::UInt32 NotNetworked = std::numeric_limits<Nz::UInt32>::max();

	private:
		inline void AssignNetworkId(Nz::UInt32 networkId);

		Nz::UInt32 m_networkId;
		NetCode::EntityType m_entityType;
};

#include <Server/Components/NetworkComponent.inl>
