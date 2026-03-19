#pragma once
#include "adx-engine\framework.h"
#include "Bullet.h"
#include "adx-render\Window.h"

struct Shot
{
	std::vector<Bullet*> bulletList;
};

class Shoot_Pattern_Single_Shot
{
public:
	Shoot_Pattern_Single_Shot() {
		if (instance == nullptr)
		{
			instance = this;
		}

	}
	~Shoot_Pattern_Single_Shot() {}
	inline static Shoot_Pattern_Single_Shot* instance;

	inline static Shoot_Pattern_Single_Shot& GetInstance() {
		if (instance == nullptr)
		{
			instance = new Shoot_Pattern_Single_Shot();
		}
		return *instance;
	}
	void Update(float _deltaTime);
	static Bullet* Shoot(Entity sender);
	void Reset();
	static void SetPlayerIndex(int index) { instance->mPlayerIndex = index; }
	int mPlayerIndex = -1;
};

class Shoot_Pattern_Explosion
{
	public:
	Shoot_Pattern_Explosion() {}
	~Shoot_Pattern_Explosion() {}
	inline static Shoot_Pattern_Explosion* instance;

	inline static Shoot_Pattern_Explosion& GetInstance() {
		if (instance == nullptr)
		{
			instance = new Shoot_Pattern_Explosion();
		}
		return *instance;
	}
	void Update(float _deltaTime);
	void Reset();
	static void SetPlayerIndex(int index) { instance->mPlayerIndex = index; }
	int mPlayerIndex = -1;
	static Shot* Shoot(Entity sender, float bullets, float _damage, Window* window);
};