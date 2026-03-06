#pragma once
#include "../adx-engine/transform.h"

struct adx_aabb
{
    FLOAT3 min;
    FLOAT3 max;

    bool Contains(FLOAT3 pos);
    bool Intersects(const adx_aabb& other) const;
};
