#pragma once
#include "Utils.hpp"

class Ray
{
public:
	vec3 origin;
	vec3 direction;

	Ray(const vec3& ori, const vec3& dir) : origin(ori), direction(dir) {}
};