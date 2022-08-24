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

	ClientGame game;
	return game.Run();
}
