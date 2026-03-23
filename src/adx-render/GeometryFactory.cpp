#include "pch.h"
#include "GeometryFactory.h"
#include <random>


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

void GeometryFactory::Init(ID3D12Device* _device) {
    mDevice = _device;
}
Mesh GeometryFactory::CreateGeoCube()
{
    Mesh mesh;

    // 24 vertices (4 par face)
    std::vector<Vertex> vertices = {
        // Front (+Z)
        {{-1,-1, 1},{1,0,0,1},{0,0,1},{0,1}},
        {{ 1,-1, 1},{0,1,0,1},{0,0,1},{1,1}},
        {{ 1, 1, 1},{0,0,1,1},{0,0,1},{1,0}},
        {{-1, 1, 1},{1,1,0,1},{0,0,1},{0,0}},

        // Back (-Z)
        {{-1,-1,-1},{1,0,1,1},{0,0,-1},{1,1}},
        {{-1, 1,-1},{0,1,1,1},{0,0,-1},{1,0}},
        {{ 1, 1,-1},{1,1,1,1},{0,0,-1},{0,0}},
        {{ 1,-1,-1},{0,0,0,1},{0,0,-1},{0,1}},

        // Left (-X)
        {{-1,-1,-1},{1,0,0,1},{-1,0,0},{0,1}},
        {{-1,-1, 1},{0,1,0,1},{-1,0,0},{1,1}},
        {{-1, 1, 1},{0,0,1,1},{-1,0,0},{1,0}},
        {{-1, 1,-1},{1,1,0,1},{-1,0,0},{0,0}},

        // Right (+X)
        {{ 1,-1,-1},{1,0,1,1},{1,0,0},{1,1}},
        {{ 1, 1,-1},{0,1,1,1},{1,0,0},{1,0}},
        {{ 1, 1, 1},{1,1,1,1},{1,0,0},{0,0}},
        {{ 1,-1, 1},{0,0,0,1},{1,0,0},{0,1}},

        // Top (+Y)
        {{-1, 1,-1},{1,0,0,1},{0,1,0},{0,1}},
        {{-1, 1, 1},{0,1,0,1},{0,1,0},{0,0}},
        {{ 1, 1, 1},{0,0,1,1},{0,1,0},{1,0}},
        {{ 1, 1,-1},{1,1,0,1},{0,1,0},{1,1}},

        // Bottom (-Y)
        {{-1,-1,-1},{1,0,1,1},{0,-1,0},{1,1}},
        {{ 1,-1,-1},{0,1,1,1},{0,-1,0},{0,1}},
        {{ 1,-1, 1},{1,1,1,1},{0,-1,0},{0,0}},
        {{-1,-1, 1},{0,0,0,1},{0,-1,0},{1,0}}
    };

    // 36 indices (6 faces, 2 triangles each)
    std::vector<uint32_t> indices = {
        0,1,2, 0,2,3,       // Front
        4,5,6, 4,6,7,       // Back
        8,9,10, 8,10,11,    // Left
        12,13,14, 12,14,15, // Right
        16,17,18, 16,18,19, // Top
        20,21,22, 20,22,23  // Bottom
    };

    // Création du mesh
    mesh.Create(mDevice, vertices, indices);

    return mesh;
}
/*
Mesh GeometryFactory::CreateGeoPyramide()
{
    Mesh mesh;
    std::array<Vertex, 5> pyramideVertices = {
        Vertex{{1,-1,1},{1,0,0}},
        Vertex{{-1, -1,1},{0,1,0}},
        Vertex{{ 0, 1,0},{1,1,0}},//
        Vertex{{ 1,-1,-1},{0,0,1}},
        Vertex{{-1,-1, -1},{1,0,1}},
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
                vertex.color = { 1,0,0 };
            if (height == 1)
                vertex.color = { 0,0,1 };
            if (height == 2)
                vertex.color = { 0,1,0 };
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