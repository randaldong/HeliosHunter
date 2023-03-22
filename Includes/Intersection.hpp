#pragma once
#include "Object.hpp"

class Intersection
{
public:
	float hitDistance; // min t of the ray
	vec3 WorldPosition; // WorldPosition of the hitPoint
	vec3 WorldNormal; // WorldNormal of the hitPoint
	Object* object; // hit object
};