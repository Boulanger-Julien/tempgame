#include "pch.h"
#include "Shoot_Pattern.h"
//include file for max
#include <algorithm>
void Shoot_Pattern_Single_Shot::Update(float _deltaTime)
{
}

Bullet* Shoot_Pattern_Single_Shot::Shoot(Entity sender, float scale, float speedBullet, float accuracy)
{
    
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));
    transformComponent senderTrans = ECS::GetInstance().getComponent<transformComponent>(sender);
    Bullet* newBullet = new Bullet();
    newBullet->mTransform = senderTrans;
	newBullet->mTransform.scale = FLOAT3(1,1,1) * scale;
    newBullet->mCollider.updateCollider();
	newBullet->m_speed = speedBullet;
	float signe = (rand() % 2 == 0) ? -1.0f : 1.0f;
    if (accuracy < 100)
    {
		int minOffset = 100.0f - accuracy;
        float randomOffset = rand() % minOffset;
		accuracy += randomOffset;


        newBullet->mTransform.rotation.y = senderTrans.rotation.y + (signe * XM_PI / 2) * ((100 / accuracy)-1);
    }
    transformSystem::RotateAround(newBullet->mTransform, ECS::GetInstance().getComponent<transformComponent>(sender), newBullet->mTransform.scale.z *2);
    ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
    ECS::GetInstance().getComponent<ColliderComponent>(newBullet->mEntity) = newBullet->mCollider;
    transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);
    bulletTrans.forward = bulletTrans.forward * -1;
    newBullet->isPersistantBullet = false;
    return newBullet;
}

void Shoot_Pattern_Single_Shot::Reset()
{
}

Shot* Shoot_Pattern_Explosion::Shoot(Entity sender, float bullets, float _damage, Window* window, float scale, float speedBullet)
{
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));
    transformComponent& senderTrans = ECS::GetInstance().getComponent<transformComponent>(sender);

    const float angleStep = (2.0f * XM_PI) / bullets; 
	Shot* newShot = new Shot();
	float accuracy = 75.0f;
    int minOffset = 100.0f - accuracy;
    float randomOffset = rand() % minOffset;

    for (int i = 0; i < bullets; ++i) {
        Bullet* newBullet = new Bullet();
        newBullet->mTransform = senderTrans;
        newBullet->mTransform.scale = FLOAT3(1, 1, 1) * scale;

        newBullet->mCollider.updateCollider();

		newBullet->m_speed = speedBullet;
        float newAccuracy = accuracy;
        newAccuracy += randomOffset;

        newBullet->mTransform.rotation.y = senderTrans.rotation.y + (XM_PI / 2) * ((100 / newAccuracy) - 1)+ (i * angleStep);

        transformSystem::UpdateForward(newBullet->mTransform);

        float distFromPlayer = 5.0f;
        transformSystem::MoveForward(newBullet->mTransform, distFromPlayer);

        transformSystem::Move(newBullet->mTransform, 0, 0, 2);

        window->RegisterExistingMeshForEntity(newBullet->mEntity);

        XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(newBullet->mTransform);
        window->Update(newBullet->mEntity, bulletWorld);

        newBullet->mDamage = _damage;
		newShot->bulletList.push_back(newBullet);
	    ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
    }
    return newShot;
}

Shot* Shoot_Pattern_Pump::Shoot(Entity sender, float bullets, float _damage, Window* window, float scale,float speedBullet)
{
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));
    transformComponent& senderTrans = ECS::GetInstance().getComponent<transformComponent>(sender);
    const float angleStep = (XM_PI / 4) / bullets; 
    Shot* newShot = new Shot();
    float accuracy = 20.0f;
    int minOffset = 100.0f - accuracy;
    int sign = 1;
	int depart = 0;
    if ((int)bullets % 2 == 0) {
        depart = 1;
    }
    for (int i = 0; i < bullets; ++i) {
        int valeurAbsolue = (i + depart + 1) / 2;

        sign = (i % 2 == 0) ? 1 : -1;

        Bullet* newBullet = new Bullet();
		newBullet->type = BulletType::SHOTGUN;
        newBullet->mTransform = senderTrans;
        newBullet->mTransform.scale = FLOAT3(1, 1, 1) * scale;
        newBullet->mCollider.updateCollider();
        newBullet->m_speed = speedBullet;
        float newAccuracy = accuracy;
        float randomOffset = rand() % minOffset;

        newAccuracy += randomOffset;
        newBullet->mTransform.rotation.y = senderTrans.rotation.y + /*(XM_PI / 2)*/  + (valeurAbsolue* sign * angleStep* ((100 / newAccuracy) - 1));
        transformSystem::UpdateForward(newBullet->mTransform);
        float distFromPlayer = 5.0f;
        transformSystem::MoveForward(newBullet->mTransform, distFromPlayer);
        transformSystem::Move(newBullet->mTransform, 0, 0, 2);
        window->RegisterExistingMeshForEntity(newBullet->mEntity);
        XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(newBullet->mTransform);
        window->Update(newBullet->mEntity, bulletWorld);
        newBullet->mDamage = _damage/bullets;
        newShot->bulletList.push_back(newBullet);
        ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
    }
	return newShot;
}

Bullet* Shoot_Pattern_Line::Shoot(Entity sender, float _damage, float range, float width, Window* window)
{
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));
	transformComponent senderTrans = ECS::GetInstance().getComponent<transformComponent>(sender);
    Bullet* newBullet = new Bullet();
	newBullet->mTransform = senderTrans;
	newBullet->mDamage = _damage/1.5f;
	newBullet->maxLifetime = 0.3f;
	newBullet->m_speed = 0;
	newBullet->mTransform.scale = FLOAT3(width, 0.5f, range);
	newBullet->mCollider.updateCollider();
    newBullet->type = BulletType::LINE;
	transformSystem::RotateAround(newBullet->mTransform, ECS::GetInstance().getComponent<transformComponent>(sender), newBullet->mTransform.scale.z / 2);
	ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(newBullet->mEntity) = newBullet->mCollider;
    transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);
    bulletTrans.forward = bulletTrans.forward * -1;
	newBullet->isPersistantBullet = true;
    return newBullet;
}