// For the test scenes, resolution of 100x100, and fov 90 degree, my
// generator creates the test images. My ray dirs are normalized.

//Hard code resolution for now
#define RES 500


#define _USE_MATH_DEFINES //This enables math constants in Windows
#include <math.h> //Math functions and some constants
#include <stdlib.h>
#include "objLoader.hpp"
#include "GenVector.h"
#include "Buffer.h"
#include "RayGenerator.h"
#include "Ray.h"
#include "Camera.h"
#include "simplePNG.h"
#include "Normals.h"
#include "Shading.h"
#include "ToneMapping.h"
#include "Primitive.h"
#include "Tree.h"
#include <vector>
#include <string>

Camera setupCamera(objLoader* objData) {

	obj_camera* c = objData->camera;

	//printf("camera pos: <%f, %f, %f> \n", objData->vertexList[c->camera_pos_index]->e[0], objData->vertexList[c->camera_pos_index]->e[1], objData->vertexList[c->camera_pos_index]->e[2]);
	//printf("camera look point: <%f, %f, %f> \n", objData->vertexList[c->camera_look_point_index]->e[0], objData->vertexList[c->camera_look_point_index]->e[1], objData->vertexList[c->camera_look_point_index]->e[2]);
	//printf("camera up norm: <%f, %f, %f> \n", objData->normalList[c->camera_up_norm_index]->e[0], objData->normalList[c->camera_up_norm_index]->e[1], objData->normalList[c->camera_up_norm_index]->e[2]);

	Vector3 cameraNorm = Vector3(objData->normalList[c->camera_up_norm_index]->e[0], objData->normalList[c->camera_up_norm_index]->e[1], objData->normalList[c->camera_up_norm_index]->e[2]);
	Vector3 cameraLookPoint = Vector3(objData->vertexList[c->camera_look_point_index]->e[0], objData->vertexList[c->camera_look_point_index]->e[1], objData->vertexList[c->camera_look_point_index]->e[2]);

	Vector3 e = Vector3(objData->vertexList[c->camera_pos_index]->e[0], objData->vertexList[c->camera_pos_index]->e[1], objData->vertexList[c->camera_pos_index]->e[2]);
	
	Vector3 g = cameraLookPoint - e; //look normal
	Vector3 t = cameraNorm; //up normal

	//printf("\n");

	//printf("g: <%f, %f, %f> \n", g[0], g[1], g[2]);
	//printf("t: <%f, %f, %f> \n", t[0], t[1], t[2]);
	//printf("e: <%f, %f, %f> \n", e[0], e[1], e[2]);

	Camera camera = Camera(g, t, e);

	//printf("\n");

	/*printf("u: <%f, %f, %f> \n", camera.u[0], camera.u[1], camera.u[2]);
	printf("v: <%f, %f, %f> \n", camera.v[0], camera.v[1], camera.v[2]);
	printf("w: <%f, %f, %f> \n", camera.w[0], camera.w[1], camera.w[2]);
	printf("e: <%f, %f, %f> \n", camera.e[0], camera.e[1], camera.e[2]);*/

	return camera;
}

std::vector<Primitive*> getPrimitiveArray(objLoader* objData) {
	
	int total = objData->faceCount + objData->sphereCount;

	std::vector<Primitive*> objects;

	//get all spheres
	for (int i = 0; i < objData->sphereCount; i++) {
		objects.push_back(new Sphere(objData->sphereList[i], objData));
	}


	//get all triangles
	for (int i = 0; i < objData->faceCount; i++) {
		objects.push_back(new Triangle(objData->faceList[i], objData));
	}

	return objects;
}

void split(std::vector<Primitive*> objects, int count, objLoader* loader) {
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
		if (objects.at(i)->minPoint[0] < xmin) {
			xmin = objects.at(i)->maxPoint[0];
		}

		if (objects.at(i)->minPoint[1] < ymin) {
			ymin = objects.at(i)->maxPoint[1];
		}

		if (objects.at(i)->minPoint[2] < zmin) {
			zmin = objects.at(i)->maxPoint[2];
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

	//populate the arrays
	for (int i = 0; i < count; i++) {
		if (objects.at(i)->center[axisIndex] < total[axisIndex]) {
			left.push_back(objects.at(i));
			printf("left: %f\n", left.back()->center[0]);
		}
		else {
			right.push_back(objects.at(i));
			printf("right: %f\n", right.back()->center[0]);
		}
	}

}

void printTree(TreeNode* root, int rec) {
	if (root == NULL) {
		return;
	}
	
	std::string s = "";
	for (int i = 0; i < rec; i++) {
		s = s + " ";
	}
	
	printf("%snode %p\n", s, root);
	if(root->primitive != NULL) printf("%sis leaf\n", s);
	printf("%smin: <%f, %f, %f>\n", s, root->minPoint[0], root->minPoint[1], root->minPoint[2]);
	printf("%smax: <%f, %f, %f>\n\n", s, root->maxPoint[0], root->maxPoint[1], root->maxPoint[2]);

	printTree(root->left, rec + 1);
	if(root->left != NULL) printf("\n");
	printTree(root->right, rec + 1);

}

int main(int argc, char ** argv)
{
	//create a frame buffer for RESxRES
	Buffer<Color> buffer = Buffer<Color>(RES, RES);
	Buffer<Vector3> vecbuffer = Buffer<Vector3>(RES, RES);

	//Need at least two arguments (obj input and png output)
	if (argc < 3)
	{
		printf("Usage %s input.obj output.png\n", argv[0]);
		exit(0);
	}

	//load obj from file argv1
	objLoader objData = objLoader();
	printf("loading object data...\n");
	objData.load(argv[1]);

	//print # of objects
	printf("faces: %d\n", objData.faceCount);
	printf("spheres: %d\n", objData.sphereCount);

	//place the objects into a primitive buffer
	std::vector<Primitive*> primitives = getPrimitiveArray(&objData);

	//create the tree
	Tree tree;
	printf("building tree...\n");
	buildTree(tree.root, primitives, objData.faceCount + objData.sphereCount, &objData);
	//printf("---\n");
	//printTree(tree.root, 0);

	//create a camera object
	printf("setting up camera...\n");
	Camera camera = setupCamera(&objData);
	
	//create ray generator
	RayGenerator generator = RayGenerator(&camera, RES, RES);

	printf("starting ray generation...\n");
	//Convert vectors to RGB colors for testing results
	for (int y = 0; y<RES; y++)
	{
		for (int x = 0; x<RES; x++)
		{
			Ray r = generator.getRay(x, y);
			Vector3 d = (r.dir)*255.0;
			
			Vector3 c = Vector3(0, 0, 0); 

			Primitive* hit = tree.hitsNode(r, &objData);
			//printf("hits: %d\n", hit);

			if (hit != NULL) {
				if (hit->face != NULL) {
					//triangle
					Vector3 intersection = r.pointAt(r.triangleIntersection(hit->face, &objData));

					c = getFaceColor(0, tree, &objData, hit->face, intersection, &camera, r);
				}
				else {
					//sphere
					Vector3 intersection = r.pointAt(r.sphereIntersection(hit->sphere, &objData));

					c = getSphereColor(0, tree, &objData, hit->sphere, intersection, &camera, r);
				}
			}
			
			vecbuffer.at(x, y) = c;
		}
	}

	buffer = toneMap(buffer, vecbuffer);

	//Write output buffer to file argv2
	printf("writing data to buffer...\n");
	simplePNG_write(argv[2], buffer.getWidth(), buffer.getHeight(), (unsigned char*)&buffer.at(0, 0));

	printf("finished\n");
	return 0;
}



