#include "pch.h"
#include "MeshCreator.h"
#include <cmath>
#include "json.hpp"
void MeshCreator::ComputeNormals(std::vector<Vertex>& verts, const std::vector<std::uint32_t>& inds)
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



MeshGeometry MeshCreator::CreateBox(Window* win, int index, float width, float height, float depth, XMFLOAT4 color, const wchar_t* filepath)
{
	float w2 = width * 0.5f;
	float h2 = height * 0.5f;
	float d2 = depth * 0.5f;

	std::vector<std::uint32_t> indices;
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

	for (int i = 0; i < 6; ++i)
	{
		uint32_t baseIndex = (uint32_t)vertices.size();

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
	return win->BuildMesh(vertices, indices, index, filepath);
}

Road MeshCreator::CreateRoad(Window* win, int index, float width, float length1, int totRoad, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
	std::vector<FLOAT2> points;
	int road[3] =
	{ 0, //straight
		1, //left curve
		-1 //right curve
	};
	int lastType = 0;
	int currentRoad = 0;
	int type = 0;
	float zPos = 0;
	float xPos = 0;
	for (int i = 0; i < totRoad; ++i) {
		int length = max((int)length1 / 2, rand() % (int)length1) * 2;
		int baseIndex = vertices.size();
		type = road[rand() % 3];
		while (lastType != 0 && type == lastType)
			type = road[rand() % 3];
		lastType = type;
		if (type == 0) // straight
		{
			if (currentRoad == 0)
			{
				vertices.push_back({ XMFLOAT3(xPos, 0, zPos + length), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(-width + xPos, 0, zPos + length), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos, 0, zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(-width + xPos, 0, zPos), color, XMFLOAT3(0,1,0) });
				points.push_back(FLOAT2(xPos - width / 2, zPos));
				points.push_back(FLOAT2(xPos - width / 2, zPos + length));

				zPos += length;
				currentRoad = 0;
			}
			else if (currentRoad == -1)
			{
				vertices.push_back({ XMFLOAT3(xPos - width, 0, zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos + length - width, 0, zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos - width, 0, width + zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos + length - width, 0, width + zPos), color, XMFLOAT3(0,1,0) });
				points.push_back(FLOAT2(xPos - width, zPos + width / 2));
				points.push_back(FLOAT2(xPos + length - width, zPos + width / 2));
				xPos += length;
				currentRoad = -1;
			}
			else if (currentRoad == 1)
			{
				vertices.push_back({ XMFLOAT3(xPos, 0, width + zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos - length, 0, width + zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos, 0, zPos), color, XMFLOAT3(0,1,0) });
				vertices.push_back({ XMFLOAT3(xPos - length, 0, zPos), color, XMFLOAT3(0,1,0) });
				points.push_back(FLOAT2(xPos, zPos + width / 2));
				points.push_back(FLOAT2(xPos - length / 2, zPos + width / 2));
				xPos -= length;
				currentRoad = 1;
			}

			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 3);
			indices.push_back(baseIndex + 1);
		}
		else if (type == 1) // left curve
		{
			baseIndex = vertices.size();
			if (currentRoad == 0)
			{
				vertices.push_back({ XMFLOAT3(xPos - width, 0, zPos), color, XMFLOAT3(0,1,0) });
				for (float i = 0; i < 9; ++i) {
					float fraction = i / 8;
					float theta = XM_PI * fraction / 2.0f;
					float x = width * cosf(theta) + xPos;
					float z = width * sinf(theta) + zPos;
					vertices.push_back({ DirectX::XMFLOAT3(x - width, 0, z), color, XMFLOAT3(0,1,0) });
				}
				for (int i = 0; i < 8; ++i) {
					points.push_back(FLOAT2(
						(vertices[baseIndex].Pos.x + vertices[i + 1 + baseIndex].Pos.x) / 2,
						(vertices[baseIndex].Pos.z + vertices[i + 1 + baseIndex].Pos.z) / 2
					));
				}
				for (int i = 0; i < 8; ++i) {
					indices.push_back((i + 1) % 9 + 1 + baseIndex);
					indices.push_back(i + 1 + baseIndex);
					indices.push_back(baseIndex);
				}

				xPos -= width;
				currentRoad = 1;
			}
			else if (currentRoad == -1)
			{
				vertices.push_back({ XMFLOAT3(xPos - width, 0, zPos + width), color, XMFLOAT3(0,1,0) });

				for (float i = 0; i < 9; ++i) {
					float fraction = i / 8;
					float theta = XM_PI * fraction / 2.0f;
					float x = width * cosf(theta) + xPos;
					float z = -width * sinf(theta) + zPos;
					vertices.push_back({ DirectX::XMFLOAT3(x - width, 0, z + width), color, XMFLOAT3(0,1,0) });
				}
				for (int i = static_cast<int>(vertices.size()) - 1; i >= static_cast<int>(vertices.size()) - 9; --i) {
					points.push_back(FLOAT2(
						(vertices[vertices.size() - 9].Pos.x + vertices[i].Pos.x - width) / 2,
						(vertices[vertices.size() - 9].Pos.z + vertices[i].Pos.z) / 2
					));
				}
				for (int i = 0; i < 8; ++i) {
					indices.push_back(baseIndex);
					indices.push_back(i + 1 + baseIndex);
					indices.push_back((i + 1) % 9 + 1 + baseIndex);
				}
				zPos += width;
				currentRoad = 0;
			}
		}
		else if (type == -1) // right curve
		{
			baseIndex = vertices.size();
			if (currentRoad == 0)
			{
				vertices.push_back({ XMFLOAT3(xPos, 0, zPos), color, XMFLOAT3(0,1,0) });

				for (float i = 0; i < 9; ++i) {
					float fraction = i / 8;
					float theta = XM_PI * fraction / 2.0f;
					float x = -width * cosf(theta) + xPos;
					float z = width * sinf(theta) + zPos;
					vertices.push_back({ DirectX::XMFLOAT3(x, 0, z), color, XMFLOAT3(0,1,0) });
				}
				for (int i = 0; i < 8; ++i) {
					points.push_back(FLOAT2(
						(vertices[baseIndex].Pos.x + vertices[i + 1 + baseIndex].Pos.x) / 2,
						(vertices[baseIndex].Pos.z + vertices[i + 1 + baseIndex].Pos.z) / 2
					));
				}
				for (int i = 0; i < 8; ++i) {
					indices.push_back(baseIndex);
					indices.push_back(i + 1 + baseIndex);
					indices.push_back((i + 1) % 9 + 1 + baseIndex);
				}
				xPos += width;
				currentRoad = -1;
			}
			else if (currentRoad == 1)
			{
				vertices.push_back({ XMFLOAT3(xPos, 0, zPos + width), color, XMFLOAT3(0,1,0) });

				for (float i = 0; i < 9; ++i) {
					float fraction = i / 8;
					float theta = XM_PI * fraction / 2.0f;
					float x = -width * cosf(theta) + xPos;
					float z = -width * sinf(theta) + zPos;
					vertices.push_back({ DirectX::XMFLOAT3(x, 0, z + width), color, XMFLOAT3(0,1,0) });
				}
				for (int i = static_cast<int>(vertices.size()) - 1; i >= static_cast<int>(vertices.size()) - 9; --i) {
					points.push_back(FLOAT2(
						(vertices[vertices.size() - 9].Pos.x + vertices[i].Pos.x + width) / 2,
						(vertices[vertices.size() - 9].Pos.z + vertices[i].Pos.z) / 2
					));
				}
				for (int i = 0; i < 8; ++i) {
					indices.push_back((i + 1) % 9 + 1 + baseIndex);
					indices.push_back(i + 1 + baseIndex);
					indices.push_back(baseIndex);
				}
				zPos += width;
				currentRoad = 0;
			}
		}
	}
	ComputeNormals(vertices, indices);

	return Road(FLOAT2(0, 0), FLOAT2(xPos, zPos), points, win->BuildMesh(vertices, indices, index, filepath));
}

MeshGeometry MeshCreator::CreateMount(Window* win, int index, int rows, int cols, int height, XMFLOAT4 colorTop, XMFLOAT4 colorBottom)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (int z = 0; z <= rows; ++z) {
		for (int x = 0; x <= cols; ++x) {
			float xPos = (float)x;
			float zPos = (float)z;
			float randomOffset = ((std::rand() % 100) / 100.0f - 0.5f);
			if (x > 0)
				if (vertices[z * cols + x - 1].Pos.y < 0.f) vertices[z * cols + x - 1].Pos.y = 0.f;
				else if (vertices[z * cols + x - 1].Pos.y < 0.2f || vertices[z * cols - 1].Pos.y < 0.2f) randomOffset *= 0.7f;
			float yPos = sinf(xPos * 0.5f) * cosf(zPos * 0.5f) + randomOffset;
			float t = fmaxf(0.f, fminf(1.f, (yPos + 0.5f) / 1.5f));
			if (z == 0)
			{
				yPos = 0.f;
				t = 0.f;
			}
			vertices.push_back({ XMFLOAT3(xPos, yPos * height, zPos),
				XMFLOAT4(colorBottom.x * (1 - t) + colorTop.x * t,
				colorBottom.y * (1 - t) + colorTop.y * t,
				colorBottom.z * (1 - t) + colorTop.z * t,
				1.0f), XMFLOAT3(0, 1, 0) });

		}
	}

	for (int z = 0; z < rows; ++z) {
		for (int x = 0; x < cols; ++x) {
			int start = z * (cols + 1) + x;
			indices.push_back(start);
			indices.push_back(start + cols + 1);
			indices.push_back(start + 1);
			indices.push_back(start + 1);
			indices.push_back(start + cols + 1);
			indices.push_back(start + cols + 2);
		}
	}
	ComputeNormals(vertices, indices);
	return win->BuildMesh(vertices, indices, index, nullptr);
}

MeshGeometry MeshCreator::CreateBall(Window* win, int index, float radius, int sliceCount, int stackCount, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

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

	ComputeNormals(vertices, indices);

	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreateDiamond(Window* win, int index, float width, float height, float midHeight, float depth, int sliceCount, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	float radiusX = width * 0.8f;
	float radiusZ = depth * 0.8f;
	// Top vertex
	for (float i = 0; i <= sliceCount; ++i) {
		float fraction = i / sliceCount;
		float theta = 2.0f * XM_PI * fraction;

		float x = radiusX * cosf(theta);
		float z = radiusZ * sinf(theta);

		vertices.push_back({ DirectX::XMFLOAT3(x, height, z), color, XMFLOAT3(0,1,0) });
	}

	for (int i = 0; i < sliceCount; ++i) {
		indices.push_back((i + 1) % sliceCount + 1);
		indices.push_back(i + 1);
		indices.push_back(0);
	}
	int baseIndex = sliceCount + 1;
	// Mid vertex
	radiusX = width;
	radiusZ = depth;
	for (float i = 0; i <= sliceCount; ++i) {
		float fraction = i / sliceCount;
		float theta = 2.0f * XM_PI * fraction;

		float x = radiusX * cosf(theta);
		float z = radiusZ * sinf(theta);

		vertices.push_back({ DirectX::XMFLOAT3(x, midHeight, z), color, XMFLOAT3(0,1,0) });
	}

	for (int i = -1; i < sliceCount; ++i) {
		indices.push_back(baseIndex + (i + 1) % sliceCount);
		indices.push_back(baseIndex + i);
		indices.push_back(i + 1);
		indices.push_back(baseIndex + (i + 1) % sliceCount);
		indices.push_back(i + 1);
		indices.push_back(i + 1 + 1);
	}

	// Bottom vertex
	vertices.push_back({ DirectX::XMFLOAT3(0, 0, 0), color, XMFLOAT3(0,1,0) });

	for (int i = 0; i < sliceCount; ++i) {
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + (i + 1) % sliceCount);
		indices.push_back(baseIndex + sliceCount + 1);
	}

	ComputeNormals(vertices, indices);

	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreateCylinder(Window* win, int index, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (int i = 0; i <= stackCount; ++i)
	{
		float y = height * i / stackCount;
		float radius = bottomRadius + (topRadius - bottomRadius) * i / stackCount;
		for (int j = 0; j <= sliceCount; ++j)
		{
			float theta = XM_2PI * j / sliceCount;
			float x = radius * cosf(theta);
			float z = radius * sinf(theta);
			vertices.push_back({ XMFLOAT3(x, y, z), color, XMFLOAT3(0,1,0) });
		}
	}

	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			int first = i * (sliceCount + 1) + j;
			int second = first + sliceCount + 1;
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
		}
	}

	vertices.push_back({ XMFLOAT3(0, 0, 0), color, XMFLOAT3(0,1,0) });
	vertices.push_back({ XMFLOAT3(0, height, 0), color, XMFLOAT3(0,1,0) });

	for (int j = 0; j < sliceCount; ++j)
	{
		int baseIndex = (stackCount + 1) * (sliceCount + 1);
		indices.push_back(baseIndex);
		indices.push_back(j);
		indices.push_back((j + 1) % (sliceCount + 1));
		indices.push_back((stackCount) * (sliceCount + 1) + (j + 1) % (sliceCount + 1));
		indices.push_back((stackCount) * (sliceCount + 1) + j);
		indices.push_back(baseIndex + 1);
	}

	ComputeNormals(vertices, indices);

	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreateCone(Window* win, int index, float radius, float height, int sliceCount, int stackCount, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (int i = 0; i <= stackCount; ++i)
	{
		float y = height * i / stackCount;
		float r = radius * (1 - (float)i / stackCount);
		for (int j = 0; j <= sliceCount; ++j)
		{
			float theta = XM_2PI * j / sliceCount;
			float x = r * cosf(theta);
			float z = r * sinf(theta);
			vertices.push_back({ XMFLOAT3(x, y, z), color, XMFLOAT3(0,1,0) });
		}
	}

	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			int first = i * (sliceCount + 1) + j;
			int second = first + sliceCount + 1;
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
		}
	}

	vertices.push_back({ XMFLOAT3(0, 0, 0), color, XMFLOAT3(0,1,0) });

	for (int j = 0; j < sliceCount; ++j)
	{
		int baseIndex = (stackCount + 1) * (sliceCount + 1);
		indices.push_back(baseIndex);
		indices.push_back(j);
		indices.push_back((j + 1) % (sliceCount + 1));
	}

	ComputeNormals(vertices, indices);

	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreateTorus(Window* win, int index, float majorRadius, float minorRadius, int majorSliceCount, int minorSliceCount, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (int i = 0; i <= majorSliceCount; ++i)
	{
		float phi = XM_2PI * i / majorSliceCount;
		for (int j = 0; j <= minorSliceCount; ++j)
		{
			float theta = XM_2PI * j / minorSliceCount;
			float x = (majorRadius + minorRadius * cosf(theta)) * cosf(phi);
			float y = minorRadius * sinf(theta);
			float z = (majorRadius + minorRadius * cosf(theta)) * sinf(phi);
			vertices.push_back({ XMFLOAT3(x, y, z), color, XMFLOAT3(0,1,0) });
		}
	}

	for (int i = 0; i < majorSliceCount; ++i)
	{
		for (int j = 0; j < minorSliceCount; ++j)
		{
			int first = i * (minorSliceCount + 1) + j;
			int second = first + minorSliceCount + 1;
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(first);
			indices.push_back(second + 1);
			indices.push_back(second);
			indices.push_back(first + 1);
		}
	}

	ComputeNormals(vertices, indices);
	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreatePyramid(Window* win, int index, float width, float height, float depth, XMFLOAT4 color, const wchar_t* filepath)
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
	float w2 = width * 0.5f;
	float h2 = height * 0.5f;
	float d2 = depth * 0.5f;

	XMFLOAT3 corner[6] = {
		{-w2, -h2, -d2}, { w2, -h2, -d2 }, { 0, h2, 0 },
		{-w2, -h2, d2},  { w2, -h2, d2 },  { 0, h2, 0 }
	};

	int faceIndices[5][3] = {
		{0, 1, 2},
		{3, 5, 4},
		{0, 2, 3},
		{1, 4, 2},
		{4, 5, 2}
	};

	XMFLOAT3 normals[5] = {
		{0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,1,0}
	};

	for (int i = 0; i < 5; ++i)
	{
		uint32_t baseIndex = (uint32_t)vertices.size();
		for (int j = 0; j < 3; ++j)
		{
			int cornerIndex = faceIndices[i][j];
			vertices.push_back({ corner[cornerIndex], color, normals[i] });
		}
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 0);
	}

	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(3);
	indices.push_back(1);

	ComputeNormals(vertices, indices);

	return win->BuildMesh(vertices, indices, index, filepath);
}

MeshGeometry MeshCreator::CreateCustomMesh(Window* win, int index, const char* jsonpath, int numOfMesh, XMFLOAT4 color, const wchar_t* filepath)
{
	std::ifstream file(jsonpath);
	if (!file.is_open()) {
		throw std::runtime_error("Impossible d'ouvrir le fichier");
	}
	nlohmann::json j;

	file >> j;

	// Load model
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (const auto& vertexJson : j["vertices"]) {
		Vertex v;
		v.FromJson(vertexJson, v, color,{0,0,0});
		vertices.push_back(v);
	}
	for (int j = 0; j < (int)vertices.size(); j++) {
		indices.push_back(j);
	}

	// Draw multiple models
	std::vector<Vertex> mergedVertices;
	std::vector<std::uint32_t> mergedIndices;

	for (int i = 0; i < numOfMesh; ++i)
	{
		FLOAT3 randomOffset = FLOAT3(0, 0, 0);
		int signX = rand() % 2 == 0 ? -1 : 1;
		int signY = rand() % 2 == 0 ? -1 : 1;
		int rotation = rand() % 2 == 0 ? -1 : 1;
		int randomX = rand() % 1000;
		int randomY = (rand() % 150) + 20;
		int randomZ = rand() % 30000;
		randomOffset = FLOAT3(randomX * signX, randomY * signY, randomZ);

		for (auto& vertice : vertices)
		{
			Vertex tempVertex = vertice;
			tempVertex.Pos = toXMFLOAT3(randomOffset + toFLOAT3(vertice.Pos));
			FLOAT3 tempPos = toFLOAT3(tempVertex.Pos);
			tempPos.x *= rotation;
			tempVertex.Pos = toXMFLOAT3(tempPos);
			mergedVertices.push_back(tempVertex);
		}
	}

	for (size_t i = 0; i < numOfMesh * indices.size(); i++) {
		mergedIndices.push_back(i);
	}

	ComputeNormals(vertices, indices);

	return win->BuildMesh(mergedVertices, mergedIndices, index, filepath);
}