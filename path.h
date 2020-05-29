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

#include "Enemy.h"
#include "Roadmap.h"
#include "tower.h"
#include "EconSystem.h"
#include "Particle.h"

class SPath
{
public:

	// Enemy
	int numEnemy;
	std::vector<Enemy*> enemys_;
	Roadmap* roadmap_;

	// Obstacles
	int numObstacle;
	std::vector<glm::vec2> o_pos;
	std::vector<float> o_radius;
	std::vector<bool> o_locked;

	// Tower
	int numTower;
	std::vector<Tower*> towers_;

	// Particles
	Particle* particle_;
	bool FIREON;

	// General 
	float* vertices_;	
	int n_edges_;
	Mix_Chunk * explosion = Mix_LoadWAV("audio/Explosion3.wav");
	Mix_Chunk* ohno = Mix_LoadWAV("audio/ohno.wav");

	int PICKED = -1;
	bool BUILD_MOD = false;
	
	EconSystem* econ_system_;

	SPath(int num_o);
	void init();
	void update(float dt);
	void March();
	void DeleteAll();

	void buildTower(int picked,int type);
	void buildObstacle();
	void largeObstacle();
	void shrinkObstacle();
	void nextObstacle();
	void lastObstacle();
	void moveObstacle(int type);
	bool obsLock();
	void deleteObstacle();

	void upgradeTower();


	void fire();

	void printMsg(const std::string msg) {
		game_msg = msg;
		game_msg_time = SDL_GetTicks();
	};
	std::string getMsg() { return game_msg; };
	size_t getMsgTime() { return game_msg_time; };

private:

	std::string game_msg;
	size_t game_msg_time;

	float max_speed = 1.f;
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

