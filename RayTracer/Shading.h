#pragma once
#include "objLoader.hpp"
#include "GenVector.h"
#include "Normals.h"
#include "Ray.h"
#include <float.h>
#include "Tree.h"

Vector3 getSphereColor(int recursion, Tree tree, objLoader* objData, obj_sphere* sphere, Vector3 point, Camera* camera, Ray r);
Vector3 getFaceColor(int recursion, Tree tree, objLoader* objData, obj_face* face, Vector3 point, Camera* camera, Ray r);

Vector3 getFaceAmbient(objLoader* objData, obj_face* face) {

	float r = 0;
	float g = 0;
	float b = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		obj_light_point* light = objData->lightPointList[i];

		r += objData->materialList[face->material_index]->amb[0] * objData->materialList[light->material_index]->amb[0];
		g += objData->materialList[face->material_index]->amb[1] * objData->materialList[light->material_index]->amb[1];
		b += objData->materialList[face->material_index]->amb[2] * objData->materialList[light->material_index]->amb[2];

	}

	Vector3 c = Vector3(r, g, b);
	//printf("ambient: <%f, %f, %f>\n", r, g, b);
	return c;
}

Vector3 getSphereAmbient(objLoader* objData, obj_sphere* sphere) {

	float r = 0;
	float g = 0;
	float b = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		obj_light_point* light = objData->lightPointList[i];

		r += objData->materialList[sphere->material_index]->amb[0] * objData->materialList[light->material_index]->amb[0];
		g += objData->materialList[sphere->material_index]->amb[1] * objData->materialList[light->material_index]->amb[1];
		b += objData->materialList[sphere->material_index]->amb[2] * objData->materialList[light->material_index]->amb[2];

	}

	Vector3 c = Vector3(r, g, b);
	//printf("ambient: <%f, %f, %f>\n", r, g, b);
	return c;
}

Vector3 getSphereDiffuse(objLoader* objData, obj_sphere* sphere, Vector3 point) {

	Vector3 sphereNormal = getSphereNormal(point, sphere, objData);

	float reflectance = objData->materialList[sphere->material_index]->reflect;

	float r = 0;
	float g = 0;
	float b = 0;

	int i = 0;
	for (int i = 0; i < objData->lightPointCount; i++) {
		obj_light_point* light = objData->lightPointList[i];
		Vector3 lightPoint = objToVec3(objData->vertexList[objData->lightPointList[i]->pos_index]);

		Vector3 l = (lightPoint - point).normalize();

		float ldotn = l.dot(sphereNormal);
		//printf("dot: %f\n", ldotn);
		if (ldotn < 0) {
			continue;
		}

		r += objData->materialList[sphere->material_index]->diff[0] * objData->materialList[light->material_index]->diff[0] * ldotn;
		g += objData->materialList[sphere->material_index]->diff[1] * objData->materialList[light->material_index]->diff[1] * ldotn;
		b += objData->materialList[sphere->material_index]->diff[2] * objData->materialList[light->material_index]->diff[2] * ldotn;
	}

	Vector3 c = Vector3(r, g, b);

	//printf("diffuse: <%f, %f, %f>\n", c[0], c[1], c[2]);

	return c;
}

Vector3 getFaceDiffuse(objLoader* objData, obj_face* face, Vector3 point) {

	Vector3 faceNormal = getTriangleNormal(face, objData);

	float r = 0;
	float g = 0;
	float b = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		obj_light_point* light = objData->lightPointList[i];
		Vector3 lightPoint = objToVec3(objData->vertexList[objData->lightPointList[i]->pos_index]);

		Vector3 l = (lightPoint - point).normalize();

		float ldotn = l.dot(faceNormal);
		//printf("%f\n", ldotn);

		if (ldotn < 0) {
			continue;
		}

		//printf("<%f, %f, %f>\n", objData->materialList[light->material_index]->diff[0], objData->materialList[light->material_index]->diff[1], objData->materialList[light->material_index]->diff[2]);
		r += objData->materialList[face->material_index]->diff[0] * objData->materialList[light->material_index]->diff[0] * ldotn;
		g += objData->materialList[face->material_index]->diff[1] * objData->materialList[light->material_index]->diff[1] * ldotn;
		b += objData->materialList[face->material_index]->diff[2] * objData->materialList[light->material_index]->diff[2] * ldotn;
	}

	Vector3 c = Vector3(r, g, b);
	//printf("<%f, %f, %f>\n", r, g, b);
	return c;
}

Vector3 getSphereSpecular(objLoader* objData, obj_sphere* sphere, Vector3 point, Camera* camera) {

	Vector3 sphereNormal = getSphereNormal(point, sphere, objData);

	float sum = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		Vector3 lightPoint = objToVec3(objData->vertexList[objData->lightPointList[i]->pos_index]);

		Vector3 lightNorm = (point - lightPoint).normalize();
		Vector3 rv = lightNorm.reflect(sphereNormal);

		float cos = rv.dot((point - camera->e).normalize());

		obj_material* mat = objData->materialList[sphere->material_index];

		//alpha is shiny
		float a = mat->glossy;
		float p = powf(cos, a);

		sum += p;
	}

	//printf("<%f, %f, %f>\n", sum * 255, sum * 255, sum * 255);
	return Vector3(sum, sum, sum);
}

Vector3 getFaceSpecular(objLoader* objData, obj_face* face, Vector3 point, Camera* camera) {

	Vector3 faceNormal = getTriangleNormal(face, objData);

	float sum = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		Vector3 lightPoint = objToVec3(objData->vertexList[objData->lightPointList[i]->pos_index]);

		Vector3 lightNorm = (point - lightPoint).normalize();
		Vector3 rv = lightNorm.reflect(faceNormal);

		float cos = rv.dot((point - camera->e).normalize());

		obj_material* mat = objData->materialList[face->material_index];

		//alpha is shiny
		float a = mat->shiny;

		if (a == 0) {
			continue;
		}

		float p = powf(cos, a);

		sum += p;
	}

	//printf("<%f, %f, %f>\n", sum * 255, sum * 255, sum * 255);
	return Vector3(sum, sum, sum);
}

int inShadow(objLoader* objData, Tree tree, Vector3 point, Vector3 normal) {

	int count = 0;

	for (int i = 0; i < objData->lightPointCount; i++) {
		Vector3 lightPoint = objToVec3(objData->vertexList[objData->lightPointList[i]->pos_index]);

		//from the point given to the light (direct)
		//we bump it up a little bit by the normal so it doesn't detect the point that it came from
		Ray lightRay = Ray(lightPoint - point, point + (normal * 0.1));
		float lightDistance = (lightPoint - point).length();

		
		Primitive* hit = tree.hitsNode(lightRay, objData);
		if (hit == NULL) {
			return 0;
		}
		else if (hit->face != NULL) {
			obj_face* plane = hit->face;
			float intersect = lightRay.triangleIntersection(plane, objData);
			Vector3 intersectionPoint = lightRay.pointAt(intersect);
			Vector3 planeRay = point - intersectionPoint;
			float planeDistance = planeRay.length();
			if (planeDistance < lightDistance) {
				count++;
			}
		}
		else {
			obj_sphere* sphere = hit->sphere;
			//get the t value where it intersects
			float intersect = lightRay.sphereIntersection(sphere, objData);
			Vector3 intersectionPoint = lightRay.pointAt(intersect);

			Vector3 sphereRay = intersectionPoint - point;
			float sphereDistance = sphereRay.length();

			if (sphereDistance < lightDistance) {
				count++;
			}
		}


		////check sphere intersections
		//int sphereIntersect = lightRay.hitsSphere(objData);
		//if (sphereIntersect >= 0) {
		//	//get which sphere it hits
		//	obj_sphere* sphere = objData->sphereList[sphereIntersect];
		//	//get the t value where it intersects
		//	float intersect = lightRay.sphereIntersection(sphere, objData);
		//	Vector3 intersectionPoint = lightRay.pointAt(intersect);

		//	Vector3 sphereRay = intersectionPoint - point;
		//	float sphereDistance = sphereRay.length();

		//	if (sphereDistance < lightDistance) {
		//		count++;
		//	}
		//}

		////check plane intersection
		//int planeIntersect = lightRay.hitsTriangle(objData);
		//if (planeIntersect >= 0) {
		//	obj_face* plane = objData->faceList[planeIntersect];
		//	float intersect = lightRay.triangleIntersection(plane, objData);
		//	Vector3 intersectionPoint = lightRay.pointAt(intersect);
		//	Vector3 planeRay = point - intersectionPoint;
		//	float planeDistance = planeRay.length();
		//	if (planeDistance < lightDistance) {
		//		count++;
		//	}
		//}

	}

	return count;
}

Vector3 getSphereReflection(int recursion, Tree tree, objLoader* objData, obj_sphere* sphere, Vector3 point, Camera* camera) {

	int recCount = 3;

	if (recursion >= recCount) {
		return Vector3(0, 0, 0);
	}

	Vector3 sphereNormal = getSphereNormal(point, sphere, objData);
	obj_material* sphereMat = objData->materialList[sphere->material_index];

	if (sphereMat->reflect == 0) {
		//no reflection
		return Vector3(0, 0, 0);
	}

	//printf("%f\n", sphereMat->reflect);

	Ray ray = Ray(camera->e - point, point);

	Vector3 currentPoint = point;

	//reflect the ray
	Vector3 reflect = ray.dir.reflect(sphereNormal);

	ray = Ray(-reflect, currentPoint + (sphereNormal * 0.1));

	Primitive* hit = tree.hitsNode(ray, objData);

	if (hit == NULL) {
		return Vector3(0, 0, 0);
	}
	else if (hit->face != NULL) {
		obj_face* hitTriangle = hit->face;
		float t = ray.triangleIntersection(hitTriangle, objData);
		currentPoint = ray.pointAt(t);

		Vector3 color = getFaceColor(recursion + 1, tree, objData, hitTriangle, currentPoint, camera, ray);
		return -color * sphereMat->reflect;
	}
	else {
		obj_sphere* hitSphere = hit->sphere;
		float t = ray.sphereIntersection(hitSphere, objData);
		currentPoint = ray.pointAt(t);
		Vector3 color = getSphereColor(recursion + 1, tree, objData, hitSphere, currentPoint, camera, ray);
		return -color * sphereMat->reflect;
	}

	
	//int sphereIntersect = ray.hitsSphere(objData);
	//int triangleIntersect = ray.hitsTriangle(objData);

	//if (sphereIntersect >= 0) {
	//	obj_sphere* hitSphere = objData->sphereList[sphereIntersect];
	//	float t = ray.sphereIntersection(hitSphere, objData);
	//	currentPoint = ray.pointAt(t);
	//	Vector3 color = getSphereColor(recursion + 1, tree, objData, hitSphere, currentPoint, camera, ray);
	//	//printf("<%f, %f, %f>\n", color[0] * sphereMat->reflect, color[1] * sphereMat->reflect, color[2] * sphereMat->reflect);
	//	return -color * sphereMat->reflect;

	//}
	//else if (triangleIntersect >= 0) {
	//	obj_face* hitTriangle = objData->faceList[triangleIntersect];
	//	float t = ray.triangleIntersection(hitTriangle, objData);
	//	currentPoint = ray.pointAt(t);

	//	Vector3 color = getFaceColor(recursion + 1, tree, objData, hitTriangle, currentPoint, camera, ray);
	//	return -color * sphereMat->reflect;
	//}

	////hit nothing
	//return Vector3(0, 0, 0);
	
}

Vector3 getFaceReflection(int recursion, Tree tree, objLoader* objData, obj_face* face, Vector3 point, Camera* camera) {

	int recCount = 3;

	if (recursion >= recCount) {
		return Vector3(0, 0, 0);
	}

	Vector3 triangleNormal = getTriangleNormal(face, objData);
	obj_material* sphereMat = objData->materialList[face->material_index];

	if (sphereMat->reflect == 0) {
		//no reflection
		return Vector3(0, 0, 0);
	}

	//printf("%f\n", sphereMat->reflect);

	Ray ray = Ray(camera->e - point, point);

	Vector3 currentPoint = point;

	//reflect the ray
	Vector3 reflect = ray.dir.reflect(triangleNormal);

	ray = Ray(reflect, currentPoint + (triangleNormal * 0.1));

	
	Primitive* hit = tree.hitsNode(ray, objData);

	if (hit == NULL) {
		return Vector3(0, 0, 0);
	}
	else if (hit->face != NULL) {
		obj_face* hitTriangle = hit->face;
		float t = ray.triangleIntersection(hitTriangle, objData);
		currentPoint = ray.pointAt(t);

		Vector3 color = getFaceColor(recursion + 1, tree, objData, hitTriangle, currentPoint, camera, ray);
		return -color * sphereMat->reflect;
	}
	else {
		obj_sphere* hitSphere = hit->sphere;
		float t = ray.sphereIntersection(hitSphere, objData);
		currentPoint = ray.pointAt(t);
		Vector3 color = getSphereColor(recursion + 1, tree, objData, hitSphere, currentPoint, camera, ray);
		return -color * sphereMat->reflect;
	}
	
	
	//int sphereIntersect = ray.hitsSphere(objData);
	//int triangleIntersect = ray.hitsTriangle(objData);

	//if (sphereIntersect >= 0) {
	//	obj_sphere* hitSphere = objData->sphereList[sphereIntersect];
	//	float t = ray.sphereIntersection(hitSphere, objData);
	//	currentPoint = ray.pointAt(t);
	//	Vector3 color = getSphereColor(recursion + 1, tree, objData, hitSphere, currentPoint, camera, ray);
	//	//printf("<%f, %f, %f>\n", color[0] * sphereMat->reflect, color[1] * sphereMat->reflect, color[2] * sphereMat->reflect);
	//	return -color * sphereMat->reflect;

	//}
	//else if (triangleIntersect >= 0) {
	//	obj_face* hitTriangle = objData->faceList[triangleIntersect];
	//	float t = ray.triangleIntersection(hitTriangle, objData);
	//	currentPoint = ray.pointAt(t);

	//	Vector3 color = getFaceColor(recursion + 1, tree, objData, hitTriangle, currentPoint, camera, ray);
	//	return -color * sphereMat->reflect;
	//}

	////hit nothing
	//return Vector3(0, 0, 0);
	
}

Vector3 getFaceColor(int recursion, Tree tree, objLoader* objData, obj_face* face, Vector3 point, Camera* camera, Ray r) {
	Vector3 ambient = getFaceAmbient(objData, face);
	Vector3 diffuse = getFaceDiffuse(objData, face, point);
	Vector3 spec = getFaceSpecular(objData, face, point, camera);

	Vector3 reflection = getFaceReflection(recursion, tree, objData, face, point, camera);

	Vector3 normal = getTriangleNormal(face, objData);

	int shadowCount = inShadow(objData, tree, point, normal);

	if (shadowCount > 0) {
		return ambient - reflection + ((diffuse + spec) / (shadowCount + 1));
	}

	Vector3 total = (ambient + diffuse + spec) - reflection;
	return total;
}

Vector3 getSphereColor(int recursion, Tree tree, objLoader* objData, obj_sphere* sphere, Vector3 point, Camera* camera, Ray r) {
	Vector3 ambient = getSphereAmbient(objData, sphere);
	Vector3 diffuse = getSphereDiffuse(objData, sphere, point);
	Vector3 spec = getSphereSpecular(objData, sphere, point, camera);

	Vector3 reflection = getSphereReflection(recursion, tree, objData, sphere, point, camera);

	Vector3 normal = getSphereNormal(point, sphere, objData);

	int shadowCount = inShadow(objData, tree, point, normal);

	if (shadowCount > 0) {
		return ambient - reflection + ((diffuse + spec) / (shadowCount + 1));
	}

	Vector3 total = (ambient + diffuse + spec) - reflection;
	return total;
}
