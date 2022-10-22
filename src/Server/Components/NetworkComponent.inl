#include <Server/Components/NetworkComponent.hpp>
#include <cassert>

inline NetworkComponent::NetworkComponent(NetCode::EntityType entityType) :
m_networkId(NotNetworked),
m_entityType(entityType)
{
}

inline NetCode::EntityType NetworkComponent::GetEntityType() const
{
	return m_entityType;
}

inline Nz::UInt32 NetworkComponent::GetNetworkId() const
{
	return m_networkId;
}

inline void NetworkComponent::AssignNetworkId(Nz::UInt32 networkId)
{
	assert(m_networkId == NotNetworked);
	m_networkId = networkId;
}
