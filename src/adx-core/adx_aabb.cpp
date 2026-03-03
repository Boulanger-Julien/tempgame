#include "pch.h"
#include "adx_aabb.h"

bool adx_aabb::Contains(FLOAT3 pos) {
	return min.x <= pos.x && min.y <= pos.y && min.z <= pos.z && max.x >= pos.x && max.y >= pos.y && max.z >= pos.z;
}