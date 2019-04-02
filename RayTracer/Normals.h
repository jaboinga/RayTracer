#pragma once

#include "GenVector.h"
#include "objLoader.hpp"

//This might be helpful to convert from obj vectors to GenVectors
Vector3 objToVec3(obj_vector const * objVec)
{
	Vector3 v;
	v[0] = objVec->e[0];
	v[1] = objVec->e[1];
	v[2] = objVec->e[2];
	return v;
}

Vector3 getTriangleNormal(obj_face* face, objLoader* objData) {
	Vector3 p1 = objToVec3(objData->vertexList[face->vertex_index[0]]);
	Vector3 p2 = objToVec3(objData->vertexList[face->vertex_index[1]]);
	Vector3 p3 = objToVec3(objData->vertexList[face->vertex_index[2]]);

	Vector3 u = p2 - p1;
	Vector3 v = p3 - p1;

	Vector3 n = (u.cross(v)).normalize();
	return n;
}

Vector3 getSphereNormal(Vector3 point, obj_sphere* sphere, objLoader* objData) {
	Vector3 sphereNormal = (point - objToVec3(objData->vertexList[sphere->pos_index])).normalize();
	return sphereNormal;
}