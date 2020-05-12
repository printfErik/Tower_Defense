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

class Roadmap
{
public:
	
	// PRM 
	std::vector<std::vector<int>> neighbors;
	std::vector<glm::vec2> nodes;
	std::vector<std::vector<int>> edges;
	std::vector<int> Path;

	float* vertices;

	Roadmap(std::vector<glm::vec2> o_pos, std::vector<float> o_radius);
	//~Roadmap();
	void searchPath(float e);
	float distance(int index1, int index2);
	void PRM();
	bool isEdgeAvaliavle(float x1, float y1, float x2, float y2);
	void computeEdges();

	void updateVerts();

	int num_edges_;

	bool canReachGoal() {
		auto src_flag = std::find(Path.begin(), Path.end(), 0);
		auto goal_flag = std::find(Path.begin(), Path.end(), 1);

		return (src_flag != Path.end()) && (goal_flag != Path.end());
	};

private:
	// Obstacles
	std::vector<glm::vec2> o_pos_;
	std::vector<float> o_radius_;

	
	

	glm::vec2 start_ = glm::vec2(-19.5f, 19.5f );
	glm::vec2 goal_ = glm::vec2( 19.5f, -19.5f );
	int num_nodes_ = 100;

	float enemy_rad_ = 0.5f;
};

