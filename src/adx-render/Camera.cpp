#include "Camera.h"
#include <DirectXMath.h>
#include "pch.h"

using namespace DirectX;

Camera::Camera()
{
    XMStoreFloat4x4(&mView, XMMatrixIdentity());
    XMStoreFloat4x4(&mProj, XMMatrixIdentity());
}

void Camera::SetPosition(float x, float y, float z)
{
    mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(pos.x, pos.y, pos.z);
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
    mFovY = fovY;
    mAspect = aspect;
    mNearZ = zn;
    mFarZ = zf;

    XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
    XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up)
{
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(up, XMVectorSubtract(target, pos)));
    XMVECTOR U = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(target, pos), R));
    XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));

    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mRight), R);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mUp), U);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mLook), L);

    XMStoreFloat3(&mPosition, pos);

    UpdateViewMatrix();
}

void Camera::Strafe(float d)
{
    XMVECTOR s = XMLoadFloat3(&mRight) * d;
    XMVECTOR p = XMLoadFloat3(&mPosition) + s;
    XMStoreFloat3(&mPosition, p);
}

void Camera::Walk(float d)
{
    XMVECTOR s = XMLoadFloat3(&mLook) * d;
    XMVECTOR p = XMLoadFloat3(&mPosition) + s;
    XMStoreFloat3(&mPosition, p);
}

void Camera::Pitch(float angle)
{
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
    XMVECTOR u = XMVector3TransformNormal(XMLoadFloat3(&mUp), R);
    XMVECTOR l = XMVector3TransformNormal(XMLoadFloat3(&mLook), R);
    XMStoreFloat3(&mUp, u);
    XMStoreFloat3(&mLook, l);
}

void Camera::RotateY(float angle)
{
    XMMATRIX R = XMMatrixRotationY(angle);
    XMVECTOR r = XMVector3TransformNormal(XMLoadFloat3(&mRight), R);
    XMVECTOR u = XMVector3TransformNormal(XMLoadFloat3(&mUp), R);
    XMVECTOR l = XMVector3TransformNormal(XMLoadFloat3(&mLook), R);
    XMStoreFloat3(&mRight, r);
    XMStoreFloat3(&mUp, u);
    XMStoreFloat3(&mLook, l);
}

void Camera::UpdateViewMatrix()
{
    XMVECTOR R = XMLoadFloat3(&mRight);
    XMVECTOR U = XMLoadFloat3(&mUp);
    XMVECTOR L = XMLoadFloat3(&mLook);
    XMVECTOR P = XMLoadFloat3(&mPosition);

    // ensure the vectors are orthonormal
    L = XMVector3Normalize(L);
    U = XMVector3Normalize(XMVector3Cross(L, R));
    R = XMVector3Cross(U, L);

    float x = -XMVectorGetX(XMVector3Dot(P, R));
    float y = -XMVectorGetX(XMVector3Dot(P, U));
    float z = -XMVectorGetX(XMVector3Dot(P, L));

    XMFLOAT4X4 V;
    XMStoreFloat4x4(&V, XMMATRIX(
        R.m128_f32[0], U.m128_f32[0], L.m128_f32[0], 0.0f,
        R.m128_f32[1], U.m128_f32[1], L.m128_f32[1], 0.0f,
        R.m128_f32[2], U.m128_f32[2], L.m128_f32[2], 0.0f,
        x, y, z, 1.0f
    ));

    mView = V;
}

transformComponent Camera::GetTransform()
{
    transformComponent transform;
    transform.position = mPosition;
    transform.right = mRight;
    transform.up = mUp;
    transform.forward = mLook;
    return transform;
}

Ray Camera::GetRayFromMouse(float mouseX, float mouseY, float screenWidth, float screenHeight) const
{
    float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;

    XMMATRIX view = View();
    XMMATRIX proj = Proj();
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, XMMatrixMultiply(view, proj));

    // 2. Points dans l'espace NDC
    XMVECTOR nearPoint = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    XMVECTOR farPoint = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    // 3. Unproject vers l'espace Monde
    XMVECTOR worldNear = XMVector3TransformCoord(nearPoint, invViewProj);
    XMVECTOR worldFar = XMVector3TransformCoord(farPoint, invViewProj);

    XMVECTOR rayDir = XMVector3Normalize(XMVectorSubtract(worldFar, worldNear));

    return { worldNear, rayDir };
}
