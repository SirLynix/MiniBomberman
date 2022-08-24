#include <Nazara/Core.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Server/ServerGame.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Nz::Modules<Nz::Network> nazara;

	ServerGame game;
	return game.Run();
}
