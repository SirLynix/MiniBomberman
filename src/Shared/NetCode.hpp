#pragma once

#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/ENetPacket.hpp>
#include <Shared/Map.hpp>
#include <Shared/PlayerInputs.hpp>
#include <cstddef>
#include <string>
#include <vector>

namespace NetCode
{
	enum class Opcode
	{
		// Sent by clients
		C_PlayerInput,

		// Sent by server
		S_BombSpawn,
		S_InitialData,
		S_PlayerConnected,
		S_PlayerDisconnected,
		S_PlayerPositions
	};

	struct PlayerInfo
	{
		Nz::UInt8 index;
		Nz::Vector3f position;
		std::string name;

		void Serialize(Nz::ByteStream& stream) const;
		void Unserialize(Nz::ByteStream& stream);
	};

	struct BombSpawnPacket
	{
		static constexpr Opcode OpCode = Opcode::S_BombSpawn;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		Nz::Vector3f position;

		void Serialize(Nz::ByteStream& stream) const;

		static BombSpawnPacket Unserialize(Nz::ByteStream& stream);
	};

	struct InitialDataPacket
	{
		static constexpr Opcode OpCode = Opcode::S_InitialData;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		Nz::UInt8 mapWidth;
		Nz::UInt8 mapHeight;
		std::vector<Map::CellType> mapCells;
		Nz::UInt8 playerIndex;
		std::vector<PlayerInfo> players;

		void Serialize(Nz::ByteStream& stream) const;

		static InitialDataPacket Unserialize(Nz::ByteStream& stream);
	};

	struct PlayerConnectedPacket
	{
		static constexpr Opcode OpCode = Opcode::S_PlayerConnected;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		PlayerInfo playerInfo;

		void Serialize(Nz::ByteStream& stream) const;

		static PlayerConnectedPacket Unserialize(Nz::ByteStream& stream);
	};

	struct PlayerDisconnectedPacket
	{
		static constexpr Opcode OpCode = Opcode::S_PlayerDisconnected;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		Nz::UInt8 playerIndex;

		void Serialize(Nz::ByteStream& stream) const;

		static PlayerDisconnectedPacket Unserialize(Nz::ByteStream& stream);
	};

	struct PlayerInputPacket
	{
		static constexpr Opcode OpCode = Opcode::C_PlayerInput;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0; //< TODO: set to 1

		PlayerInputs inputs;

		void Serialize(Nz::ByteStream& stream) const;

		static PlayerInputPacket Unserialize(Nz::ByteStream& stream);
	};

	struct PlayerPositionsPacket
	{
		static constexpr Opcode OpCode = Opcode::S_PlayerPositions;
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
}
