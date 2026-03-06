#include "pch.h"
#include "adx_aabb.h"

bool adx_aabb::Contains(FLOAT3 pos) {
	return min.x <= pos.x && min.y <= pos.y && min.z <= pos.z && max.x >= pos.x && max.y >= pos.y && max.z >= pos.z;
}

bool adx_aabb::Intersects(const adx_aabb& other) const
{
	return (min.x <= other.max.x && max.x >= other.min.x) && (min.y <= other.max.y && max.y >= other.min.y) && (min.z <= other.max.z && max.z >= other.min.z);
}