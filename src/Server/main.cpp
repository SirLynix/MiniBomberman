#include <Nazara/Core.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Server/ServerGame.hpp>
#include <iostream>

int main()
{
	Nz::Modules<Nz::Network> nazara;

	try
	{
		ServerGame game;
		return game.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
