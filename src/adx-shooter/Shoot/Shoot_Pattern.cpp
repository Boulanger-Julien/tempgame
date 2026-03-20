#include "pch.h"
#include "Shoot_Pattern.h"

void Shoot_Pattern_Single_Shot::Update(float _deltaTime)
{
}

Bullet* Shoot_Pattern_Single_Shot::Shoot(Entity sender)
{
    transformSystem::UpdateForward(ECS::GetInstance().getComponent<transformComponent>(sender));

    Bullet* newBullet = new Bullet();

    ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = ECS::GetInstance().getComponent<transformComponent>(sender);
    if (sender == GetInstance().mPlayerIndex)
    {
        transformComponent& playerTrans = ECS::GetInstance().getComponent<transformComponent>(instance->mPlayerIndex);
        float pitch = playerTrans.rotation.x;
        float yaw = playerTrans.rotation.y;

        FLOAT3 forward = {
            sin(yaw) * cos(pitch),
            -sin(pitch),
            cos(yaw) * cos(pitch)
        };

        FLOAT3 right = { cos(yaw), 0, -sin(yaw) };
        ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity).position = playerTrans.position + (forward * 2.5f);
    }
    transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);
    bulletTrans.forward = bulletTrans.forward * -1;

    transformSystem::Move(ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity), 0, 0, 2);

	return newBullet;
}

void Shoot_Pattern_Single_Shot::Reset()
{
}

Shot* Shoot_Pattern_Explosion::Shoot(Entity sender, float bullets, float _damage, Window* window)
{
    const float angleStep = (2.0f * XM_PI) / bullets; // 360° / 8 en radians
    transformComponent& playerTrans = ECS::GetInstance().getComponent<transformComponent>(GetInstance().mPlayerIndex);
	Shot* newShot = new Shot();

    for (int i = 0; i < bullets; ++i) {
        Bullet* newBullet = new Bullet();
        transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);

        bulletTrans = playerTrans;

        bulletTrans.rotation.y = playerTrans.rotation.y + (i * angleStep);

        transformSystem::UpdateForward(bulletTrans);

        float distFromPlayer = 5.0f;
        transformSystem::MoveForward(bulletTrans, distFromPlayer);

        transformSystem::Move(bulletTrans, 0, 0, 2);

        window->RegisterExistingMeshForEntity(newBullet->mEntity);

        XMMATRIX bulletWorld = transformSystem::GetWorldMatrix(bulletTrans);
        window->Update(newBullet->mEntity, bulletWorld);

        newBullet->mDamage = _damage;
		newShot->bulletList.push_back(newBullet);
    }
    return newShot;
}

Shot* Shoot_Pattern_Pump::Shoot(Entity sender, float bullets, float _damage, Window* window)
{
	return nullptr;
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
	transformSystem::RotateAround(newBullet->mTransform, ECS::GetInstance().getComponent<transformComponent>(sender), newBullet->mTransform.scale.z / 2);
	ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity) = newBullet->mTransform;
	ECS::GetInstance().getComponent<ColliderComponent>(newBullet->mEntity) = newBullet->mCollider;
    transformComponent& bulletTrans = ECS::GetInstance().getComponent<transformComponent>(newBullet->mEntity);
    bulletTrans.forward = bulletTrans.forward * -1;
	newBullet->isLineBullet = true;
    return newBullet;
}