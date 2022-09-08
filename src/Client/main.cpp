#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Client/ClientGame.hpp>
#include <fmt/core.h>

int main()
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
		fmt::print("hostname not found\n");
		return EXIT_FAILURE;
	}

	try
	{
		ClientGame game(hostnames.front().address);
		return game.Run();
	}
	catch (const std::exception& e)
	{
		fmt::print("{}", e.what());
		return EXIT_FAILURE;
	}
}
