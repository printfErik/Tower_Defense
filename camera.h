#pragma once
#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <vector>
#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES

#include <cmath>

class Camera
{
public:
	glm::vec3 position;
	float theta, phi;
	float moveSpeed, turnSpeed;
	Camera(glm::vec3 iposition, float width, float height);
	void updateCamera(float dt, float width, float height);
	glm::mat4 proj;
	glm::mat4 view;
	glm::vec3 negativeMovement;
	glm::vec3 positiveMovement;
	glm::vec2 negativeTurn;
	glm::vec2 positiveTurn;
	float fovy;
	float aspectRatio;
	float nearPlane;
	float farPlane;

private:
	
};