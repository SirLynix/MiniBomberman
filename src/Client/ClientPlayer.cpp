
#include <Client/ClientPlayer.hpp>
#include <Nazara/Platform/Keyboard.hpp>

PlayerInputs ClientPlayer::PollInputs()
{
	PlayerInputs inputs;
	inputs.moveDown = Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down);
	inputs.moveLeft = Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left);
	inputs.moveRight = Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right);
	inputs.moveUp = Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up);
	inputs.placeBomb = Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Return);

	return inputs;
}
