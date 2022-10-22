#pragma once

#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/ENetPacket.hpp>
#include <Shared/Game.hpp>
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
		S_EntityDelete,
		S_EntitySpawn,
		S_GameStateUpdate,
		S_InitialData,
		S_MapClearCells,
		S_PlayerConnected,
		S_PlayerDisconnected,
		S_PlayerPositions
	};

	enum class EntityType
	{
		Bomb,
		Player
	};

	struct PlayerInfo
	{
		Nz::UInt8 index;
		Nz::Vector3f position;
		std::string name;

		void Serialize(Nz::ByteStream& stream) const;
		void Unserialize(Nz::ByteStream& stream);
	};

	struct EntityDeletePacket
	{
		static constexpr Opcode OpCode = Opcode::S_EntityDelete;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		Nz::UInt32 networkId;

		void Serialize(Nz::ByteStream& stream) const;

		static EntityDeletePacket Unserialize(Nz::ByteStream& stream);
	};

	struct EntitySpawnPacket
	{
		static constexpr Opcode OpCode = Opcode::S_EntitySpawn;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		EntityType type;
		Nz::Quaternionf rotation;
		Nz::Vector3f position;
		Nz::UInt32 networkId;

		void Serialize(Nz::ByteStream& stream) const;

		static EntitySpawnPacket Unserialize(Nz::ByteStream& stream);
	};

	struct GameStateUpdatePacket
	{
		static constexpr Opcode OpCode = Opcode::S_GameStateUpdate;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		Game::State newState;

		void Serialize(Nz::ByteStream& stream) const;

		static GameStateUpdatePacket Unserialize(Nz::ByteStream& stream);
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
		Game::State gameState;

		void Serialize(Nz::ByteStream& stream) const;

		static InitialDataPacket Unserialize(Nz::ByteStream& stream);
	};

	struct MapClearCellsPacket
	{
		static constexpr Opcode OpCode = Opcode::S_MapClearCells;
		static constexpr Nz::ENetPacketFlags Flags = Nz::ENetPacketFlag_Reliable;
		static constexpr Nz::UInt8 ChannelId = 0;

		std::vector<Nz::UInt16> cellIds;

		void Serialize(Nz::ByteStream& stream) const;

		static MapClearCellsPacket Unserialize(Nz::ByteStream& stream);
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
