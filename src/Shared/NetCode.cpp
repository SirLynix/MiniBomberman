
#include <Shared/NetCode.hpp>

void NetCode::PlayerInfo::Serialize(Nz::ByteStream& stream) const
{
	stream << index << name << position;
}

void NetCode::PlayerInfo::Unserialize(Nz::ByteStream& stream)
{
	stream >> index >> name >> position;
}

void NetCode::EntityDeletePacket::Serialize(Nz::ByteStream& stream) const
{
	stream << networkId;
}

NetCode::EntityDeletePacket NetCode::EntityDeletePacket::Unserialize(Nz::ByteStream& stream)
{
	EntityDeletePacket packet;
	stream >> packet.networkId;

	return packet;
}

void NetCode::EntitySpawnPacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt8 typeInt = Nz::SafeCast<Nz::UInt8>(type);
	stream << typeInt << networkId << position << rotation;
}

NetCode::EntitySpawnPacket NetCode::EntitySpawnPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::EntitySpawnPacket packet;

	Nz::UInt8 typeInt;
	stream >> typeInt;

	packet.type = Nz::SafeCast<EntityType>(typeInt);

	stream >> packet.networkId >> packet.position >> packet.rotation;

	return packet;
}

void NetCode::GameStateUpdatePacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt8 state = Nz::SafeCast<Nz::UInt8>(newState);
	stream << state;
}

NetCode::GameStateUpdatePacket NetCode::GameStateUpdatePacket::Unserialize(Nz::ByteStream& stream)
{
	GameStateUpdatePacket packet;

	Nz::UInt8 state;
	stream >> state;

	packet.newState = Nz::SafeCast<Game::State>(state);

	return packet;
}

void NetCode::InitialDataPacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt8 state = Nz::SafeCast<Nz::UInt8>(gameState);
	stream << state;

	stream << mapWidth << mapHeight;

	assert(mapCells.size() == mapWidth * mapHeight);
	for (Map::CellType cellType : mapCells)
	{
		Nz::UInt8 typeIndex = Nz::SafeCast<Nz::UInt8>(cellType);
		stream << typeIndex;
	}

	stream << playerIndex;

	Nz::UInt8 playerCount = Nz::SafeCast<Nz::UInt8>(players.size());
	stream << playerCount;

	for (const auto& playerInfo : players)
		playerInfo.Serialize(stream);
}

NetCode::InitialDataPacket NetCode::InitialDataPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::InitialDataPacket packet;

	Nz::UInt8 state;
	stream >> state;

	packet.gameState = Nz::SafeCast<Game::State>(state);

	stream >> packet.mapWidth >> packet.mapHeight;

	packet.mapCells.resize(packet.mapWidth * packet.mapHeight);
	for (Map::CellType& cellType : packet.mapCells)
	{
		Nz::UInt8 typeIndex;
		stream >> typeIndex;

		cellType = Nz::SafeCast<Map::CellType>(typeIndex);
	}

	Nz::UInt8 playerIndex;
	stream >> playerIndex;

	Nz::UInt8 playerCount;
	stream >> playerCount;

	packet.players.resize(playerCount);
	for (auto& playerInfo : packet.players)
		playerInfo.Unserialize(stream);

	return packet;
}

void NetCode::MapClearCellsPacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt16 cellCount = Nz::SafeCast<Nz::UInt16>(cellIds.size());
	stream << cellCount;

	for (Nz::UInt16 cellId : cellIds)
		stream << cellId;
}

NetCode::MapClearCellsPacket NetCode::MapClearCellsPacket::Unserialize(Nz::ByteStream& stream)
{
	MapClearCellsPacket packet;

	Nz::UInt16 cellCount;
	stream >> cellCount;

	packet.cellIds.resize(cellCount);
	for (Nz::UInt16& cellId : packet.cellIds)
		stream >> cellId;

	return packet;
}


void NetCode::PlayerConnectedPacket::Serialize(Nz::ByteStream& stream) const
{
	playerInfo.Serialize(stream);
}

NetCode::PlayerConnectedPacket NetCode::PlayerConnectedPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::PlayerConnectedPacket packet;
	packet.playerInfo.Unserialize(stream);

	return packet;
}

void NetCode::PlayerDisconnectedPacket::Serialize(Nz::ByteStream& stream) const
{
	stream << playerIndex;
}

NetCode::PlayerDisconnectedPacket NetCode::PlayerDisconnectedPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::PlayerDisconnectedPacket packet;
	stream >> packet.playerIndex;

	return packet;
}

void NetCode::PlayerInputPacket::Serialize(Nz::ByteStream& stream) const
{
	stream << inputs.moveDown << inputs.moveLeft << inputs.moveRight << inputs.moveUp << inputs.placeBomb;
}

NetCode::PlayerInputPacket NetCode::PlayerInputPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::PlayerInputPacket packet;
	stream >> packet.inputs.moveDown >> packet.inputs.moveLeft >> packet.inputs.moveRight >> packet.inputs.moveUp >> packet.inputs.placeBomb;

	return packet;
}

void NetCode::PlayerPositionsPacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt8 playerCount = Nz::SafeCast<Nz::UInt8>(playerPos.size());
	stream << playerCount;

	for (const auto& playerData : playerPos)
		stream << playerData.playerIndex << playerData.position;
}

NetCode::PlayerPositionsPacket NetCode::PlayerPositionsPacket::Unserialize(Nz::ByteStream& stream)
{
	NetCode::PlayerPositionsPacket packet;
	
	Nz::UInt8 playerCount;
	stream >> playerCount;

	packet.playerPos.resize(playerCount);
	for (auto& playerData : packet.playerPos)
		stream >> playerData.playerIndex >> playerData.position;

	return packet;
}
