#pragma once
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
class Enemy
{
public:
	Enemy(int path_size);
	float health  ;
	bool isDead;
	int eCurrent;
	glm::vec2 ePos;
	glm::vec2 eVel;
};

