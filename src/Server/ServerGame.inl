#include <Server/ServerGame.hpp>

template<typename T>
void ServerGame::BroadcastPacket(const T& packet, ServerPlayer* except)
{
	return BroadcastPacket(T::ChannelId, BuildPacket(packet), except);
}