#pragma once
#include "TeleportComponent.h"
#include "Player.h"
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "MeshCreator.h" 
#include "Enemy\Enemy.h"

class Door
{
public:
	Door();
	~Door();
	void Initialize(Window* _window);
	void Update(int _enemyleft);
	void UpdateComponent();
	transformComponent mTransform;
	Entity mEntity;
	
	ColliderComponent mCollider;
	TeleportComponent mTeleport;
	MeshGeometry doorMesh;
	bool changeRoom = false;
	Entity mPlayer;
protected:
};

