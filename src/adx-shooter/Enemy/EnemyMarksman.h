#pragma once
#include "adx-engine/framework.h"
#include "Enemy.h"
class EnemyMarksman : public Enemy
{
public:

	void OnInit() override;
	void OnUpdate(float _deltaTime) override;

	bool Test();
protected: 

};

