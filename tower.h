#pragma once
#include "glad/glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <SDL_mixer.h>
#include <cstdio>
#include <vector>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "bullet.h"

class Tower
{
public:
	Tower(int type, glm::vec3 t_pos);

	bool isDestroy;

	glm::vec3 tower_pos;
	std::vector<Bullet*> bullets;
	int numBullets;
	Enemy* target_;
	float radius_;

	float timer1;
	float timer2;


	Mix_Chunk* laser = Mix_LoadWAV("audio/laser_Shoot4.wav");

	void shot();
	void update(float dt);
	bool hasTarget();
	void set_target(Enemy* tar);
	void upgrade();

	int getLevel()
	{
		return tower_level_;
	}

	int getTowerType()
	{
		return T_type_;
	}
private:
	int T_type_;
	int tower_level_;
	float frequency_;
};

