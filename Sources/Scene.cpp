#include "Scene.hpp"

void Scene::addObject(Object* obj)
{
	Objects.push_back(obj);
}

void Scene::buildBVH()
{
	printf("-----Generateing BVH...\n\n");
	this->bvh = new BVHAccel(Objects, 1, BVHAccel::SplitMethod::Naive, vertices);
}