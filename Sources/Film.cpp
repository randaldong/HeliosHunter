#include "Film.hpp"
#include <stdlib.h>
#include "Object.hpp"
#include "Bbox.hpp"

extern int numObjects;
extern int numLights;
extern vec3 attenuation;
extern int maxDepth;

const float bias = 0.01f; // avoid self shadowing

/*---------------------------------------------------------- Intersect ----------------------------------------------------------*/
inline PII RaySphereIntersect(Ray ray, Object* obj)
{
	mat4 invTransf = glm::inverse(obj->transform);
	vec3 oriTransf = vec3(invTransf * vec4(ray.origin, 1.0f));
	vec3 dirTransf = vec3(invTransf * vec4(ray.direction, 0.0f));

	float a = glm::dot(dirTransf, dirTransf);
	float b = 2 * glm::dot(dirTransf, (oriTransf - obj->centerPosition));
	float c = glm::dot(oriTransf - obj->centerPosition, oriTransf - obj->centerPosition) - obj->Radius * obj->Radius;
	float delta = b * b - 4 * a * c;
	if (delta >= 0) {
		float t1 = (-b + sqrt(delta)) / (2 * a);
		float t2 = (-b - sqrt(delta)) / (2 * a);
		float t = fmin(t1, t2);
		if (0.00001 < t) return { true, t };
	}
	return { false, -1.0f };
}

inline PII RayTriangleIntersect(Ray ray, Object* obj, vec3* vertices)
{
	vec3 A = vec3(obj->transform * vec4(vertices[obj->indices[0]], 1));
	vec3 B = vec3(obj->transform * vec4(vertices[obj->indices[1]], 1));
	vec3 C = vec3(obj->transform * vec4(vertices[obj->indices[2]], 1));

	vec3 triNormal = glm::normalize(glm::cross(C - A, B - A));
	float t = (glm::dot(A, triNormal) - glm::dot(ray.origin, triNormal)) / glm::dot(ray.direction, triNormal);
	vec3 P = ray.origin + t * ray.direction;

	// P in triangle? Barycentric Coord -- triangle area ratio, refer to ravi's lecture 16
	// for beta
	vec3 ACcrossAB = glm::cross(C - A, B - A);
	vec3 ACcrossAP = glm::cross(C - A, P - A);
	// for gamma
	vec3 ABcrossAC = -ACcrossAB;
	vec3 ABcrossAP = glm::cross(B - A, P - A);

	if (glm::dot(ACcrossAB, ACcrossAP) >= 0 && glm::dot(ABcrossAC, ABcrossAP) >= 0) { // beta, gamma >= 0
		float beta = glm::length(ACcrossAP) / glm::length(ACcrossAB);
		float gamma = glm::length(ABcrossAP) / glm::length(ABcrossAC);
		if (beta + gamma <= 1 && 0.00001 < t) return { true, t };
	}
	return { false, -1.0f };
}

Intersection Film::ClosestHitSphere(Ray ray, float hitDistance, Object* closestSphere)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.object = closestSphere;

	mat4 invTransf = glm::inverse(closestSphere->transform);
	vec3 oriTransf = vec3(invTransf * vec4(ray.origin, 1.0f));
	vec3 dirTransf = vec3(invTransf * vec4(ray.direction, 0.0f));

	vec3 hitPosition = oriTransf + hitDistance * dirTransf;
	vec3 sphereNormalObj = hitPosition - closestSphere->centerPosition;

	vec4 hitPointWorld = closestSphere->transform * vec4(hitPosition, 1.0f);
	vec4 sphereNormalWorld = glm::transpose(invTransf) * vec4(sphereNormalObj, 0.0f);

	intersection.WorldPosition = vec3(hitPointWorld / hitPointWorld.w);
	intersection.WorldNormal = glm::normalize(vec3(sphereNormalWorld));

	return intersection;
}

Intersection Film::ClosestHitTriangle(Ray ray, float hitDistance, Object* closestTriangle, vec3* vertices)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.object = closestTriangle;

	vec3 A = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[0]], 1));
	vec3 B = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[1]], 1));
	vec3 C = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[2]], 1));

	vec3 triNormal = glm::normalize(glm::cross(B - A, C - A));
	vec3 P = ray.origin + hitDistance * ray.direction;

	intersection.WorldPosition = P;
	intersection.WorldNormal = triNormal;

	return intersection;
}

Intersection Film::Miss(Ray ray)
{
	Intersection intersection;
	intersection.hitDistance = -1.0f;
	return intersection;
}

Intersection Film::findIntersection(Ray ray, Object* object)
{
	if (object->type == sphere)
	{
		if (RaySphereIntersect(ray, object).first)
		{
			float t = RaySphereIntersect(ray, object).second;
			return ClosestHitSphere(ray, t, object);
		}
		else return Miss(ray);
	}
	else
	{
		if (RayTriangleIntersect(ray, object, myActiveScene->vertices).first)
		{
			float t1 = RayTriangleIntersect(ray, object, myActiveScene->vertices).second;
			return ClosestHitTriangle(ray, t1, object, myActiveScene->vertices);
		}
		else return Miss(ray);
	}
}

Intersection Film::getIntersection(BVHBuildNode* node, Ray ray)
{
	float x = 0, y = 0, z = 0;
	if (ray.direction.x != 0.0f) x = 1.0f / ray.direction.x;
	if (ray.direction.y != 0.0f) y = 1.0f / ray.direction.y;
	if (ray.direction.z != 0.0f) z = 1.0f / ray.direction.z;
	vec3 invDir(x, y, z);

	std::array<int, 3> dirIsNeg;
	dirIsNeg[0] = ray.direction.x > 0 ? 0 : 1;
	dirIsNeg[1] = ray.direction.y > 0 ? 0 : 1;
	dirIsNeg[2] = ray.direction.z > 0 ? 0 : 1;

	if (!node->bounds.IntersectionP(ray, invDir, dirIsNeg))
	{
		return Miss(ray);
	}

	if (node->left == nullptr && node->right == nullptr)
	{
		return findIntersection(ray, node->object);
	}

	Intersection left = getIntersection(node->left, ray);
	Intersection right = getIntersection(node->right, ray);
	if (left.hitDistance > 0 && right.hitDistance > 0)
		return left.hitDistance <= right.hitDistance ? left : right;
	else if (left.hitDistance > 0)
		return left;
	else
		return right;
}

/*---------------------------------------------------------- Color ----------------------------------------------------------*/
static uint32_t ConvertToRGB(const vec3& color)
{
	uint8_t r = (uint8_t)(color.r * 255.0f);
	uint8_t g = (uint8_t)(color.g * 255.0f);
	uint8_t b = (uint8_t)(color.b * 255.0f);

	uint32_t result = (b << 16) | (g << 8) | r;
	return result;
}

inline vec3 ComputeColor(vec3 lightDir, vec3 lightCol, vec3 normal, vec3 halfvec, vec3 mydiffuse, vec3 myspecular, float myshininess)
{
	float nDotL = glm::dot(normal, lightDir);
	vec3 lambert = mydiffuse * lightCol * fmax(nDotL, 0.0f);
	float nDotH = glm::dot(normal, halfvec);
	vec3 phong = myspecular * lightCol * pow(fmax(nDotH, 0.0f), myshininess);
	return (lambert + phong);
}

/*---------------------------------------------------------- Render ----------------------------------------------------------*/
Intersection Film::TraceRay(Ray ray, BVHAccel* bvh)
{
	Intersection isect = getIntersection(bvh->root, ray);
	return isect;
}

vec3 Film::FindColor(Ray ray, int currDepth)
{
	vec3 currDepthColor(0.0f);
	if (currDepth == maxDepth) return currDepthColor;
	
	vec3 bgColor(0.0f);
	Intersection intersection = TraceRay(ray, myActiveScene->bvh);
	if (intersection.hitDistance <= 0.0f) return bgColor;

	Object* object = intersection.object;

	vec3 objDiffuse = object->material.diffuse;
	vec3 objSpecular = object->material.specular;
	vec3 rayDir = glm::normalize(ray.direction); // from eye to hit point

	for (int i = 0; i < numLights; i++) {
		Light* curr_light = &(myActiveScene->lights[i]);
		vec3 lightDir = vec3(0.0f);
		float visibility = 1.0f;
		float attnCoeff = 1.0f;

		if (curr_light->lightPosition.w == 0) {
			lightDir = glm::normalize(vec3(curr_light->lightPosition));
		}
		else {
			lightDir = vec3(curr_light->lightPosition) - intersection.WorldPosition; // from hit point to light
			float dist = glm::length(lightDir);
			attnCoeff = 1.0f / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
			lightDir = glm::normalize(lightDir);
		}
		vec3 halfvec = glm::normalize(-rayDir + lightDir);
		vec3 lightCol = curr_light->lightColor;

		// visibility & shadow
		Ray toLight(intersection.WorldPosition, lightDir);
		Intersection nextIntersection = TraceRay(toLight, myActiveScene->bvh);

		if (nextIntersection.hitDistance > 0.0f) {
			if (curr_light->lightPosition.w != 0) {
				vec3 l1 = nextIntersection.WorldPosition - intersection.WorldPosition;
				vec3 l2 = vec3(curr_light->lightPosition) - intersection.WorldPosition;
				if (glm::length(l1) < glm::length(l2))
					visibility = 0;
			}
			else visibility = 0;
		}
		currDepthColor += visibility * attnCoeff * ComputeColor(lightDir, lightCol, intersection.WorldNormal, halfvec, objDiffuse, objSpecular, object->material.shininess);
	
	}

	currDepthColor += object->material.emission + object->material.ambient;
	// add next depth color
	vec3 reflDir = glm::normalize(rayDir - 2 * glm::dot(intersection.WorldNormal, rayDir) * intersection.WorldNormal);
	Ray reflRay(intersection.WorldPosition, reflDir);
	currDepthColor += FindColor(reflRay, currDepth + 1) * objSpecular;
	
	return currDepthColor;
}

void Film::Render(Scene scene, Camera camera)
{
	myActiveCamera = &camera;
	myActiveScene = &scene;

	int printVal = 5;

	myActiveScene->buildBVH();

	int pix = w * h;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			int base = 3 * (x + y * w);
			Ray ray = myActiveCamera->RayThruPixel(x, y);
			vec3 color = FindColor(ray);
			color = glm::clamp(color, vec3(0.0f), vec3(1.0f));
			uint32_t result_color = ConvertToRGB(color);

			pixels[base] = (uint8_t)(result_color >> 16);
			pixels[base + 1] = (uint8_t)(result_color >> 8);
			pixels[base + 2] = (uint8_t)result_color;

			// progress bar
			int finished = (y * w + x) / (float)pix * 100;
			if (finished % 100 >= printVal) {
				printf("Ray Tracing Progress: %i %%\n", finished);
				printVal += 5;
			}
		}
	}
	FreeImage_Initialise();
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);

	FreeImage_Save(FIF_PNG, img, outputFilename, 0);
	FreeImage_DeInitialise();

}







