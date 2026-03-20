#include "pch.h"
#include "LimitMapSystem.h"

void LimitMapSystem::CheckLimitMap(Player& _player, Rooms& _currentRoom)
{
	transformComponent transformMap = ECS::GetInstance().getComponent<transformComponent>(_currentRoom.ground);

	float dt = Timer::GetInstance()->GetDeltatime();
	float speed = _player.GetStats().mSpeed+ 5*dt;

	float rad = -45 * XM_PI / 180.0f;

	// Calcul des vecteurs forward (Z) et right (X)
	float forwardX = sin(rad) * dt * speed;
	float forwardZ = cos(rad) * dt * speed;

	float rightX = cos(rad) * dt * speed;
	float rightZ = -sin(rad) * dt * speed;


	if (_player.GetTransform().position.x < transformMap.position.x - (transformMap.scale.x * 0.5f))
	{
		transformSystem::Move(_player.GetTransform(), -forwardX, 0, 0);
		transformSystem::Move(_player.GetTransform(), 0, 0, -rightZ);
	}
	if (_player.GetTransform().position.x > transformMap.position.x + (transformMap.scale.x * 0.5f))
	{
		transformSystem::Move(_player.GetTransform(), forwardX, 0, 0);
		transformSystem::Move(_player.GetTransform(), 0, 0, rightZ);
	}
	if (_player.GetTransform().position.z < transformMap.position.z - (transformMap.scale.z * 0.5f))
	{
		transformSystem::Move(_player.GetTransform(), 0, 0, forwardZ);
		transformSystem::Move(_player.GetTransform(), rightX, 0, 0);
	}
	if (_player.GetTransform().position.z > transformMap.position.z + (transformMap.scale.z * 0.5f))
	{
		transformSystem::Move(_player.GetTransform(), 0, 0, -forwardZ);
		transformSystem::Move(_player.GetTransform(), -rightX, 0, 0);
	}
}
