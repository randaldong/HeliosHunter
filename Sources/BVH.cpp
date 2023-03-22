#include <algorithm>
#include <cassert>
#include "BVH.hpp"

void quickSort(std::vector<Object*>& objects, int x, vec3* vertex, int l, int r)
{
    if (l >= r) return;

    int i = l - 1, j = r + 1;
    int mid = l + r >> 1;
    Object* middle = objects[mid];

    while (i < j)
    {
        if (x == 0)
        {
            do i++; while (objects[i]->getObjectBbox(vertex).Centroid().x < middle->getObjectBbox(vertex).Centroid().x);
            do j--; while (objects[j]->getObjectBbox(vertex).Centroid().x > middle->getObjectBbox(vertex).Centroid().x);
        }
        else if (x == 1)
        {
            do i++; while (objects[i]->getObjectBbox(vertex).Centroid().y < middle->getObjectBbox(vertex).Centroid().y);
            do j--; while (objects[j]->getObjectBbox(vertex).Centroid().y > middle->getObjectBbox(vertex).Centroid().y);
        }
        else
        {
            do i++; while (objects[i]->getObjectBbox(vertex).Centroid().z < middle->getObjectBbox(vertex).Centroid().z);
            do j--; while (objects[j]->getObjectBbox(vertex).Centroid().z > middle->getObjectBbox(vertex).Centroid().z);
        }
        if (i < j) std::swap(objects[i], objects[j]);
    }

    quickSort(objects, x, vertex, l, j);
    quickSort(objects, x, vertex, j + 1, r);
}

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod, vec3* vertex)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(SplitMethod::Naive), primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives, vertex);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects, vec3* vertex)
{
    BVHBuildNode* node = new BVHBuildNode();


    if (objects.size() == 1)
    {
        //leafNode created
        node->bounds = objects[0]->getObjectBbox(vertex);
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2)
    {
        node->left = recursiveBuild(std::vector<Object*>{ objects[0] }, vertex);
        node->right = recursiveBuild(std::vector<Object*>{ objects[1] }, vertex);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else
    {
        Bbox centroidBounds;
        for (int i = 0; i < objects.size(); i++)
            centroidBounds =
            Union(centroidBounds, objects[i]->getObjectBbox(vertex).Centroid());
        int dim = centroidBounds.maxExtent(), size = objects.size() - 1;
        quickSort(objects, dim, vertex, 0, size);

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes, vertex);
        node->right = recursiveBuild(rightshapes, vertex);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}