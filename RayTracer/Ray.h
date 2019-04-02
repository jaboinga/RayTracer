#pragma once
#include "GenVector.h"
#include "Primitive.h"
class Ray {
public:

	Ray(Vector3 d, Vector3 o) {
		dir = Vector3(d[0], d[1], d[2]);
		origin = Vector3(o[0], o[1], o[2]);
	}

	Vector3 dir;
	Vector3 origin;

	Vector3 pointAt(float t) {
		return origin + (t * dir);
	}

	int hitsSphere(objLoader* loader) {

		float min = INFINITY;
		int minIndex = -1;

		for (int i = 0; i < loader->sphereCount; i++) {
			obj_sphere* sphere = loader->sphereList[i];
			float val = sphereIntersection(sphere, loader);
			if (val > 0) {
				if (val < min) {
					min = val;
					minIndex = i;
				}
			}
		}
		return minIndex;
	}

	//returns a t value of where the ray intersects with the sphere, or zero if negative or misses
	float sphereIntersection(obj_sphere* sphere, objLoader* loader) {
		Vector3 c = Vector3(loader->vertexList[sphere->pos_index]->e[0], loader->vertexList[sphere->pos_index]->e[1], loader->vertexList[sphere->pos_index]->e[2]);
		Vector3 up_normal = Vector3(loader->normalList[sphere->up_normal_index]->e[0], loader->normalList[sphere->up_normal_index]->e[1], loader->normalList[sphere->up_normal_index]->e[2]);
		float r = up_normal.length();

		Vector3 d = dir;
		Vector3 e = origin;

		//quadratic formula
		float aq = d.dot(d);
		float bq = (2.0f * d).dot(e - c);
		float cq = (e - c).dot(e - c) - (r*r);

		//the math under the square root
		float squarePart = (bq*bq) - (4.0f * aq * cq);

		if (squarePart < 0) {
			//if it's negative, then it misses the sphere
			return 0;
		}

		float tplus = (-bq + sqrt(squarePart)) / (2.0f * aq);
		float tminus = (-bq - sqrt(squarePart)) / (2.0f * aq);

		//doesn't return negative values
		if (tplus < 0) {
			return 0;
		}
		else if (tminus < 0) {
			return tplus;
		}

		return tplus < tminus ? tplus : tminus;
	}

	int hitsTriangle(objLoader* loader) {

		float min = INFINITY;
		int minIndex = -1;

		for (int i = 0; i < loader->faceCount; i++) {

			obj_face* face = loader->faceList[i];

			if (face->vertex_count != 3) {
				//not a triangle
				continue;
			}

			float val = triangleIntersection(face, loader);

			if (val > 0) {
				if (val < min) {
					min = val;
					minIndex = i;
				}
			}
		}
		return minIndex;
	}

	float triangleIntersection(obj_face* face, objLoader* loader) {
		Vector3 a = Vector3(loader->vertexList[face->vertex_index[0]]->e[0], loader->vertexList[face->vertex_index[0]]->e[1], loader->vertexList[face->vertex_index[0]]->e[2]);
		Vector3 b = Vector3(loader->vertexList[face->vertex_index[1]]->e[0], loader->vertexList[face->vertex_index[1]]->e[1], loader->vertexList[face->vertex_index[1]]->e[2]);
		Vector3 c = Vector3(loader->vertexList[face->vertex_index[2]]->e[0], loader->vertexList[face->vertex_index[2]]->e[1], loader->vertexList[face->vertex_index[2]]->e[2]);

		Vector3 n = (b - a).cross(c - b);

		float t = ((a - origin).dot(n) / (dir.dot(n)));
		Vector3 x = pointAt(t);

		//now check to see if t is in the triangle
		if (((b - a).cross(x - a)).dot(n) > 0
			&& ((c - b).cross(x - b)).dot(n) > 0
			&& ((a - c).cross(x - c)).dot(n) > 0) {
			return t;
		}

		//not in triangle
		return 0;
	}
};
