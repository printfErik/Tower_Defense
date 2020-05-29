#pragma once
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Enemy.h"

class Bullet
{
public:
	Bullet(int type, Enemy* tar) {
		bullet_type = type;
		target = tar;
		alpha = 0;
		if (bullet_type == 1)
		{
			damage = 100.f;
			real = glm::vec3(target->ePos.x, 0.5, target->ePos.y);
			parameter = 1.f;
		}
		else if (bullet_type == 2)
		{
			damage = 300.f;
			real = glm::vec3(target->ePos.x, 4, target->ePos.y);
			parameter = 0.5f;
		}
		else if (bullet_type == 3)
		{
			damage = 400.f;
			real = glm::vec3(target->ePos.x, 0.5, target->ePos.y);
			parameter = 0.6f;
		}
	};
	int bullet_type;
	glm::vec3 init_pos;
	glm::vec3 b_pos;
	Enemy* target;
	float alpha;
	float parameter;

	glm::vec3 real;
	float damage;

	void set_init_pos(glm::vec3 p);
};
