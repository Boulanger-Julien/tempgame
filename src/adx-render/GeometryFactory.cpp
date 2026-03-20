#include "pch.h"
#include "GeometryFactory.h"
#include <random>

void GeometryFactory::Init(ID3D12Device* _device) {
    mDevice = _device;
}

void GeometryFactory::ComputeNormals(std::vector<Vertex>& verts, const std::vector<std::uint32_t>& inds)
{
    for (Vertex& v : verts)
        v.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

    for (size_t i = 0; i + 2 < inds.size(); i += 3)
    {
        uint32_t i0 = inds[i + 0];
        uint32_t i1 = inds[i + 1];
        uint32_t i2 = inds[i + 2];

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

Mesh GeometryFactory::CreateGeoCube()
{
    Mesh mesh;
    std::vector<std::uint32_t> indices;
    std::vector<Vertex> vertices;

    XMFLOAT3 corner[8] = {
        {-1, -1, -1}, { -1, +1, -1 }, { +1, +1, -1 }, { +1, -1, -1 },
        {-1, -1, +1}, { -1, +1, +1 }, { +1, +1, +1 }, { +1, -1, +1 }
    };

    int faceIndices[6][4] = {
        {0, 1, 2, 3},
        {7, 6, 5, 4},
        {4, 5, 1, 0},
        {3, 2, 6, 7},
        {1, 5, 6, 2},
        {4, 0, 3, 7}
    };
    XMFLOAT4 color = { 1,0,0,1 };
    XMFLOAT3 normals[6] = {
        {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,1,0}, {0,-1,0}
    };

    XMFLOAT2 uv[4] = { {0,1}, {0,0}, {1,0}, {1,1} };

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

    mesh.Create(mDevice, vertices, indices);

    return mesh;
}
/*
Mesh GeometryFactory::CreateGeoPyramide()
{
    Mesh mesh;
    std::array<Vertex, 5> pyramideVertices = {
        Vertex{{1,-1,1},{0,0,1},{1,0,0,1}},
        Vertex{{-1, -1,1},{0,0,1},{0,1,0,1}},
        Vertex{{ 0, 1,0},{0,0,1},{1,1,0,1}},//
        Vertex{{ 1,-1,-1},{0,0,1},{0,0,1,1}},
        Vertex{{-1,-1, -1},{0,0,1},{1,0,1,1}},
    };

    std::array<uint16_t, 18> pyramideIndices = {
        1,0,2,
        2,0,3,
        2,3,4,
        2,4,1,
        0,1,3,
        4,3,1
    };

    std::vector<Vertex> pyrVerts(pyramideVertices.begin(), pyramideVertices.end());
    std::vector<uint16_t> pyrIndices(pyramideIndices.begin(), pyramideIndices.end());

    mesh.Create(mDevice, pyrVerts, pyrIndices);
    return mesh;
}

Mesh GeometryFactory::GenerateGrid(float _width, float _depth, uint32_t col, uint32_t row)
{
    Mesh mesh;

    int nbVertice = col * row;
    int nbIndice = nbVertice;
    float nbFace = (col - 1) * (row - 1);
    float nbTriangle = nbFace * 2;
    float dm = _width / col;
    float dn = _depth / row;

    std::vector<Vertex> gridVertices;

    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < row; j++)
        {
            float height = rand() % 3;
            Vertex vertex;
            if (i == 0 || i == col - 1 ||
                j == 0 || j == row - 1)
            {
                vertex.position = { i * dm, 0,j * dn };
            }
            else {
                vertex.position = { i * dm, height,j * dn };
            }

            if (height == 0)
                vertex.color = { 1,0,0 ,1 };
            if (height == 1)
                vertex.color = { 0,0,1 ,1 };
            if (height == 2)
                vertex.color = { 0,1,0 ,1 };
            gridVertices.push_back(vertex);
        }
    }

    std::vector<uint16_t> gridIndices;

    for (int j = 0; j < col - 1; j++)
    {
        for (int i = 0; i < row - 1; i++)
        {
            int vertexIndice = i + j * row;
            gridIndices.push_back(vertexIndice);
            gridIndices.push_back(vertexIndice + 1);
            gridIndices.push_back(vertexIndice + row);

            gridIndices.push_back(vertexIndice + 1);
            gridIndices.push_back(vertexIndice + row + 1);
            gridIndices.push_back(vertexIndice + row);
        }
    }

    std::vector<Vertex> grVerts(gridVertices.begin(), gridVertices.end());
    std::vector<uint16_t> grIndices(gridIndices.begin(), gridIndices.end());

    mesh.Create(mDevice, grVerts, grIndices);

    return mesh;
}
*/