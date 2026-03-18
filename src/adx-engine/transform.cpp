#include "pch.h"
#include <algorithm>
#include <cmath>

using namespace DirectX;


void transformSystem::Move(transformComponent& transform, float x, float y, float z)
{
	transform.position.x += z * transform.forward.x + x * transform.right.x + y * transform.up.x;
	transform.position.y += z * transform.forward.y + x * transform.right.y + y * transform.up.y;
	transform.position.z += z * transform.forward.z + x * transform.right.z + y * transform.up.z;
	return;
}

void transformSystem::MoveByKey(transformComponent& transform,float velo, float yAngle, float deltaTime)
{
	// On convertit l'angle de degrés en radians si nécessaire
	// float rad = angle.y * (3.14159f / 180.0f); 
	float rad = yAngle * XM_PI / 180.0f;

	// Calcul des vecteurs forward (Z) et right (X)
	float forwardX = sin(rad) * deltaTime * velo;
	float forwardZ = cos(rad) * deltaTime * velo;

	float rightX = cos(rad) * deltaTime * velo;
	float rightZ = -sin(rad) * deltaTime * velo;

	// Z / S : Avancer / Reculer
	if (InputSystem::isKeyDown('Z'))
		Move(transform, forwardX, 0, forwardZ);
	if (InputSystem::isKeyDown('S'))
		Move(transform, -forwardX, 0, -forwardZ);

	// Q / D : Strafe Gauche / Droite
	if (InputSystem::isKeyDown('Q'))
		Move(transform, -rightX, 0, -rightZ);
	if (InputSystem::isKeyDown('D'))
		Move(transform, rightX, 0, rightZ);

	return;
}

void transformSystem::MoveForward(transformComponent& transform, float distance) {
	transform.position.x += distance * transform.forward.x;
	transform.position.y += distance * transform.forward.y;
	transform.position.z += distance * transform.forward.z;
}

void transformSystem::SetYPR(transformComponent& transform, float x, float y, float z)
{
	transform.rotation.x = XMConvertToRadians(x);
	transform.rotation.y = XMConvertToRadians(y);
	transform.rotation.z = XMConvertToRadians(z);
	XMMATRIX mRotation = XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	XMFLOAT3 right = toXMFLOAT3(transform.right);
	XMFLOAT3 up = toXMFLOAT3(transform.up);
	XMFLOAT3 forward = toXMFLOAT3(transform.forward);
	XMStoreFloat3(&right, XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), mRotation));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), mRotation));
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), mRotation));
	return;
}

void transformSystem::Rotate(transformComponent& transform, float x, float y, float z)
{
	transform.rotation.x += XMConvertToRadians(x);
	transform.rotation.y += XMConvertToRadians(y);
	transform.rotation.z += XMConvertToRadians(z);
	return;
}

XMMATRIX transformSystem::GetWorldMatrix(transformComponent& transform)
{
	XMMATRIX mScale = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);

	XMMATRIX mRotation = XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	XMMATRIX mTranslation = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

	XMMATRIX world = mScale * mRotation * mTranslation;

	return world;
}

void transformSystem::RotateAround(transformComponent& transform, transformComponent target, float radius)
{
    const float eps = 1e-4f;
    const float maxPitch = XM_PIDIV2 - eps;
    target.rotation.x = std::clamp(target.rotation.x, -maxPitch, maxPitch);

    const float twoPi = XM_PI * 2.0f;
    float yaw = std::fmod(target.rotation.y + XM_PI, twoPi);
    if (yaw < 0.0f) yaw += twoPi;
    yaw -= XM_PI;
    target.rotation.y = yaw;

    float cp = cosf(target.rotation.x); 
    float sp = sinf(target.rotation.x);
    float sy = sinf(target.rotation.y); 
    float cy = cosf(target.rotation.y);

    transform.position.x = target.position.x + radius * cp * sy;
    transform.position.y = target.position.y - radius * sp;
    transform.position.z = target.position.z + radius * cp * cy;
}

void transformSystem::LookAt(transformComponent& transform, FLOAT3 target)
{
	FLOAT3 direction = target - transform.position;
	float yaw = atan2f(direction.x, direction.z);
	float pitch = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
	transform.rotation.x = pitch;
	transform.rotation.y = yaw;
}

void transformSystem::UpdateForward(transformComponent& transform)
{
	// Récupère un quaternion grâce au vecteur de rotation (roll, pitch, yaw)
	 XMVECTOR quat = XMQuaternionRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	// XMVector3Rotate(Axe du forward(0,0,1), quaternion);
	
	 XMVECTOR fwVect = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), quat);
	
	// normalize le vector -> forward
	XMVECTOR normVect = XMVector3Normalize(fwVect);

	transform.forward.x = -XMVectorGetX(normVect);
	transform.forward.y = -XMVectorGetY(normVect);
	transform.forward.z = -XMVectorGetZ(normVect);
}
