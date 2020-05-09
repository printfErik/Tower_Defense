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

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <math.h>
#include <deque>

#include "Enemy.h"
#include "KDTree.h"
#include "Roadmap.h"


class SPath
{
public:

	// Attributes
	int numEnemy = 1;


	// Obstacles
	std::vector<glm::vec2> o_pos;
	std::vector<float> o_radius;

	// General 
	float* vertices_;
	Roadmap* roadmap_;
	std::vector<Enemy*> enemys_;
	int n_edges_;

	SPath(int num_o);
	void init();
	void update(float dt);
	void March();
	void DeleteAll();
	
private:
	float max_speed = 3.f;
	float agent_radius = 0.5f;
	float goal_speed = 10;
	float goal_cof = 40;
	float sep_r = 2.f;
	float sep_cof = 20.f;
	float coh_r = 1;
	float coh_cof = 5;
	float alig_r = 1;
	float alig_cof = 4;

	
	int path_size_;
};

