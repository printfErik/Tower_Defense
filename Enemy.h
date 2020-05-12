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

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
class Enemy
{
public:
	Enemy(int path_size);
	float timer1;
	float health;
	bool isDead;
	bool isGoal;
	int eCurrent;
	glm::vec2 dir;
	float rotation;
	bool locked = false;
	glm::vec2 ePos;
	glm::vec2 eVel;
};

