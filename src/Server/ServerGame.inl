#include <Server/ServerGame.hpp>

template<typename T>
void ServerGame::BroadcastPacket(const T& packet)
{
	return BroadcastPacket(T::ChannelId, BuildPacket(packet));
}