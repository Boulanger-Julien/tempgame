#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class SceneData
{
public:
	SceneData() = default;
	~SceneData() = default;

	void SetCamera(const Camera& cam);
	void SetLight(const Light& light);
	
	const Camera& GetCamera() const { return mCamera; }
	const Light& GetLight() const { return mLight; }
	
	XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&mView); }
	XMMATRIX GetProjMatrix() const { return XMLoadFloat4x4(&mProj); }
	
	LightConstants GetLightConstants() const;

private:
	Camera mCamera;
	Light mLight;
	XMFLOAT4X4 mView = MathsUtils::Identity4x4();
	XMFLOAT4X4 mProj = MathsUtils::Identity4x4();
};