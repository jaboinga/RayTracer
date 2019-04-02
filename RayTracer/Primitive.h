#pragma once
#include "objLoader.hpp"
#include "GenVector.h"

Vector3 objToVec3(obj_vector * objVec)
{
	Vector3 v;
	v[0] = objVec->e[0];
	v[1] = objVec->e[1];
	v[2] = objVec->e[2];
	return v;
}

class Primitive {
public: 
	obj_sphere* sphere = NULL;
	obj_face* face = NULL;
	Vector3 minPoint;
	Vector3 maxPoint;
	Vector3 center;
	int test;

	bool isSphere() {
		return sphere != NULL;
	}

	bool isTriangle() {
		return face != NULL;
	}

};


class Sphere : public Primitive{
public:

	Sphere(obj_sphere* s, objLoader* loader) {
		sphere = s;

		float x = loader->vertexList[sphere->pos_index]->e[0];
		float y = loader->vertexList[sphere->pos_index]->e[1];
		float z = loader->vertexList[sphere->pos_index]->e[2];

		center = Vector3(x, y, z);
		float radius = getRadius(loader);

		Vector3 min = center - radius;
		Vector3 max = center + radius;

		minPoint = min;
		maxPoint = max;
	}

	Vector3 getCenter(objLoader* loader) {
		float x = loader->vertexList[sphere->pos_index]->e[0];
		float y = loader->vertexList[sphere->pos_index]->e[1];
		float z = loader->vertexList[sphere->pos_index]->e[2];

		return Vector3(x, y, z);
	}

	Vector3 getUpNormal(objLoader* loader) {
		return Vector3(loader->normalList[sphere->up_normal_index]->e[0], loader->normalList[sphere->up_normal_index]->e[1], loader->normalList[sphere->up_normal_index]->e[2]);
	}

	float getRadius(objLoader* loader) {
		return getUpNormal(loader).length();
	}

	Vector3 getSphereNormal(Vector3 point, objLoader* objData) {
		Vector3 sphereNormal = (point - objToVec3(objData->vertexList[sphere->pos_index])).normalize();
		return sphereNormal;
	}

};

class Triangle : public Primitive {
public:

	Triangle(obj_face* f, objLoader* loader) {
		face = f;

		//printf("f: %d\n", face);

		Vector3 a = Vector3(loader->vertexList[face->vertex_index[0]]->e[0], loader->vertexList[face->vertex_index[0]]->e[1], loader->vertexList[face->vertex_index[0]]->e[2]);
		Vector3 b = Vector3(loader->vertexList[face->vertex_index[1]]->e[0], loader->vertexList[face->vertex_index[1]]->e[1], loader->vertexList[face->vertex_index[1]]->e[2]);
		Vector3 c = Vector3(loader->vertexList[face->vertex_index[2]]->e[0], loader->vertexList[face->vertex_index[2]]->e[1], loader->vertexList[face->vertex_index[2]]->e[2]);
		
		center =  (a + b + c) / 3;

		float xmin = INFINITY;
		float xmax = -INFINITY;
		float ymin = INFINITY;
		float ymax = -INFINITY;
		float zmin = INFINITY;
		float zmax = -INFINITY;

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

		minPoint = Vector3(xmin, ymin, zmin);
		maxPoint = Vector3(xmax, ymax, zmax);

	}

	Vector3 getCenter() {
		return center;
	}

	Vector3 getTriangleNormal(objLoader* objData) {
		Vector3 p1 = objToVec3(objData->vertexList[face->vertex_index[0]]);
		Vector3 p2 = objToVec3(objData->vertexList[face->vertex_index[1]]);
		Vector3 p3 = objToVec3(objData->vertexList[face->vertex_index[2]]);

		Vector3 u = p2 - p1;
		Vector3 v = p3 - p1;

		Vector3 n = (u.cross(v)).normalize();
		return n;
	}

};