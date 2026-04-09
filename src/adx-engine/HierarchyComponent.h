#pragma once

struct HierarchyComponent
{
	int parentEntityID = -1; // -1
	std::vector<int> childEntityIDs;
};