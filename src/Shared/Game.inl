#include <Shared/Game.hpp>

template<typename T>
Nz::ENetPacketRef Game::BuildPacket(const T& packet)
{
	Nz::ENetPacketRef packetRef = m_networkHost.AllocatePacket(T::Flags);

	Nz::UInt16 opcode = Nz::SafeCast<Nz::UInt16>(T::OpCode);
	packetRef->data << opcode;

	packet.Serialize(packetRef->data);

	packetRef->data.FlushBits();

	return packetRef;
}