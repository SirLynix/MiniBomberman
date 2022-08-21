#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Game.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Nz::Modules<Nz::Graphics> nazara;

	Nz::PluginLoader pluginLoader;
	Nz::Plugin<Nz::AssimpPlugin> assimp = pluginLoader.Load<Nz::AssimpPlugin>();

	Game game;
	return game.Run();
}
