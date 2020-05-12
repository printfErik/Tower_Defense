#include "tower.h"

Tower::Tower(int type, glm::vec3 t_pos)
{
	T_type_ = type;
	tower_pos = t_pos;
	isDestroy = false;
	numBullets = 0;
	radius_ = 200.f;
	target_ = NULL;
	timer1 = SDL_GetTicks();
	tower_level_ = 1;
	frequency_ = 2000.f;
}

void Tower::upgrade()
{
	if (T_type_ == 1)
	{
		if (tower_level_ > 3)
		{
			return;
		}
		tower_level_++;
		frequency_ /= 2.f;
		radius_ += 100.f;
	}
	
}

void Tower::shot()
{
	if (target_ == NULL)
	{
		return;
	}
	if (T_type_ == 1)
	{
		Mix_PlayChannel(-1, laser, 0);
		numBullets += 1;
		Bullet* new_bullet = new Bullet(1, target_);
		glm::vec3 muzzle = tower_pos + glm::vec3(0.f, 5.f, 0.f);
		new_bullet->b_pos = muzzle;
		new_bullet->init_pos = muzzle;
		new_bullet->alpha = 0;
		bullets.push_back(new_bullet);
	}
	else if (T_type_ == 2)
	{
		numBullets += 1;
		Bullet* new_bullet = new Bullet(2, target_);
		glm::vec3 muzzle = glm::vec3(target_->ePos.x, -3.f, target_->ePos.y);
		new_bullet->b_pos = muzzle;
		new_bullet->init_pos = muzzle;
		new_bullet->alpha = 0;
		bullets.push_back(new_bullet);
	}

}

void Tower::update(float dt)
{
	float timer2 = SDL_GetTicks();
	if (timer2 - timer1 > frequency_)
	{
		shot();
		timer1 = timer2;
	}
	for (int i = numBullets - 1; i >= 0; i--)
	{	
		bullets[i]->b_pos = bullets[i]->init_pos * (1 - bullets[i]->alpha) + bullets[i]->real * (bullets[i]->alpha);
		bullets[i]->alpha += 1.2 * bullets[i]->parameter * dt;

		if (bullets[i]->alpha >= 1)
		{
			bullets[i]->target->health -= bullets[i]->damage;
			numBullets -= 1;
			bullets.erase(bullets.begin() + i);
		}
	}
}

bool Tower::hasTarget()
{
	if (target_ == NULL)
	{
		return false;
	}
	if (target_->isDead)
	{
		target_ = NULL;
		return false;
	}
	float dx, dy;
	dx = target_->ePos.x - tower_pos.x;
	dy = target_->ePos.y - tower_pos.z;
	if (dx * dx + dy * dy > radius_)
	{
		target_ = NULL;
		//bullets.clear();
		return false;
	}
	return true;
}

void Tower::set_target(Enemy* tar)
{
	target_ = tar;
}
