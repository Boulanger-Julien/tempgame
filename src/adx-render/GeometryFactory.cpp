#include "pch.h"
#include "GeometryFactory.h"
#include <random>


void GeometryFactory::ComputeNormals(std::vector<Vertex>& verts, const std::vector<std::uint16_t>& inds)
{
    for (Vertex& v : verts)
        v.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

    for (size_t i = 0; i + 2 < inds.size(); i += 3)
    {
        uint16_t i0 = inds[i + 0];
        uint16_t i1 = inds[i + 1];
        uint16_t i2 = inds[i + 2];

        XMVECTOR p0 = XMLoadFloat3(&verts[i0].Pos);
        XMVECTOR p1 = XMLoadFloat3(&verts[i1].Pos);
        XMVECTOR p2 = XMLoadFloat3(&verts[i2].Pos);

        XMVECTOR e0 = XMVectorSubtract(p1, p0);
        XMVECTOR e1 = XMVectorSubtract(p2, p0);
        XMVECTOR n = XMVector3Cross(e0, e1);

        XMFLOAT3 fn;
        XMStoreFloat3(&fn, n);

        verts[i0].Normal.x += fn.x; verts[i0].Normal.y += fn.y; verts[i0].Normal.z += fn.z;
        verts[i1].Normal.x += fn.x; verts[i1].Normal.y += fn.y; verts[i1].Normal.z += fn.z;
        verts[i2].Normal.x += fn.x; verts[i2].Normal.y += fn.y; verts[i2].Normal.z += fn.z;
    }

    for (Vertex& v : verts)
    {
        XMVECTOR n = XMLoadFloat3(&v.Normal);
        if (XMVector3Equal(n, XMVectorZero()))
        {
            v.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        }
        else
        {
            n = XMVector3Normalize(n);
            XMFLOAT3 nn; XMStoreFloat3(&nn, n);
            v.Normal = nn;
        }
    }
}

void GeometryFactory::Init(ID3D12Device* _device) {
    mDevice = _device;
}

Mesh GeometryFactory::CreateGeoCube()
{
    Mesh mesh;

    float w2 = 1;
    float h2 = 1;
    float d2 = 1;

    std::vector<std::uint16_t> indices;
    std::vector<Vertex> vertices;

    XMFLOAT3 corner[8] = {
        {-w2, -h2, -d2}, { -w2, +h2, -d2 }, { +w2, +h2, -d2 }, { +w2, -h2, -d2 },
        {-w2, -h2, +d2}, { -w2, +h2, +d2 }, { +w2, +h2, +d2 }, { +w2, -h2, +d2 }
    };

    int faceIndices[6][4] = {
        {0, 1, 2, 3},
        {7, 6, 5, 4},
        {4, 5, 1, 0},
        {3, 2, 6, 7},
        {1, 5, 6, 2},
        {4, 0, 3, 7}
    };

    XMFLOAT3 normals[6] = {
        {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,1,0}, {0,-1,0}
    };

    XMFLOAT2 uv[4] = { {0,1}, {0,0}, {1,0}, {1,1} };
    XMFLOAT4 color = { 1,0,0,1 };
    for (int i = 0; i < 6; ++i)
    {
        unsigned int baseIndex = (unsigned int)vertices.size();

        for (int j = 0; j < 4; ++j)
        {
            int cornerIndex = faceIndices[i][j];
            vertices.push_back({ corner[cornerIndex], color, normals[i], uv[j] });
        }

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    ComputeNormals(vertices, indices);

    // Création du mesh
    mesh.Create(mDevice, vertices, indices);

    return mesh;
}
Mesh GeometryFactory::CreateGeoBall() {
    Mesh mesh;

    float radius = 1;
    float sliceCount = 10;
    float stackCount = 10;


    std::vector<Vertex> vertices;
    std::vector<std::uint16_t> indices;

    XMFLOAT2 uv[4] = { {0,1}, {0,0}, {1,0}, {1,1} };
    XMFLOAT4 color = { 1,0,0,1 };
    for (int i = 0; i <= stackCount; ++i)
    {
        float phi = XM_PI * i / stackCount;
        for (int j = 0; j <= sliceCount; ++j)
        {
            float theta = XM_2PI * j / sliceCount;
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);
            vertices.push_back({ XMFLOAT3(x, y, z), color, XMFLOAT3(0,1,0) });
        }
    }

    for (int i = 0; i < stackCount; ++i)
    {
        for (int j = 0; j < sliceCount; ++j)
        {
            int first = i * (sliceCount + 1) + j;
            int second = first + sliceCount + 1;
            indices.push_back(first + 1);
            indices.push_back(second);
            indices.push_back(first);
            indices.push_back(second + 1);
            indices.push_back(second);
            indices.push_back(first + 1);
        }
    }

    //ComputeNormals(vertices, indices);

    mesh.Create(mDevice, vertices, indices);

    return mesh;
}