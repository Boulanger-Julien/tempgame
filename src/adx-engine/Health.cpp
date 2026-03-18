#include "pch.h"
#include "Health.h"

void HealthSystem::TakeDamage(HealthComponent& _healthComponent, float _damage)
{
	_healthComponent.mHealth -= _damage;
}

void HealthSystem::RecoverHealth(HealthComponent& _healthComponent, float _value)
{
	_healthComponent.mHealth += _value;
}

