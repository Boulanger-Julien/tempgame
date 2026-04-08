#pragma once
#include "Player.h"
#include "adx-render/framework.h"
#include "adx-engine/framework.h"
#include "MeshCreator.h" 

enum RoomType;

class Totem
{
public:
	Totem();
	~Totem();
	void Initialize(Window* _window);
	void Update(RoomType _enemyleft);
	void UpdateComponent();
	transformComponent mTransform;
	Entity mEntityfeur;

	ColliderComponent mCollider;
	MeshGeometry TotemMesh;
	Entity mPlayer;
protected:

};

