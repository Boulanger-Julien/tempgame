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
	Choc,
	Thunder,
	Boomerang,
	Bomb,
	Amount
};



class Shoot_Pattern_Single_Shot
{
public:
	static Bullet* Shoot(Entity sender, float scale, float speedBullet, float accuracy, int damage);
};

class Shoot_Pattern_Explosion
{
public:
	static Shot* Shoot(Entity sender, float bullets, float _damage, float scale, float speedBullet);
};

class Shoot_Pattern_Pump
{
public:
	static Shot* Shoot(Entity sender, float bullets, float _damage, float scale, float speedBullet);
};

class Shoot_Pattern_Line
{
public:
	static Bullet* Shoot(Entity sender, float _damage, float range, float width);
};

class Shoot_Pattern_Thunder
{
public:
	static Shot* Shoot(Entity sender, float _damage, FLOAT3 range);
};

// VOID / BOOMERANG / BOMB / CHOC