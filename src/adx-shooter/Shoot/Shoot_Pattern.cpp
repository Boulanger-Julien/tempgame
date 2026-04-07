#include "pch.h"
#include "Shoot_Pattern.h"
//include file for max
#include <algorithm>


Bullet* Shoot_Pattern_Single_Shot::Shoot(Entity sender, float scale, float speedBullet, float accuracy, int damage)
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
	newBullet->mDamage = damage;
    return newBullet;
}

Shot* Shoot_Pattern_Explosion::Shoot(Entity sender, float bullets, float _damage, float scale, float speedBullet)
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

        newBullet->mDamage = _damage;
		newShot->bulletList.push_back(newBullet);
	    ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
    }
    return newShot;
}

Shot* Shoot_Pattern_Pump::Shoot(Entity sender, float bullets, float _damage, float scale,float speedBullet)
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
        newBullet->mTransform = senderTrans;
        newBullet->mTransform.scale = FLOAT3(1, 1, 1) * scale;
        newBullet->mCollider.updateCollider();
        newBullet->m_speed = speedBullet;
        float newAccuracy = accuracy;
        float randomOffset = rand() % minOffset;

        newAccuracy += randomOffset;
        newBullet->mTransform.rotation.y = senderTrans.rotation.y + (valeurAbsolue* sign * angleStep* ((100 / newAccuracy) - 1));
        transformSystem::UpdateForward(newBullet->mTransform);
        float distFromPlayer = 5.0f;
        transformSystem::MoveForward(newBullet->mTransform, distFromPlayer);
        transformSystem::Move(newBullet->mTransform, 0, 0, 2);
        newBullet->mDamage = (_damage / bullets)*2;
        newShot->bulletList.push_back(newBullet);
        ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
    }
	return newShot;
}

Bullet* Shoot_Pattern_Line::Shoot(Entity sender, float _damage, float range, float width)
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
	transformSystem::RotateAround(newBullet->mTransform, ECS::GetInstance().getComponent<transformComponent>(sender), newBullet->mTransform.scale.z / 2);
	ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(newBullet->mEntity) = newBullet->mCollider;
    transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);
    bulletTrans.forward = bulletTrans.forward * -1;
	newBullet->isPersistantBullet = true;
    return newBullet;
}

Shot* Shoot_Pattern_Thunder::Shoot(Entity sender, float _damage, FLOAT3 range)
{
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));

    Shot* newShot = new Shot();

    transformComponent& senderTrans = ECS::GetInstance().getComponent<transformComponent>(sender);
    Bullet* circleBullet = new Bullet();
    circleBullet->mTransform = senderTrans;
    circleBullet->mTransform.position = range;
    circleBullet->mDamage = 0;
    circleBullet->isPersistantBullet = true;
    circleBullet->maxLifetime = 1.0f;
    circleBullet->m_speed = 0;
    ECS::GetInstance().getComponent<transformComponent>(circleBullet->mEntity) = circleBullet->mTransform;
    ECS::GetInstance().getComponent<ColliderComponent>(circleBullet->mEntity) = circleBullet->mCollider;
	newShot->bulletList.push_back(circleBullet);
	Bullet* lightningBullet = new Bullet();
	lightningBullet->mTransform = senderTrans;
	lightningBullet->mTransform.position = range;
    lightningBullet->mTransform.position.y = 210;
    lightningBullet->mTransform.scale.z = 30;
	lightningBullet->mTransform.rotation = { -XM_PIDIV2, 0, 0 };
	lightningBullet->mDamage = _damage*2;
	lightningBullet->isPersistantBullet = true;
	lightningBullet->maxLifetime = 1.0f;
	lightningBullet->m_speed = 200;
	lightningBullet->mCollider.width = 10;
	lightningBullet->mCollider.height = 10;
    lightningBullet->mCollider.updateCollider();
	ECS::GetInstance().getComponent<transformComponent>(lightningBullet->mEntity) = lightningBullet->mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(lightningBullet->mEntity) = lightningBullet->mCollider;
	newShot->bulletList.push_back(lightningBullet);

    return newShot;
}