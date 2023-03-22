#include "Transform.hpp"

mat4 Transform::rotate(const float degrees, const vec3& axis)
{
    float cosValue = cos(glm::radians(degrees));
    float sinValue = sin(glm::radians(degrees));
    mat3 identity = mat3(1.0f);
    vec3 axisUnit = glm::normalize(axis);
    float x = axisUnit.x;
    float y = axisUnit.y;
    float z = axisUnit.z;
    vec3 outProduct_x = vec3(x * x, x * y, x * z);
    vec3 outProduct_y = vec3(x * y, y * y, y * z);
    vec3 outProduct_z = vec3(x * z, y * z, z * z);
    mat3 outProduct = mat3(outProduct_x, outProduct_y, outProduct_z);
    mat3 dual = mat3(vec3(0, z, -y), vec3(-z, 0, x), vec3(y, -x, 0));
    mat3 rotateMtx = cosValue * identity + (1 - cosValue) * outProduct + sinValue * dual;
    return mat4(rotateMtx);
}

mat4 Transform::scale(const float& sx, const float& sy, const float& sz)
{
    mat4 scaleMtx = mat4(
        vec4(sx, 0, 0, 0),
        vec4(0, sy, 0, 0),
        vec4(0, 0, sz, 0),
        vec4(0, 0, 0, 1));
    return scaleMtx;
}

mat4 Transform::translate(const float& tx, const float& ty, const float& tz)
{
    mat4 translateMtx = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(tx, ty, tz, 1));
    return translateMtx;
}

vec3 Transform::upvector(const vec3& up, const vec3& zvec)
{
    vec3 x = glm::cross(up, zvec);
    vec3 y = glm::cross(zvec, x);
    vec3 ret = glm::normalize(y);
    return ret;
}