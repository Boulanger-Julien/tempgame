#pragma once

struct HealthComponent {
	float mHealth;
};
struct HealthSystem {
	static void TakeDamage(HealthComponent& _healthComponent, float _damage);
	static void RecoverHealth(HealthComponent& _healthComponent, float _value);
};
