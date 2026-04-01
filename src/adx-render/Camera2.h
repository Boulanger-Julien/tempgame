#pragma once
#include <DirectXMath.h>
#include "Utils.h"

class Camera2
{
public:
	Camera2();
	~Camera2();

	void SetLens(float _fovAngleY, float _aspectRatio, float _nearZ, float _farZ);
	void SetViewMatrix();
	void UpdateViewMatrix();
	DirectX::XMMATRIX GetProjMatrix();
	DirectX::XMMATRIX GetViewMatrix();
	void SetPosition(float x, float y, float z);
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	//move camera
	void Walk(float d);
	void Strafe(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	void Update(float _deltaTime);

	XMFLOAT3 GetPosition() { return mPosition; }

private:
	// Camera coordinate system with coordinates relative to world space
	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLookAt = { 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };

	// Cache frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView = Utils::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = Utils::Identity4x4();
};

