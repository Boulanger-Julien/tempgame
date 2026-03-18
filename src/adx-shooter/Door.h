#pragma once
#include "TeleportComponent.h"
#include "Player.h"

class Door
{
public:
	Door();
	~Door();

	void Update();
	transformComponent mTransform;
	Entity m_entity;
	
	ColliderComponent mCollider;
	TeleportComponent mTeleport;
	MeshGeometry doorMesh;
protected:
};

