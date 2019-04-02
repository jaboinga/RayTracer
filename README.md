# Ray Tracer
This is a ray tracer that inputs an .obj file using triangles and spheres, with an assocciated .mtl file for color.

Based off of this, the program renders 3D objects based off of camera and light positions from the .obj, and colors accordingly. It includes shadows and reflections, using Phong shading properties.

A few sample files are under /testobjs along with sample output images.

To trace through the code, start with main.cpp, it will walk you through each step. 

## How it works: 
1. Calculate the camera position based off of the objData.
2. Create a BVH tree based on all objects in the scene (spheres or triangles).
3. Then based off of a resolution size, we generate a ray shooting outward from the camera for every pixel.
4. Taking these rays, we see if they intersect an object in the scene (found by traversing the BVH tree). If they do, color it accordingly.
5. While this is happening, we check to see if there are any shadows by creating a new ray from the intersected point to a light source in the scene. If there is one, color accordingly. 
6. Reflections take the ray intersecting with the point, and reflect them against the object's normal vector. Then it recursively ray traces against that ray until a recursion cap has been hit or there is no reflectance on the object it hits. 
7. Finally, all the color values are added to a buffer for the output .png, and tone-mapped. 

## Known issues:
Note: some of the current shadow code is broken for certain triangles. Go look at blue_sphere.png and see for yourself. If you turn off shadows, the image will render perfectly (there are no shadows in this scene anyway). 

## FYI
Some of this code (mainly GenVector, objLoader, and objParser) was written by my professor as starter code, Micah Taylor. 

Also, some of the splits for the BVH tree could have been calculated better. 
