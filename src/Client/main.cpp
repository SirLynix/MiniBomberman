#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Client/ClientGame.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Nz::Modules<Nz::Graphics, Nz::Network> nazara;

	Nz::PluginLoader pluginLoader;
	Nz::Plugin<Nz::AssimpPlugin> assimp = pluginLoader.Load<Nz::AssimpPlugin>();

	//std::cout << "Enter server ip: " << std::flush;
	std::string serverIp = "localhost";
	//std::cin >> serverIp;

	std::vector<Nz::HostnameInfo> hostnames = Nz::IpAddress::ResolveHostname(Nz::NetProtocol::IPv6, serverIp, "14642");
	if (hostnames.empty())
	{
		std::cout << "hostname not found" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		ClientGame game(hostnames.front().address);
		return game.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
