#include "Particle.h"
Particle::Particle(int n)
{
	srand(static_cast <unsigned> (time(0)));
	numP = n;
	acc = glm::vec3(0.f, 30.f, 0.f);
	genRate = 1000;
	for (int i = 0; i < numP; i++)
	{
		pos.push_back(randomYDisk(1.f, 0,5,0));
		vel.push_back(glm::vec3());
		life.push_back(random(1,5));
		size.push_back(1.f);
	}

}

void Particle::computePhy(float dt)
{
	for (int i = numP-1; i >= 0; i--)
	{
		life[i] -= dt;
		size[i] = life[i]/5.f ;
		pos[i] = pos[i] + (vel[i] * dt);
		vel[i] = vel[i] + (acc * dt);
		if (life[i] < 0)
		{
			pos.erase(pos.begin() + i);
			vel.erase(vel.begin() + i);
			life.erase(life.begin() + i);
			size.erase(size.begin() + i);
			numP -= 1;
		}
	}
	
	int toGen = genRate * dt;
	if (toGen + numP > maxP)
	{
		toGen = maxP - numP;
		
	}
	numP += toGen;
	for (int i = toGen - 1; i >= 0; i--)
	{
		pos.push_back(randomYDisk(1.f, 0, 5, 0));
		vel.push_back(glm::vec3());
		life.push_back(random(1, 5));
		size.push_back(1.f);
	}


}

glm::vec3 Particle::randomYDisk(float r, float ix, float iy, float iz)
{
	float radius = r * sqrt(random(0.f, 1.f));
	float theta = 2 * M_PI * random(0.f, 1.f);
	float x = radius * sin(theta) + ix; 
	float y = iy + 1* random(-1, 1);
	float z = radius * cos(theta) + iz;
	glm::vec3 ans(x,y,z);
	return ans;
}

float Particle::random(float l, float h)
{
	return l + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (h - l)));
}