#pragma once

#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/ENetPacket.hpp>
#include <Shared/Map.hpp>
#include <Shared/PlayerInputs.hpp>
#include <cstddef>
#include <vector>

enum class PacketOpcode
{
	// Sent by clients
	C_PlayerInput,

	// Sent by server
	S_BombSpawn,
	S_InitialData,
	S_PlayerPositions
};

struct BombSpawnPacket
{
	static constexpr PacketOpcode OpCode = PacketOpcode::S_BombSpawn;
	static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
	static constexpr Nz::UInt8 ChannelId = 0;

	Nz::Vector3f position;

	void Serialize(Nz::ByteStream& stream) const;

	static BombSpawnPacket Unserialize(Nz::ByteStream& stream);
};

struct InitialDataPacket
{
	static constexpr PacketOpcode OpCode = PacketOpcode::S_InitialData;
	static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
	static constexpr Nz::UInt8 ChannelId = 0;

	Nz::UInt8 mapWidth;
	Nz::UInt8 mapHeight;
	std::vector<Map::CellType> mapCells;

	void Serialize(Nz::ByteStream& stream) const;

	static InitialDataPacket Unserialize(Nz::ByteStream& stream);
};

struct PlayerInputPacket
{
	static constexpr PacketOpcode OpCode = PacketOpcode::C_PlayerInput;
	static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
	static constexpr Nz::UInt8 ChannelId = 0;

	PlayerInputs inputs;

	void Serialize(Nz::ByteStream& stream) const;

	static PlayerInputPacket Unserialize(Nz::ByteStream& stream);
};

struct PlayerPositionsPacket
{
	static constexpr PacketOpcode OpCode = PacketOpcode::S_PlayerPositions;
	static constexpr Nz::ENetPacketFlags Flags = {};
	static constexpr Nz::UInt8 ChannelId = 0; //< TODO: set to 1

	struct PlayerData
	{
		Nz::UInt8 playerIndex;
		Nz::Vector3f position;
	};

	std::vector<PlayerData> playerPos;

	void Serialize(Nz::ByteStream& stream) const;

	static PlayerPositionsPacket Unserialize(Nz::ByteStream& stream);
};
