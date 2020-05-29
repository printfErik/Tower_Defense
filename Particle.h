#pragma once
#include "glad/glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#include <SDL_mixer.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <math.h>
#include <deque>

class Particle
{
public:
	Particle(int n,float x, float y, float z);
	int numP;
	float genRate;
	int maxP = 200;
	float x_;
	float y_;
	float z_;

	void genPos(float x, float y, float z)
	{
		x_ = x;
		y_ = y;
		z_ = z;
	}
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> vel;
	glm::vec3 acc;
	std::vector<float> life;
	std::vector<float> size;

	void computePhy(float dt);
	float random(float l, float h);
	glm::vec3 randomYDisk(float r, float ix, float iy, float iz);
};


