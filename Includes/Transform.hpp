#pragma once

#include "Utils.hpp"

class Transform
{
public:
	static void left(float degrees, vec3& eye, vec3& up);
	static void up(float degrees, vec3& eye, vec3& up);
	static mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up);
	static mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	static mat4 rotate(const float degrees, const vec3& axis);
	static mat4 scale(const float& sx, const float& sy, const float& sz);
	static mat4 translate(const float& tx, const float& ty, const float& tz);
	static vec3 upvector(const vec3& up, const vec3& zvec);
};