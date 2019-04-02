#pragma once
#include "Primitive.h"
#include "objLoader.hpp"
#include "GenVector.h"
#include <vector>


class TreeNode {
public:

	TreeNode* left = NULL;
	TreeNode* right = NULL;
	Primitive* primitive = NULL;
	Vector3 minPoint;
	Vector3 maxPoint;
	bool isLeaf = false;

	Vector3 getCentroid(objLoader* loader) {
		return primitive->center;
	}

	Primitive* hitsNode(Ray r, objLoader* loader) {

		if (intersects(r, loader)) {

			if (isLeaf) {
				return primitive;
			}

			Primitive* ln = left->hitsNode(r, loader);
			Primitive* rn = right->hitsNode(r, loader);

			//printf("left: %d\tright: %d\n", ln, rn);

			if (ln != NULL && rn != NULL) {
				//the ray hits two things
				//we know what is returned are primitives (has a face or sphere)
				
				float rt = 0;
				float lt = 0;
				
				if (ln->face != NULL) {
					lt = r.triangleIntersection(ln->face, loader);
				}
				else {
					lt = r.sphereIntersection(ln->sphere, loader);

				}

				if (rn->face != NULL) {
					rt = r.triangleIntersection(rn->face, loader);
				}
				else {
					rt = r.sphereIntersection(rn->sphere, loader);

				}
				
				if (lt < rt) {
					return ln;
				}
				else {
					return rn;

				}
			}
			else if (ln != NULL) {
				return ln;
			}
			return rn;


		}
		else {
			return NULL;
		}


	}

	bool intersects(Ray r, objLoader* loader) {

		if (isLeaf) {
			if (primitive->face != NULL) {
				//triangle
				float i = r.triangleIntersection(primitive->face, loader);
				return i != 0;
			}
			else {
				//sphere
				float i = r.sphereIntersection(primitive->sphere, loader);
				return i != 0;
			}
		}

		float minx = (minPoint[0] - r.origin[0]) / r.dir[0];
		float miny = (minPoint[1] - r.origin[1]) / r.dir[1];
		float minz = (minPoint[2] - r.origin[2]) / r.dir[2];

		float maxx = (maxPoint[0] - r.origin[0]) / r.dir[0];
		float maxy = (maxPoint[1] - r.origin[1]) / r.dir[1];
		float maxz = (maxPoint[2] - r.origin[2]) / r.dir[2];

		//printf("min intersection: <%f, %f, %f>\n", minx, miny, minz);
		//printf("max intersection: <%f, %f, %f>\n\n", maxx, maxy, maxz);

		float tmin = minx;
		float tmax = maxx;

		if (tmin > tmax) {
			float temp = tmin;
			tmin = tmax;
			tmax = temp;
		}

		if (miny > maxy) {
			float temp = miny;
			miny = maxy;
			maxy = temp;
		}

		if ((tmin > maxy) || (miny > tmax)) {
			//printf("fail 1\n\n");
			return false;
		}

		if (miny > tmin) {
			tmin = miny;
		}

		if (maxy < tmax) {

			tmax = maxy;
		}

		if (minz > maxz) {
			float temp = minz;
			minz = maxz;
			maxz = temp;
		}

		if ((tmin > maxz) || (minz > tmax)) {
			//printf("fail 2\n\n");
			return false;
		}

		if (minz > tmin) {
			tmin = minz;

		}

		if (maxz < tmax)
		{
			tmax = maxz;
		}

		//printf("pass!\n\n");
		return true;


	}
};

class Tree {
public:
	TreeNode* root = new TreeNode();
	objLoader* loader;


	Primitive* hitsNode(Ray r, objLoader* loader) {
		return root->hitsNode(r, loader);
	}

};

TreeNode* getSphereLeaf(TreeNode* node, objLoader* loader, Sphere* sphere) {

	Vector3 center = sphere->center;
	float radius = sphere->getRadius(loader);

	Vector3 min = center - radius;
	Vector3 max = center + radius;

	sphere->minPoint = min;
	sphere->maxPoint = max;
	node->minPoint = min;
	node->maxPoint = max;
	node->primitive = sphere;
	node->isLeaf = true;
	return node;
}

TreeNode* getTriangleLeaf(TreeNode* node, objLoader* loader, Triangle* triangle) {
	Vector3 center = triangle->center;
	float xmin = INFINITY;
	float xmax = -INFINITY;
	float ymin = INFINITY;
	float ymax = -INFINITY;
	float zmin = INFINITY;
	float zmax = -INFINITY;

	obj_face* face = triangle->face;

	for (int i = 0; i < face->vertex_count; i++) {
		Vector3 point = objToVec3(loader->vertexList[face->vertex_index[i]]);

		if (point[0] < xmin) {
			xmin = point[0];
		}

		if (point[0] > xmax) {
			xmax = point[0];
		}

		if (point[1] < ymin) {
			ymin = point[1];
		}

		if (point[1] > ymax) {
			ymax = point[1];
		}

		if (point[2] < zmin) {
			zmin = point[2];
		}

		if (point[2] > zmax) {
			zmax = point[2];
		}
	}

	triangle->minPoint = Vector3(xmin, ymin, zmin);
	triangle->maxPoint = Vector3(xmax, ymax, zmax);
	node->minPoint = Vector3(xmin, ymin, zmin);
	node->maxPoint = Vector3(xmax, ymax, zmax);
	node->primitive = triangle;
	node->isLeaf = true;
	return node;
}

void buildTree(TreeNode* root, std::vector<Primitive*> objects, int count, objLoader* loader) {
	if (count <= 0) {
		return;
	}
	else if (count == 1) {
		//base case
		//printf("hit base case\n");
		if (objects.at(0)->isSphere()) {
			Sphere* sphere = new Sphere(objects.at(0)->sphere, loader);
			root = getSphereLeaf(root, loader, sphere);
		}
		else {
			Triangle* triangle = new Triangle(objects.at(0)->face, loader);
			root = getTriangleLeaf(root, loader, triangle);
		}
		objects.clear();
		return;
	}
	else {
		//find the spatial median

		//find the spatial median (average)
		Vector3 total = Vector3(0, 0, 0);

		for (int i = 0; i < count; i++) {
			Vector3 center = (objects.at(i)->center);
			total = total + center;
		}
		total = total / count;


		//find which axis to split
		float xmin = INFINITY;
		float xmax = -INFINITY;
		float ymin = INFINITY;
		float ymax = -INFINITY;
		float zmin = INFINITY;
		float zmax = -INFINITY;


		for (int i = 0; i < count; i++) {

			//printf("min: <%f, %f, %f>\n", objects.at(i)->minPoint[0], objects.at(i)->minPoint[1], objects.at(i)->minPoint[2]);
			//printf("max: <%f, %f, %f>\n\n", objects.at(i)->maxPoint[0], objects.at(i)->maxPoint[1], objects.at(i)->maxPoint[2]);
		
			if (objects.at(i)->minPoint[0] < xmin) {
				xmin = objects.at(i)->minPoint[0];
			}

			if (objects.at(i)->minPoint[1] < ymin) {
				ymin = objects.at(i)->minPoint[1];
			}

			if (objects.at(i)->minPoint[2] < zmin) {
				zmin = objects.at(i)->minPoint[2];
			}


			if (objects.at(i)->maxPoint[0] > xmax) {
				xmax = objects.at(i)->maxPoint[0];
			}

			if (objects.at(i)->maxPoint[1] > ymax) {
				ymax = objects.at(i)->maxPoint[1];
			}

			if (objects[i]->maxPoint[2] > zmax) {
				zmax = objects[i]->maxPoint[2];
			}


		}

		float xdiff = xmax - xmin;
		float ydiff = ymax - ymin;
		float zdiff = zmax - zmin;

		//based off of the spatial median and bigger axis, split the nodes into two lists

		std::vector<Primitive*> left = {};
		std::vector<Primitive*> right = {};

		int axisIndex = -1;

		if (xdiff >= ydiff && xdiff >= zdiff) {
			//x is bigger
			axisIndex = 0;
		}
		else if (ydiff >= zdiff && ydiff >= xdiff) {
			//y is bigger
			axisIndex = 1;
		}
		else {
			//z is bigger
			axisIndex = 2;
		}

		int rightCount = 0;
		int leftCount = 0;

		//populate the arrays
		for (int i = 0; i < count; i++) {
			if (objects.at(i)->center[axisIndex] <= total[axisIndex]) {
				left.push_back(objects.at(i));
				//printf("left: %f\n", left.back()->face);
				leftCount++;
			}
			else {
				right.push_back(objects.at(i));
				//printf("right: %f\n", right.back()->center[0]);
				rightCount++;
			}
		}

		//check to see if spatial median failed
		if (rightCount == 0 || leftCount == 0) {
			//printf("spatial median failed!\n");

			left.clear();
			right.clear();
			leftCount = 0;
			rightCount = 0;

			if (count == 2) {
				left.push_back(objects.at(0));
				right.push_back(objects.at(1));
				leftCount++;
				rightCount++;
			}
			else {

				//using centroid average
				int r = rand() % count;
				Vector3 divider = objects.at(r)->center;

				//populate the arrays
				for (int i = 0; i < count; i++) {
					Vector3 current = objects.at(i)->maxPoint;
					if (current[0] <= divider[0] || current[1] <= divider[1] || current[2] <= divider[2]) {
						left.push_back(objects.at(i));
						leftCount++;
					}
					else {
						right.push_back(objects.at(i));
						rightCount++;
					}
				}

				//if it failed again
				if (leftCount == 0) {
					left.push_back(objects.at(0));
					right.clear();
					for (int i = 1; i < count; i++) {
						right.push_back(objects.at(i));
					}
					leftCount++;
					rightCount--;
				}
				else if (rightCount == 0) {
					right.push_back(objects.at(0));
					left.clear();
					for (int i = 1; i < count; i++) {
						left.push_back(objects.at(i));
					}
					leftCount--;
					rightCount++;
				}

			}
		}


		//make the inbetween node (the current root)

		root->minPoint = Vector3(xmin, ymin, zmin);
		root->maxPoint = Vector3(xmax, ymax, zmax);

		root->left = new TreeNode();
		root->right = new TreeNode();

		//recurse using the two lists
		//printf("starting left %d\n", count);
		buildTree(root->left, left, leftCount, loader);
		//printf("left %d done, starting right\n", count);
		buildTree(root->right, right, rightCount, loader);
		//printf("right %d done\n", count);
		objects.clear();
	}

}