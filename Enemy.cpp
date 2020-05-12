#include "Enemy.h"

Enemy::Enemy(int path_size)
{
	health = 2900;
	isDead = false;
	isGoal = false;
	ePos = glm::vec2(-21.5f, 21.5f);
	eVel = glm::vec2(0.f, 0.f);
	dir = glm::vec2(0.f, 1.f);
	rotation = 0;
	eCurrent = path_size - 2;

	
}