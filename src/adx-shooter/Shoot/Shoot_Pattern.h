#pragma once
#include "adx-engine\framework.h"
#include "Bullet.h"
#include "adx-render\Window.h"

struct Shot
{
	std::vector<Bullet*> bulletList;
};

enum ShootPatternType
{
	None,
	Single_Shot,
	Explosion,
	Pump,
	Line,
	Amount
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
	static Bullet* Shoot(Entity sender, float scale);
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
	static Shot* Shoot(Entity sender, float bullets, float _damage, Window* window, float scale);
};

class Shoot_Pattern_Pump
{
	public:
		Shoot_Pattern_Pump() {}
	~Shoot_Pattern_Pump() {}
	inline static Shoot_Pattern_Pump* instance;

	inline static Shoot_Pattern_Pump& GetInstance() {
		if (instance == nullptr)
		{
			instance = new Shoot_Pattern_Pump();
		}
		return *instance;
	}
	void Update(float _deltaTime);
	void Reset();
	static void SetPlayerIndex(int index) { instance->mPlayerIndex = index; }
	int mPlayerIndex = -1;
	static Shot* Shoot(Entity sender, float bullets, float _damage, Window* window, float scale);
};

class Shoot_Pattern_Line
{
public:
	Shoot_Pattern_Line() {}
	~Shoot_Pattern_Line() {}
	inline static Shoot_Pattern_Line* instance;

	inline static Shoot_Pattern_Line& GetInstance() {
		if (instance == nullptr)
		{
			instance = new Shoot_Pattern_Line();
		}
		return *instance;
	}
	void Update(float _deltaTime);
	void Reset();
	static void SetPlayerIndex(int index) { instance->mPlayerIndex = index; }
	int mPlayerIndex = -1;
	static Bullet* Shoot(Entity sender, float _damage, float range, float width, Window* window, float scale);
};