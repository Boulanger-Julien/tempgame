#include "pch.h"
#include "Health.h"

void HealthSystem::TakeDamage(HealthComponent& _healthComponent, float _damage)
{
	if (_damage < 0)
		return;
	_healthComponent.mHealth -= _damage;
}

void HealthSystem::RecoverHealth(HealthComponent& _healthComponent, float _value)
{
	_healthComponent.mHealth += _value;
	if (_healthComponent.mHealth > _healthComponent.mMaxHealth)
		_healthComponent.mHealth = _healthComponent.mMaxHealth;

}

