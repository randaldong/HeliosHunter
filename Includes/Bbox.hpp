#pragma once
#include <vector>
#include <array>
#include "Ray.hpp"

class Bbox
{
public:
	vec3 pMin, pMax;
	Bbox()
	{
		float minNum = std::numeric_limits<float>::lowest();
		float maxNum = std::numeric_limits<float>::max();
		pMin = vec3(minNum, minNum, minNum);
		pMax = vec3(maxNum, maxNum, maxNum);
	}
	Bbox(const vec3 p) : pMin(p), pMax(p) {}
	Bbox(const vec3 p1, const vec3 p2)
	{
		pMin = vec3(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
		pMax = vec3(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
	}

	vec3 Diagonal() const { return pMax - pMin; }

	int maxExtent() const
	{
		vec3 d = Diagonal();
		if (d.x > d.y && d.x > d.z)
			return 0;
		else if (d.y > d.z)
			return 1;
		else
			return 2;
	}

	float SurfaceArea() const
	{
		vec3 d = Diagonal();
		return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
	}

	vec3 Centroid() { return 0.5f * pMin + 0.5f * pMax; }

	Bbox Intersect(const Bbox& b)
	{
		return Bbox(vec3(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y), fmax(pMin.z, b.pMin.z))
			, vec3(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y), fmin(pMax.z, b.pMax.z)));
	}

	bool Inside(const vec3& p, const Bbox& b)
	{
		return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
			p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
	}

	inline const vec3& operator[](int i) const
	{
		return (i == 0) ? pMin : pMax;
	}

	inline bool IntersectionP(const Ray& ray, const vec3& invDir, const std::array<int, 3>& dirisNeg) const;
};

inline bool Bbox::IntersectionP(const Ray& ray, const vec3& invDir, const std::array<int, 3>& dirisNeg) const
{
	float min_x = std::numeric_limits<float>::lowest();
	float max_x = std::numeric_limits<float>::max();
	if (invDir[0] != 0)
	{
		min_x = (pMin.x - ray.origin.x) * invDir[0];
		max_x = (pMax.x - ray.origin.x) * invDir[0];
	}

	float min_y = std::numeric_limits<float>::lowest();
	float max_y = std::numeric_limits<float>::max();
	if (invDir[1] != 0)
	{
		min_y = (pMin.y - ray.origin.y) * invDir[1];
		max_y = (pMax.y - ray.origin.y) * invDir[1];
	}

	float min_z = std::numeric_limits<float>::lowest();
	float max_z = std::numeric_limits<float>::max();
	if (invDir[2] != 0)
	{
		min_z = (pMin.z - ray.origin.z) * invDir[2];
		max_z = (pMax.z - ray.origin.z) * invDir[2];
	}

	if (dirisNeg[0] && invDir[0] != 0.0f) std::swap(min_x, max_x);
	if (dirisNeg[1] && invDir[1] != 0.0f) std::swap(min_y, max_y);
	if (dirisNeg[2] && invDir[2] != 0.0f) std::swap(min_z, max_z);

	float enter = std::max(min_x, std::max(min_y, min_z));
	float exit = std::min(max_x, std::min(max_y, max_z));

	if (enter <= exit && exit >= 0)
		return true;
	else
		return false;
}

inline Bbox Union(const Bbox& b1, const Bbox& b2)
{
	Bbox ret;
	ret.pMin = vec3(fmin(b1.pMin.x, b2.pMin.x), fmin(b1.pMin.y, b2.pMin.y), fmin(b1.pMin.z, b2.pMin.z));
	ret.pMax = vec3(fmax(b1.pMax.x, b2.pMax.x), fmax(b1.pMax.y, b2.pMax.y), fmax(b1.pMax.z, b2.pMax.z));
	return ret;
}

inline Bbox Union(const Bbox& b, const vec3& p)
{
	Bbox ret;
	ret.pMin = vec3(fmin(b.pMin.x, p.x), fmin(b.pMin.y, p.y), fmin(b.pMin.z, p.z));
	ret.pMax = vec3(fmax(b.pMax.x, p.x), fmax(b.pMax.y, p.y), fmax(b.pMax.z, p.z));
	return ret;
}