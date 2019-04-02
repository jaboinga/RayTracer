#pragma once
#include "GenVector.h"
#include "Buffer.h"

Buffer<Color> toneMap(Buffer<Color> buffer, Buffer<Vector3> vecbuffer) {

	float largest = 0;

	for (int i = 0; i < vecbuffer.getHeight(); i++) {
		for (int j = 0; j < vecbuffer.getWidth(); j++) {

			Vector3 c = vecbuffer.at(i, j);

			if (c[0] > largest) {
				largest = c[0];
			}

			if (c[1] > largest) {
				largest = c[1];
			}

			if (c[2] > largest) {
				largest = c[2];
			}

		}
	}

	for (int i = 0; i < vecbuffer.getHeight(); i++) {
		for (int j = 0; j < vecbuffer.getWidth(); j++) {

			Vector3 v = vecbuffer.at(i, j);

			float newr = v[0] / largest;
			float newg = v[1] / largest;
			float newb = v[2] / largest;

			Vector3 v2 = Vector3(newr, newg, newb) * 255;

			Color c = Color(v2[0], v2[1], v2[2]);

			buffer.at(i, j) = c;

			//printf("<%d, %d, %d>\n", buffer.at(i, j)[0], buffer.at(i, j)[1], buffer.at(i, j)[2]);
		}
	}
	return buffer;
}