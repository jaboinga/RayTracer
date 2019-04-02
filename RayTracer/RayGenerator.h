#pragma once
#include "Camera.h"
#include "Buffer.h"
#include "Ray.h"
class RayGenerator {

public:
	RayGenerator(Camera* c, int w, int h) {
		camera = c;
		width = w;
		height = h;

		rayBuffer = Buffer<Ray>(w, h);
	}

	Ray getRay(int x, int y) {

		//screen height
		int nx = width;
		int ny = height;

		//distances from camera
		//assuming it's centered in the frame
		float l = -nx / 2;
		float r = nx / 2;
		float b = ny / 2;
		float t = -ny / 2;

		//we'll hardcode this for now
		float fov = 90 * (M_PI / 180);

		float d = (1 / tan(fov / 2)) * (nx / 2);

		int i = x;
		int j = y;

		float u = l + (((r - l)*(i + 0.5f)) / (nx));
		float v = b + (((t - b)*(j + 0.5f)) / (ny));

		Vector3 rayDirection = camera->e + (u*camera->u) + (v*camera->v) - (d*camera->w);
		Vector3 rayOrigin = camera->e; //same as camera position

		Ray ray = Ray((rayDirection - camera->e).normalize(), rayOrigin);

		rayBuffer.at(i, j) = ray;

		return ray;
	}

private:

	Camera* camera;
	int width; 
	int height; 

	Buffer<Ray> rayBuffer;

};