#include "pch.h"
#include "Camera2.h"
Camera2::Camera2() {

}
Camera2::~Camera2() {

}

void Camera2::SetLens(float _fovAngleY, float _aspectRatio, float _nearZ, float _farZ)
{
    mFovY = _fovAngleY;
    mAspect = _aspectRatio;
    mNearZ = _nearZ;
    mFarZ = _farZ;

    mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
    mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);
    DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
    XMStoreFloat4x4(&mProj, projection);
}

void Camera2::SetViewMatrix()
{
    DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&mPosition);
    DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&mLookAt);
    DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&mUp);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
        position,
        lookAt,
        up
    );

    XMStoreFloat4x4(&mView, view);
}

void Camera2::UpdateViewMatrix()
{
    DirectX::XMVECTOR R = DirectX::XMLoadFloat3(&mRight);
    DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&mUp);
    DirectX::XMVECTOR L = DirectX::XMLoadFloat3(&mLookAt);
    DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&mPosition);

    // Keep camera's axes orthogonal to each other and of unit length.
    L = DirectX::XMVector3Normalize(L);
    U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

    // U, L already ortho-normal, so no need to normalize cross product.
    R = DirectX::XMVector3Cross(U, L);
    // Fill in the view matrix entries.
    float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
    float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
    float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

    DirectX::XMStoreFloat3(&mRight, R);
    DirectX::XMStoreFloat3(&mUp, U);
    DirectX::XMStoreFloat3(&mLookAt, L);

    mView(0, 0) = mRight.x;
    mView(1, 0) = mRight.y;
    mView(2, 0) = mRight.z;
    mView(3, 0) = x;

    mView(0, 1) = mUp.x;
    mView(1, 1) = mUp.y;
    mView(2, 1) = mUp.z;
    mView(3, 1) = y;

    mView(0, 2) = mLookAt.x;
    mView(1, 2) = mLookAt.y;
    mView(2, 2) = mLookAt.z;
    mView(3, 2) = z;

    mView(0, 3) = 0;
    mView(1, 3) = 0;
    mView(2, 3) = 0;
    mView(3, 3) = 1.0f;
}

DirectX::XMMATRIX Camera2::GetProjMatrix()
{
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
    return proj;
}

DirectX::XMMATRIX Camera2::GetViewMatrix()
{
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
    return view;
}

void Camera2::SetPosition(float x, float y, float z)
{
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
}

void Camera2::Walk(float d)
{
    // mPosition += d*mLook
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&mLookAt);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
    XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, l, p));

    //mPosition.x += 1*mLookAt.x;
    UpdateViewMatrix();
}

void Camera2::Strafe(float d)
{
    // mPosition += d*mRight
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&mRight);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
    XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera2::Pitch(float angle)
{
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

    DirectX::XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
    DirectX::XMStoreFloat3(&mLookAt, XMVector3TransformNormal(DirectX::XMLoadFloat3(&mLookAt), R));
}

void Camera2::RotateY(float angle)
{
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

    DirectX::XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
    DirectX::XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
    DirectX::XMStoreFloat3(&mLookAt, XMVector3TransformNormal(DirectX::XMLoadFloat3(&mLookAt), R));
}
void Camera2::Update(float _deltaTime) {
    //moveCamera
    if (GetAsyncKeyState('Z') & 0x8000)
        Walk(10.0f * _deltaTime);
    if (GetAsyncKeyState('S') & 0x8000)
        Walk(-10.0f * _deltaTime);

    if (GetAsyncKeyState('Q') & 0x8000)
        Strafe(-10.0f * _deltaTime);
    if (GetAsyncKeyState('D') & 0x8000)
        Strafe(10.0f * _deltaTime);

    if (GetAsyncKeyState('A') & 0x8000)
        RotateY(-1.0f * _deltaTime);
    if (GetAsyncKeyState('E') & 0x8000)
        RotateY(1.0f * _deltaTime);

    if (GetAsyncKeyState('R') & 0x8000)
        Pitch(-1.0f * _deltaTime);
    if (GetAsyncKeyState('T') & 0x8000)
        Pitch(1.0f * _deltaTime);
}