
#include <Shared/NetCode.hpp>

void BombSpawnPacket::Serialize(Nz::ByteStream& stream) const
{
	stream << position;
}

BombSpawnPacket BombSpawnPacket::Unserialize(Nz::ByteStream& stream)
{
	BombSpawnPacket packet;
	stream >> packet.position;

	return packet;
}

void InitialDataPacket::Serialize(Nz::ByteStream& stream) const
{
	stream << mapWidth << mapHeight;

	assert(mapCells.size() == mapWidth * mapHeight);
	for (Map::CellType cellType : mapCells)
	{
		Nz::UInt8 typeIndex = Nz::SafeCast<Nz::UInt8>(cellType);
		stream << typeIndex;
	}
}

InitialDataPacket InitialDataPacket::Unserialize(Nz::ByteStream& stream)
{
	InitialDataPacket packet;
	stream >> packet.mapWidth >> packet.mapHeight;

	packet.mapCells.resize(packet.mapWidth * packet.mapHeight);
	for (Map::CellType& cellType : packet.mapCells)
	{
		Nz::UInt8 typeIndex;
		stream >> typeIndex;

		cellType = Nz::SafeCast<Map::CellType>(typeIndex);
	}

	return packet;
}

void PlayerInputPacket::Serialize(Nz::ByteStream& stream) const
{
	stream << inputs.moveDown << inputs.moveLeft << inputs.moveRight << inputs.moveUp << inputs.placeBomb;
}

PlayerInputPacket PlayerInputPacket::Unserialize(Nz::ByteStream& stream)
{
	PlayerInputPacket packet;
	stream >> packet.inputs.moveDown >> packet.inputs.moveLeft >> packet.inputs.moveRight >> packet.inputs.moveUp >> packet.inputs.placeBomb;

	return packet;
}

void PlayerPositionsPacket::Serialize(Nz::ByteStream& stream) const
{
	Nz::UInt8 playerCount = Nz::SafeCast<Nz::UInt8>(playerPos.size());
	stream << playerCount;

	for (const auto& playerData : playerPos)
		stream << playerData.playerIndex << playerData.position;
}

PlayerPositionsPacket PlayerPositionsPacket::Unserialize(Nz::ByteStream& stream)
{
	PlayerPositionsPacket packet;
	
	Nz::UInt8 playerCount;
	stream >> playerCount;

	packet.playerPos.resize(playerCount);
	for (auto& playerData : packet.playerPos)
		stream >> playerData.playerIndex >> playerData.position;

	return packet;
}
