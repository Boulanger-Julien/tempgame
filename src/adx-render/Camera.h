#pragma once
#include "../adx-engine/transform.h"
#include <DirectXMath.h>

using namespace DirectX;

class Camera {
public:
    Camera();

    void SetPosition(float x, float y, float z);
    void SetPosition(const XMFLOAT3& pos);
    XMFLOAT3 Position() const noexcept { return mPosition; }

    void SetLens(float fovY, float aspect, float zn, float zf);
    void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    void Strafe(float d);
    void Walk(float d);
    void Pitch(float angle);
    void RotateY(float angle);

    void UpdateViewMatrix();

    transformComponent GetTransform();

    XMMATRIX View() const { return XMLoadFloat4x4(&mView); }
    XMMATRIX Proj() const { return XMLoadFloat4x4(&mProj); }

private:
    XMFLOAT3 mPosition{ 0.0f, 0.0f, 0.0f };
    XMFLOAT3 mRight{ 1.0f, 0.0f, 0.0f };
    XMFLOAT3 mUp{ 0.0f, 1.0f, 0.0f };
    XMFLOAT3 mLook{ 0.0f, 0.0f, 1.0f };

    XMFLOAT4X4 mView{};
    XMFLOAT4X4 mProj{};

    // lens params
    float mFovY{ 0.25f * XM_PI };
    float mAspect{ 1.0f };
    float mNearZ{ 1.0f };
    float mFarZ{ 1000.0f };
};