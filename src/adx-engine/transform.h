#pragma once
#include <DirectXMath.h>


struct FLOAT4
{
	FLOAT4(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {}
	FLOAT4(DirectX::XMFLOAT4 f) : x(f.x), y(f.y), z(f.z), w(f.w) {}
	FLOAT4 operator*(float scalar) const
	{
		return FLOAT4(x * scalar, y * scalar, z * scalar, w * scalar);
	}
	FLOAT4 operator+(FLOAT4 float4) const
	{
		return FLOAT4(x + float4.x, y + float4.y, z + float4.z, w + float4.w);
	}
	FLOAT4 operator-(FLOAT4 float4) const
	{
		return FLOAT4(x - float4.x, y - float4.y, z - float4.z, w - float4.w);
	}
	FLOAT4 operator/(float scalar) const
	{
		return FLOAT4(x / scalar, y / scalar, z / scalar, w / scalar);
	}
	float x;
	float y;
	float z;
	float w;
};
struct FLOAT3
{
	FLOAT3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
	FLOAT3(DirectX::XMFLOAT3 f) : x(f.x), y(f.y), z(f.z) {}

	FLOAT3 operator*(float scalar) const
	{
		return FLOAT3(x * scalar, y * scalar, z * scalar);
	}
	FLOAT3 operator*=(float scalar) const
	{
		return FLOAT3(x * scalar, y * scalar, z * scalar);
	}
	FLOAT3 operator*(FLOAT3 float3) const
	{
		return FLOAT3(x * float3.x, y * float3.y, z * float3.z);
	}
	FLOAT3 operator+(FLOAT3 float3) const
	{
		return FLOAT3(x + float3.x, y + float3.y, z + float3.z);
	}
	FLOAT3 operator+=(FLOAT3 float3)
	{
		x += float3.x;
		y += float3.y;
		z += float3.z;
		return FLOAT3(x, y, z);
	}
	FLOAT3 operator-(FLOAT3 float3) const
	{
		return FLOAT3(x - float3.x, y - float3.y, z - float3.z);
	}
	FLOAT3 operator/(float scalar) const
	{
		return FLOAT3(x / scalar, y / scalar, z / scalar);
	}
	float x;
	float y;
	float z;
};
struct FLOAT2
{
	FLOAT2(float x = 0, float y = 0) : x(x), y(y) {}
	FLOAT2(DirectX::XMFLOAT2 f) : x(f.x), y(f.y) {}
	FLOAT2 operator*(float scalar) const
	{
		return FLOAT2(x * scalar, y * scalar);
	}
	FLOAT2 operator+(FLOAT2 float2) const
	{
		return FLOAT2(x + float2.x, y + float2.y);
	}
	FLOAT2 operator-(FLOAT2 float2) const
	{
		return FLOAT2(x - float2.x, y - float2.y);
	}
	FLOAT2 operator/(float scalar) const
	{
		return FLOAT2(x / scalar, y / scalar);
	}
	float x;
	float y;
};
inline DirectX::XMFLOAT3 toXMFLOAT3(FLOAT3 f)
{
	return DirectX::XMFLOAT3(f.x, f.y, f.z);
}
inline FLOAT3 toFLOAT3(DirectX::XMFLOAT3 f)
{
	return FLOAT3{ f.x, f.y, f.z };
}

struct transformComponent
{
	transformComponent(float px = 0, float py = 0, float pz = 0, float sx = 1, float sy = 1, float sz = 1) : 
		position(px, py, pz), scale(sx, sy, sz), rotation(0, 0, 0), forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0) {
	}
	// position
	FLOAT3 position;

	// scale
	FLOAT3 scale;

	// rotation
	FLOAT3 rotation;
	FLOAT3 forward;
	FLOAT3 right;
	FLOAT3 up;
};

struct transformSystem
{
	static void Move(transformComponent& transform, float x = 0, float y = 0, float z = 0);
	static void MoveByKey(transformComponent& transform, float velo, float yAngle = 0, float deltaTime = 1);
	static void MoveForward(transformComponent& transform, float distance = 1);
	static void UpdateForward(transformComponent& transform);
	//
	static void SetYPR(transformComponent& transform, float x = 0, float y = 0, float z = 0);
	static void Rotate(transformComponent& transform, float x = 0, float y = 0, float z = 0);
	static DirectX::XMMATRIX GetWorldMatrix(transformComponent& transform);
	static void RotateAround(transformComponent& transform, transformComponent target, float radius);
	static void LookAt(transformComponent& transform, FLOAT3 target);
	static float GetDistance(transformComponent& target1, transformComponent& target2);
};

#include <d2d1.h>