#pragma once
#include "GenVector.h"
class Camera {
public:
	
	Camera(Vector3 g, Vector3 t, Vector3 origin) {
		//g is the look vector
		//t is the up vector
		w = (-g).normalize();
		u = (t.cross(w)).normalize();
		v = (w.cross(u)).normalize();

		e = Vector3(origin[0], origin[1], origin[2]);
	}

	Vector3 u;
	Vector3 v;
	Vector3 w;
	Vector3 e; //position of the camera
};
