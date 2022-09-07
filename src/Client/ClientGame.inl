#include <Client/ClientGame.hpp>

template<typename T>
void ClientGame::SendPacket(const T& packet)
{
	return SendPacket(T::ChannelId, BuildPacket(packet));
}