#include <Server/ServerGame.hpp>
#include <iostream>

ServerGame::ServerGame()
{
	m_map = std::make_unique<ServerMap>();
}

bool ServerGame::OnUpdate(float elapsedTime)
{
	return true;
}

void ServerGame::OnUpsUpdate(unsigned int ups)
{
	std::cout << ups << " ups" << std::endl;
}
