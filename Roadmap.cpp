#include "Roadmap.h"

Roadmap::Roadmap(std::vector<glm::vec2> o_pos, std::vector<float> o_radius)
{
	srand(time(NULL));
	o_pos_ = o_pos;
	o_radius_ = o_radius;
	
}
/*
Roadmap::~Roadmap()
{
	delete []vertices;
}
*/
void Roadmap::searchPath(float e)
{
	std::vector<bool> visited(nodes.size(), false);
	std::vector<int> parent(nodes.size(), -1);
	int expanded = 0;
	std::deque<int> open_set;

	float eplison = e;
	int s_index = 0;
	int g_index = 1;

	std::vector<float> f_value(nodes.size(), std::numeric_limits<float>::infinity());
	std::vector<float> g_value(nodes.size(), std::numeric_limits<float>::infinity());
	f_value[s_index] = 0.f + eplison * distance(s_index, g_index);
	g_value[s_index] = 0.f;

	visited[s_index] = true;
	open_set.push_front(s_index);

	while (open_set.size() > 0)
	{
		expanded++;
		int currentNode = open_set.front();
		for (int minium : open_set) {
			if (currentNode == minium)
				continue;
			if (f_value[minium] < f_value[currentNode])
				currentNode = minium;
		}

		if (currentNode == g_index)
		{
			//std::cout << "Goal found!\n";
			break;
		}
		open_set.erase(std::remove(open_set.begin(), open_set.end(), currentNode));
		visited[currentNode] = true;


		for (int i = 0; i < neighbors[currentNode].size(); i++)
		{
			int neighborNode = neighbors[currentNode][i];

			if (visited[neighborNode])
			{
				continue;
			}

			if (std::find(open_set.begin(), open_set.end(), neighborNode) == open_set.end()) {
				open_set.push_back(neighborNode);
			}

			float new_g = g_value[currentNode] + distance(neighborNode, currentNode);
			if (new_g < g_value[neighborNode])
			{
				g_value[neighborNode] = new_g;
				f_value[neighborNode] = new_g + eplison * distance(neighborNode, g_index);
				parent[neighborNode] = currentNode;
			}
		}
	}

	std::vector<int> final_path;
	//std::cout << "numExpanded = " << expanded << std::endl;
	//std::cout << "\nReverse path: ";
	final_path.push_back(g_index);
	int prevNode = parent[g_index];
	final_path.push_back(prevNode);
	//std::cout << g_index << " ";
	while (prevNode >= 0)
	{
		//std::cout << prevNode << " ";
		prevNode = parent[prevNode];
		final_path.push_back(prevNode);
	}
	//std::cout << "\n";
	//current[agent] = final_path.size() - 2;

	Path = final_path;
}

float Roadmap::distance(int index1, int index2)
{
	float xd = nodes[index1][0] - nodes[index2][0];
	float yd = nodes[index1][1] - nodes[index2][1];

	return pow(xd * xd + yd * yd, 0.5);
}

void Roadmap::PRM()
{
	num_edges_ = 0;
	float LO = -19.5f;
	float HI = 19.5f;
	float x, y;
	nodes.push_back(start_);
	nodes.push_back(goal_);

	for (int i = 0; i < num_nodes_; i++)
	{
		x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		//std::cout << x << " : " << y << std::endl;

		bool c = false;
		for (int j = 0; j < o_pos_.size(); j++)
		{
			float xd = x - o_pos_[j].x;
			float yd = y - o_pos_[j].y;
			if (xd * xd + yd * yd <= (o_radius_[j] + 0.5) * (o_radius_[j] + 0.5))
			{
				c = true;
				break;
			}
		}
		if (c) continue;
		nodes.push_back({ x,y });
	}

	neighbors.resize(nodes.size());

	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = i + 1; j < nodes.size(); j++)
		{
			float x1 = nodes[i][0];
			float y1 = nodes[i][1];
			float x2 = nodes[j][0];
			float y2 = nodes[j][1];
			float dis = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
			//std::cout << "Index "<< i<< "( x1: " << x1 << " y1: " << y1 << " ) Index " << j <<"( x2: " << x2 << "y2: " << y2 << " ) \n";
			//std::cout << "dis is :" << dis << "\n";
			if (dis < 600 && isEdgeAvaliavle(x1, y1, x2, y2))
			{
				neighbors[i].push_back(j);
				neighbors[j].push_back(i);
			}
		}
	}
	for (int i = 0; i < nodes.size(); i++)
	{
		num_edges_ += neighbors[i].size();
	}
	num_edges_ = num_edges_ / 2;
	computeEdges();

	vertices = new float[2 * 3 * num_edges_];
	for (int i = 0; i < 6 * num_edges_; i++)
	{
		vertices[i] = 0.2f;
	}

	updateVerts();
}

bool Roadmap::isEdgeAvaliavle(float x1, float y1, float x2, float y2)
{
	float A = y2 - y1;
	float B = x1 - x2;
	float C = x2 * y1 - x1 * y2;
	float d, angle1, angle2;
	for (int i = 0; i < o_pos_.size(); i++)
	{
		d = abs((A * o_pos_[i][0] + B * o_pos_[i][1] + C) / pow(A * A + B * B, 0.5f));
		if (d <= o_radius_[i] + enemy_rad_)
		{
			angle1 = (o_pos_[i][0] - x1) * (x2 - x1) + (o_pos_[i][1] - y1) * (y2 - y1);
			angle2 = (o_pos_[i][0] - x2) * (x1 - x2) + (o_pos_[i][1] - y2) * (y1 - y2);
			if (angle1 > 0 && angle2 > 0)
			{
				return false;
			}
			//std::cout << "Unavaliable.\n";
			//std::cout << "d is " << d << std::endl;
			//std::cout << "with obstacle :" << i << " which radius is "<< o_radius_[i]<<"\n";		
		}
	}
	//std::cout << "Avaliable.\n";
	return true;
}


void Roadmap::computeEdges()
{
	//std::cout << "Edges are: \n";
	for (int i = 0; i < neighbors.size(); i++)
	{
		for (int j = 0; j < neighbors[i].size(); j++)
		{
			std::vector<int> e = { i, neighbors[i][j] };
			//std::cout << "edge {" << i << " , " << neighbors[i][j] << " }\n";
			edges.push_back(e);
		}
	}
	//std::cout << "\n equal " << edges.size() << " ; " << num_edges <<"\n";
}

void Roadmap::updateVerts()
{
	for (int i = 0; i < num_edges_; i++)
	{
		int index = i * 6;
		vertices[index] = nodes[edges[i][0]][0];
		vertices[index + 2] = nodes[edges[i][0]][1];

		vertices[index + 3] = nodes[edges[i][1]][0];
		vertices[index + 5] = nodes[edges[i][1]][1];

		//std::cout << "\n Edges info :" << index <<":"<< vertices[index] << ", " <<index +1 << ":" << vertices[index + 1] << ", " << index + 2 << ":" << vertices[index + 2] << ", " << index + 3 << ":" << vertices[index + 3] << ", " << index + 4 << ":" << vertices[index + 4] << ", " << index + 5 << ":" << vertices[index + 5] << " \n";
	}
}