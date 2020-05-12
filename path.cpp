#include "path.h"

SPath::SPath(int num_o)
{
	//srand(time(NULL));
	//num_obstacles = num_o;
	//num_edges = 0;
	//num_nodes_prm = n;

	
	/*
	for (int i = 0; i < 20; i++)
	{
		start.push_back({ 19.f * cos(i * M_PI / 10),19.f * sin(i * M_PI / 10) });
		goal.push_back({ 19.f * cos(M_PI + i * M_PI / 10),19.f * sin(M_PI + i * M_PI / 10) });
	}
	*/

	
	//start.push_back({ -19.5f, 19.5f });
	//goal.push_back({ 19.5f, -19.5f });
	/*
	start.push_back({ -19.5f, -19.5f });
	goal.push_back({ 19.5f, 19.5f });

	start.push_back({ 19.5f, -19.5f });
	goal.push_back({ -19.5f, 19.5f });

	start.push_back({ 19.5f, 19.5f });
	goal.push_back({ -19.5f, -19.5f });
	
	start.push_back({ 19.5f, 0 });
	goal.push_back({ -19.5f, 0 });

	start.push_back({ -19.5f, 0 });
	goal.push_back({ 19.5f, 0 });

	start.push_back({ 0, 19.5f });
	goal.push_back({  0, -19.5f });

	start.push_back({ 0, -19.5f });
	goal.push_back({ 0, 19.5f });
	*/

	
	o_pos.push_back(glm::vec2(0.f, 0.f));
	o_radius.push_back(5.f);
	
	o_pos.push_back(glm::vec2(-14.f, -4.f));
	o_pos.push_back(glm::vec2(6.f, 6.f));
	o_pos.push_back(glm::vec2(12.f, 10.f));
	o_pos.push_back(glm::vec2(14.f, -10.f));
	o_pos.push_back(glm::vec2(-7.f, 12.f));
	
	o_radius.push_back(4.f);
	o_radius.push_back(4.f);
	o_radius.push_back(3.f);
	o_radius.push_back(3.f);
	o_radius.push_back(2.f);

	o_locked.push_back(false);
	o_locked.push_back(false);
	o_locked.push_back(false);
	o_locked.push_back(false);
	o_locked.push_back(false);
	o_locked.push_back(false);

	/*
	for (int i = 0; i < numEnemy; i++)
	{
		pos.push_back(glm::vec2(start[i][0], start[i][1]));
		vel.push_back(glm::vec2(0.f, 0.f));
	}
	*/
	
	numEnemy = 0;
	numTower = 0;
	numObstacle = o_radius.size();

	econ_system_ = new EconSystem();
	
}

void SPath::init()
{
	roadmap_ = new Roadmap(o_pos, o_radius);
	roadmap_->PRM();
	n_edges_ = roadmap_->num_edges_;
	roadmap_->searchPath(1.f);
	path_size_ = roadmap_->Path.size();
	vertices_ = roadmap_->vertices;
}

void SPath::March()
{
	enemys_.push_back(new Enemy(path_size_));
	numEnemy = enemys_.size();
}

void SPath::DeleteAll()
{
	numEnemy = 0;
	enemys_.clear();

}

void SPath::buildTower(int picked,int type)
{
	if (BUILD_MOD)
	{
		if (numObstacle == 0)
		{
			std::cout << " \n No obstacles, can't build tower. \n";
			return;
		}

		if (econ_system_->global_net > econ_system_->tower1_cost)
		{
			glm::vec3 t_pos = glm::vec3(o_pos[picked].x, 4.9f, o_pos[picked].y);

			Tower* new_tower = new Tower(type, t_pos);
			numTower++;
			towers_.push_back(new_tower);

			econ_system_->global_net -= econ_system_->tower1_cost;
			std::cout << " \n Tower constructed, good choice!!! \n";
		}
		else
		{
			std::cout << " \n No money, earn more!!!!! \n";
		}
	}
	else
	{
		return;
	}

}

void SPath::buildObstacle()
{
	if (BUILD_MOD)
	{
		if (econ_system_->global_net > econ_system_->obstacle_cost)
		{
			//OBSTACLE_NUMBER++;
			numObstacle++;
			//PICKED = OBSTACLE_NUMBER - 1;
			o_pos.push_back(glm::vec2(0.f, 0.f));
			o_radius.push_back(2.f);
			o_locked.push_back(false);
			PICKED = numObstacle - 1;
		}
		else
		{
			std::cout << " \n No money, earn more!!!!! \n";
		}
	}
	else
	{
		return;
	}
	
}

void SPath::largeObstacle()
{
	if (BUILD_MOD && !o_locked[PICKED])
	{
		o_radius[PICKED] += 0.2;
		if (o_radius[PICKED] > 4) o_radius[PICKED] = 4;
	}
}

void SPath::shrinkObstacle()
{
	if (BUILD_MOD && !o_locked[PICKED])
	{
		o_radius[PICKED] -= 0.2;
		if (o_radius[PICKED] < 0.5) o_radius[PICKED] = 0.5;
	}
}

void SPath::nextObstacle()
{
	if (BUILD_MOD)
	{
		PICKED++;
		if (PICKED > numObstacle-1)
		{
			PICKED = numObstacle - 1;
		}
	}
}

void SPath::lastObstacle()
{
	if (BUILD_MOD)
	{
		PICKED--;
		if (PICKED < 0)
		{
			PICKED = 0;
		}
	}
}

void SPath::moveObstacle(int type)
{
	if (BUILD_MOD && !o_locked[PICKED])
	{
		if (type == 1)
		{
			o_pos[PICKED].y -= 0.3;
		}
		else if (type == 2)
		{
			o_pos[PICKED].x -= 0.3;
		}
		else if (type == 3)
		{
			o_pos[PICKED].x += 0.3;
		}
		else if (type == 4)
		{
			o_pos[PICKED].y += 0.3;
		}
	}
}

void SPath::obsLock()
{
	if (BUILD_MOD)
	{
		Roadmap new_roadmap(o_pos, o_radius);
		new_roadmap.PRM();
		new_roadmap.searchPath(1.f);
		if (!new_roadmap.canReachGoal())
		{
			std::cout << "\n Can not build obstacle, you cheat by blocking the way!!!!! \n";
		}
		else
		{
			o_locked[PICKED] = true;
		}
		
	}	
}

void SPath::deleteObstacle()
{
	if (BUILD_MOD)
	{
		o_pos.erase(o_pos.begin() + PICKED);
		o_radius.erase(o_radius.begin() + PICKED);
		o_locked.erase(o_locked.begin() + PICKED);
		numObstacle--;
		PICKED = numObstacle - 1;
	}
}

void SPath::upgradeTower()
{
	if (BUILD_MOD)
	{
		towers_[0]->upgrade();
	}
}
void SPath::update(float dt)
{

	for (int i = numEnemy - 1; i >= 0; i--)
	{

		if (enemys_[i]->isDead)
		{
			float timer2 = SDL_GetTicks();
			if (timer2 - enemys_[i]->timer1 > 1000)
			{
				numEnemy--;
				enemys_.erase(enemys_.begin() + i);
			}
			continue;
		}

		if ( enemys_[i]->health <= 0)
		{
			enemys_[i]->isDead = true;
			Mix_PlayChannel(-1, explosion, 0);
			econ_system_->global_net += econ_system_->enemy_destroy_earn;
			enemys_[i]->timer1 = SDL_GetTicks();
			continue;
		}

		if (enemys_[i]->eCurrent == 0)
		{
			Mix_PlayChannel(-1, ohno, 0);
			numEnemy--;
			enemys_.erase(enemys_.begin() + i);
			continue;
		}

		// path smoothing
		/*
		while (true)
		{			
			if (enemys_[i]->eCurrent < 2) break;
			if (!roadmap_->isEdgeAvaliavle(enemys_[i]->ePos.x, enemys_[i]->ePos.y, roadmap_->nodes[roadmap_->Path[enemys_[i]->eCurrent - 1]].x, roadmap_->nodes[roadmap_->Path[enemys_[i]->eCurrent - 1]].y)) break;
			enemys_[i]->eCurrent -= 1;
		}
		*/
		// goal force
		glm::vec2 goal_force(0);
		//glm::vec2 current_vec = glm::vec2(nodes[paths[i][current[i]]][0], nodes[paths[i][current[i]]][1]);
		glm::vec2 next_vec;
		next_vec = roadmap_->nodes[roadmap_->Path[enemys_[i]->eCurrent - 1]];

		glm::vec2 forward_dir = glm::normalize(next_vec - enemys_[i]->ePos);

		goal_force = goal_cof *  ((forward_dir * goal_speed) - enemys_[i]->eVel) ;

		// seperation
		glm::vec2 sep_force(0);
		/*
		for (int j = 0; j < numEnemy; j++)
		{
			if (i == j)
			{
				continue;
			}
			glm::vec2 relative = pos[i] - pos[j];
			float dist = glm::length(relative) - 2 * agent_radius;
			if (dist < sep_r)
			{
				glm::vec2 j_force = sep_cof / dist * glm::normalize(relative);
				if (std::isnan(j_force.x) || std::isnan(j_force.y)) continue;
				sep_force += j_force;
			}
		}
		*/
		for (int j = 0; j < o_pos.size(); j++)
		{
			glm::vec2 v = enemys_[i]->ePos - o_pos[j];
			float dist = glm::length(v) - (agent_radius + o_radius[j]);
			if (dist < sep_r)
			{
				glm::vec2 j_force = sep_cof / dist * glm::normalize(v);
				if (std::isnan(j_force.x) || std::isnan(j_force.y)) continue;
				sep_force += j_force;
			}
		}
		
		
		glm::vec2 total_force = goal_force + sep_force; //+ coh_force + alig_force;

		enemys_[i]->eVel += dt * total_force;

		glm::vec2 norvel = glm::normalize(enemys_[i]->eVel);

		if (glm::length(enemys_[i]->eVel) > max_speed)
		{
			enemys_[i]->eVel = max_speed * norvel;
		}

		if (!enemys_[i]->locked)
		{
			float coszeta, zeta;
			coszeta = glm::dot(norvel, glm::normalize(enemys_[i]->dir));
			zeta = acos(coszeta);

			enemys_[i]->rotation = zeta;

			enemys_[i]->locked = true;
		}
		

		enemys_[i]->ePos += dt * enemys_[i]->eVel;


		if (sqrt(pow(enemys_[i]->ePos.x - next_vec.x, 2) + pow(enemys_[i]->ePos.y - next_vec.y, 2)) < 0.07)
		{
			enemys_[i]->ePos = next_vec;
			enemys_[i]->eCurrent -= 1;
			enemys_[i]->locked = false;
		}

		// Fix collisions
		for (int j = 0; j < numEnemy; j++) 
		{
			if (i == j) continue;
			if (glm::length(enemys_[i]->ePos - enemys_[j]->ePos) < 2 * agent_radius)
			{
				enemys_[i]->ePos = enemys_[j]->ePos + 2 * agent_radius * glm::normalize(enemys_[i]->ePos - enemys_[j]->ePos);
			}
		}
	}

	for (int t = 0; t < numTower; t++)
	{
		towers_[t]->update(dt);
		if (towers_[t]->hasTarget())
		{
			continue;
		}

		for (int e = 0; e < numEnemy; e++)
		{
			float dx, dy;
			dx = enemys_[e]->ePos.x - towers_[t]->tower_pos.x;
			dy = enemys_[e]->ePos.y - towers_[t]->tower_pos.z;
			if (!enemys_[e]->isDead && dx * dx + dy * dy < towers_[t]->radius_ )
			{
				towers_[t]->set_target(enemys_[e]);
				break;
			}
		}
	}

}


