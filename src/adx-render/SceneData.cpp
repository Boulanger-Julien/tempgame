#include "pch.h"
#include "SceneData.h"

void SceneData::SetCamera(const Camera& cam)
{
	mCamera = cam;
	XMStoreFloat4x4(&mView, mCamera.View());
	XMStoreFloat4x4(&mProj, mCamera.Proj());
}

void SceneData::SetLight(const Light& light)
{
	mLight = light;
}

LightConstants SceneData::GetLightConstants() const
{
	LightConstants lc;
	XMVECTOR dirVec = XMLoadFloat3(&mLight.direction);
	XMVECTOR lenVec = XMVector3Length(dirVec);
	const float EPS = 1e-6f;
	
	if (XMVectorGetX(lenVec) < EPS)
	{
		dirVec = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	}
	else
	{
		dirVec = XMVector3Normalize(dirVec);
	}

	XMFLOAT4 dir4;
	XMStoreFloat4(&dir4, dirVec);
	dir4.w = 0.0f;
	lc.Direction = dir4;

	lc.Strength = XMFLOAT3(mLight.strength, mLight.strength, mLight.strength);
	lc.Color = mLight.color;

	return lc;
}