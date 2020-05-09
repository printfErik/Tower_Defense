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
	/*
	for (int i = 0; i < numEnemy; i++)
	{
		pos.push_back(glm::vec2(start[i][0], start[i][1]));
		vel.push_back(glm::vec2(0.f, 0.f));
	}
	*/
	
	numEnemy = 0;
	
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

void SPath::update(float dt)
{

	for (int i = 0; i < numEnemy; i++)
	{

		if (enemys_[i]->eCurrent == 0)
		{
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
		/*
		// cohesion
		glm::vec2 coh_force(0);
		glm::vec2 center(0);
		int count = 0;
		for (int j = 0; j < numEnemy; j++)
		{
			if (i == j) continue;
			if (glm::length(pos[i] - pos[j]) < coh_r) {
				count++;
				center += pos[j];
			}
		}
		if (count)
		{
			center /= (float)count;
			coh_force = coh_cof * (center - pos[i]);
		}

		// alignment
		glm::vec2 alig_force(0);
		glm::vec2 avg_vel(0);
		count = 0;
		for (int j = 0; j < numEnemy; j++)
		{
			if (i == j) continue;
			if (glm::length(pos[i] - pos[j]) < alig_r)
			{
				count++;
				avg_vel += vel[j];
			}
		}
		if (count)
		{
			alig_force = alig_cof * avg_vel / (float)count;
		}
		*/
		
		glm::vec2 total_force = goal_force + sep_force; //+ coh_force + alig_force;

		enemys_[i]->eVel += dt * total_force;

		if (glm::length(enemys_[i]->eVel) > max_speed)
		{
			enemys_[i]->eVel = max_speed * glm::normalize(enemys_[i]->eVel);
		}

		enemys_[i]->ePos += dt * enemys_[i]->eVel;


		if (sqrt(pow(enemys_[i]->ePos.x - next_vec.x, 2) + pow(enemys_[i]->ePos.y - next_vec.y, 2)) < 0.05)
		{
			enemys_[i]->ePos = next_vec;
			enemys_[i]->eCurrent -= 1;
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

}


