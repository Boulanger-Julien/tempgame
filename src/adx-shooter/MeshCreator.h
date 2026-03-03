#pragma once 
#include "framework.h"
#include "adx-render/Window.h"

class Road {
public:
	FLOAT2 start;
	FLOAT2 end;
	std::vector<FLOAT2> points;
	MeshGeometry mesh;
	Road(FLOAT2 start, FLOAT2 end, std::vector<FLOAT2> points, MeshGeometry mesh) : start(start), end(end), points(points), mesh(mesh) {}
	Road() : start(0, 0), end(0, 0), points(), mesh() {}
};

class MeshCreator
{
private:
	static void ComputeNormals(std::vector<Vertex>& verts, const std::vector<std::uint32_t>& inds);
public:
	static Road CreateRoad(Window* win, int index, float width, float length, int totRoad, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateCylinder(Window* win, int index, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateTorus(Window* win, int index, float majorRadius, float minorRadius, int majorSliceCount, int minorSliceCount, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateCone(Window* win, int index, float radius, float height, int sliceCount, int stackCount, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateBall(Window* win, int index, float radius, int sliceCount, int stackCount, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateBox(Window* win, int index, float width, float height, float depth, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateDiamond(Window* win, int index, float width, float height, float midHeight, float depth, int sliceCount, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreatePyramid(Window* win, int index, float width, float height, float depth, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
	static MeshGeometry CreateMount(Window* win, int index, int rows, int cols, int height, XMFLOAT4 colorTop = XMFLOAT4(Colors::White), XMFLOAT4 colorBottom = XMFLOAT4(Colors::DarkGreen));

	static MeshGeometry CreateCustomMesh(Window* win, int index, const char* jsonpath, int numOfMesh, XMFLOAT4 color = XMFLOAT4(Colors::Gray), const wchar_t* filename = nullptr);
};