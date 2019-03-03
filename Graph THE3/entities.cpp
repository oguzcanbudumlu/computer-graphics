#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#define SPEEDSTEP 0.001f
#define YAWDEGREE 0.003f
#define PITCHDEGREE 0.003f
#define HEIGHTSTEP 0.1f

using namespace std;
using namespace glm;

class Light {
public:
	vec3 position;
	vec3 intensity;

	Light() {}

	Light(int width, int height) {
		position = vec3(width / 2, width + height, height / 2);
		intensity = vec3(1, 1, 1);
		cout << "The light source has been created at position (" << position.x << ", " << position.y << ", " << position.z << ")." << endl; 
	}
};

class Camera {
public:
	vec3 position;
	vec3 gaze;
	vec3 v;
	vec3 u;
	double speed;

	Camera() {}
	
	Camera(int width, int height) {
		position = vec3(width / 2, width / 10, width / -4);
		gaze = vec3(0, 0, 1);
		v = vec3(0.0, 1.0, 0.0);
		u = vec3(-1, 0, 0);
		speed = 0;
		cout << "The camera has been created at position (" << position.x << ", " << position.y << ", " << position.z << ")." << endl; 
	}

	void speedUp() {
		speed += SPEEDSTEP;
		cout << "The speed of the camera has been increased. Speed : " << speed << endl;
	}

	void speedDown() {
		if (speed > 0) {
			speed -= SPEEDSTEP;
			cout << "The speed of the camera has been decreased. Speed : " << speed << endl;
		}
	}

	void turnLeft() {
		mat4 rotationMatrix = mat4(1.0f);
		rotationMatrix = rotate(rotationMatrix, YAWDEGREE, v);
		gaze = vec3(rotationMatrix * vec4(gaze, 1));
		u = vec3(rotationMatrix * vec4(u, 1));
		gaze = normalize(gaze);
		u = normalize(u);
		cout << "The camera has been turned left." << endl;
	}

	void turnRight() {
		mat4 rotationMatrix = mat4(1.0f);
		rotationMatrix = rotate(rotationMatrix, -YAWDEGREE, v);
		gaze = vec3(rotationMatrix * vec4(gaze, 1));
		u = vec3(rotationMatrix * vec4(u, 1));
		gaze = normalize(gaze);
		u = normalize(u);
		cout << "The camera has been turned right." << endl;
	}

	void goDown() {
		mat4 rotationMatrix = mat4(1.0f);
		rotationMatrix = rotate(rotationMatrix, -PITCHDEGREE, u);
		gaze = vec3(rotationMatrix * vec4(gaze, 1));
		v = vec3(rotationMatrix * vec4(v, 1));
		gaze = normalize(gaze);
		v = normalize(v);
		cout << "The camera has been moved down." << endl;
	}

	void goUp() {
		mat4 rotationMatrix = mat4(1.0f);
		rotationMatrix = rotate(rotationMatrix, PITCHDEGREE, u);
		gaze = vec3(rotationMatrix * vec4(gaze, 1));
		v = vec3(rotationMatrix * vec4(v, 1));
		gaze = normalize(gaze);
		v = normalize(v);
		cout << "The camera has been moved up." << endl;
	}

	void go() {
		position = position + speed * gaze;
	}
};

class Terrain {
public:
	vec3* vertices;
	int triangleCount;
	double heightFactor;

	Terrain() {}

	Terrain(int width, int height) {
		cout << "The terrain has been created." << endl;
		triangleCount = 2 * width * height;
		heightFactor = 10.0f;
		getVertices(width, height);
	}

	void getVertices(int width, int height) {
		vertices = new vec3[3 * triangleCount];
		vec3 v1, v2, v3, v4;
    	int index = 0;
    	for (int i = 0; i < width - 1; ++i) {
        	for (int j = 0; j < height - 1; ++j) {
            	v1 = vec3(i, 0, j);
            	v2 = vec3(i + 1, 0, j);
            	v3 = vec3(i, 0, j + 1);
            	v4 = vec3(i + 1, 0, j + 1);

            	vertices[index++] = v1;
            	vertices[index++] = v4;
            	vertices[index++] = v2;

            	vertices[index++] = v1;
            	vertices[index++] = v3;
            	vertices[index++] = v4;
        	}
    	}
    	cout << "The vertices for the terrain have created." << endl;
	}

	void increaseHeightFactor() {
		heightFactor += HEIGHTSTEP;
		cout << "The height factor has been increased." << endl;
	}

	void decreaseHeightFactor() {
		heightFactor -= HEIGHTSTEP;
		cout << "The height factor has been decreased." << endl;
	}
};

class Space {
public:
	double projectionAngle;
	double aspectRatio;
	double near;
	double far;
	Light* light;
	Camera* camera;
	Terrain* terrain;

	Space() {}

	Space(int width, int height) {
		projectionAngle = 45;
		aspectRatio = 1;
		near = 0.1;
		far = 1000;
		light = new Light(width, height);
		camera = new Camera(width, height);
		terrain = new Terrain(width, height);
		cout << "The space has been created." << endl; 
	}

	mat4 getProjectionMatrix() {
		return perspective(projectionAngle, aspectRatio, near, far);
	}

	mat4 getMVMatrix() {
		return lookAt(camera->position, vec3(            
            camera->position.x + camera->gaze.x * near,
			camera->position.y + camera->gaze.y * near,
			camera->position.z + camera->gaze.z * near), 
			camera->v);
	}

	mat4 getMVPMatrix() {
		return getProjectionMatrix() * getMVMatrix();
	}
};