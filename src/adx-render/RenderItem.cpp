#include "pch.h"
#include "RenderItem.h"

void RenderItem::CreateItem(std::string _name, Mesh* _mesh)
{
	name = _name;
	mesh = _mesh;
}
