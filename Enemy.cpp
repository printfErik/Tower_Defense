#include "Enemy.h"

Enemy::Enemy(int path_size)
{
	ePos = glm::vec2(-19.5f, 19.5f);
	eVel = glm::vec2(0.f, 0.f);
	eCurrent = path_size - 2;
}