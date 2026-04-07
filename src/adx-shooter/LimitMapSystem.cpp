#include "pch.h"
#include "LimitMapSystem.h"

bool LimitMapSystem::CheckLimitMap(transformComponent& _player, Rooms& _currentRoom)
{
	transformComponent transformMap = ECS::GetInstance().getComponent<transformComponent>(_currentRoom.ground);
	ColliderComponent colliderMap = ECS::GetInstance().getComponent<ColliderComponent>(_currentRoom.ground);

	if (_player.position.x < transformMap.position.x - (transformMap.scale.x * 0.5f))
	{
		return true;
	}
	if (_player.position.x > transformMap.position.x + (transformMap.scale.x * 0.5f))
	{
		return true;
	}
	if (_player.position.z < transformMap.position.z - (transformMap.scale.z * 0.5f))
	{
		return true;
	}
	if (_player.position.z > transformMap.position.z + (transformMap.scale.z * 0.5f))
	{
		return true;
	}
	return false;
}
