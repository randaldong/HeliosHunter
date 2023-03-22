#pragma once
#include <vector>
#include <memory>
#include "Object.hpp"

struct BVHBuildNode;

class BVHAccel {
public:
	enum class SplitMethod { Naive, SAH };
	BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod, vec3* vertex);
	~BVHAccel();

	BVHBuildNode* root;

	BVHBuildNode* recursiveBuild(std::vector<Object*> objects, vec3* vertex);

	const int maxPrimsInNode;
	const SplitMethod splitMethod;
	std::vector<Object*> primitives;
};

struct BVHBuildNode
{
	Bbox bounds;
	BVHBuildNode* left;
	BVHBuildNode* right;
	Object* object;

	int splitAxis = 0, firstPrimOffset = 0, nPrimitive = 0;

	BVHBuildNode() {
		bounds = Bbox();
		left = nullptr, right = nullptr;
		object = nullptr;
	}
};