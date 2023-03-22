#pragma once
#include <FreeImage.h>
#include "Camera.hpp"
#include "Scene.hpp"
#include "Intersection.hpp"

typedef std::pair<bool, float> PII;

class Film {
private:
	int w, h;
	BYTE* pixels;

	const char* outputFilename;

	Scene* myActiveScene = nullptr;
	Camera* myActiveCamera = nullptr;

	vec3 FindColor(Ray ray, int currDepth = 0);

	Intersection TraceRay(Ray ray, BVHAccel* root);
	Intersection ClosestHitSphere(Ray ray, float hitDistance, Object* closestSphere);
	Intersection ClosestHitTriangle(Ray ray, float hitDistance, Object* closestTriangle, vec3* vertices);
	Intersection Miss(Ray ray);

	Intersection findIntersection(Ray ray, Object* object);
	Intersection getIntersection(BVHBuildNode* node, Ray ray);

public:
	Film(int _w, int _h) {
		w = _w, h = _h;
		pixels = new BYTE[3 * w * h];
	}

	~Film() {
		delete[] pixels;
	}

	void setOutputFilename(const char* filename) { outputFilename = filename; }
	void Render(Scene scene, Camera camera);
};