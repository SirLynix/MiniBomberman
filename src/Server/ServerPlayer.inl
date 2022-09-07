#include <Server/ServerPlayer.hpp>

template<typename T>
void ServerPlayer::SendPacket(const T& packet)
{
	SendPacket(T::ChannelId, m_game.BuildPacket(packet));
}
