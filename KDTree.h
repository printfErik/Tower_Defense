#pragma once
#include<iostream>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <deque>

struct Node {
	glm::vec2 data;
	bool is_x;
	struct Node* left;
	struct Node* right;
	struct Node* father;
	int index;
	//struct Node
};

class KDTree
{
	Node* real_root;
	public:
	KDTree() {
		real_root = NULL;
	}

	struct Node* new_node(glm::vec2 new_data, Node* father, bool is_x, int index)
	{
		struct Node* temp = new Node;

		temp->index = index;
		temp->data = new_data;
		temp->father = father;
		temp->is_x = is_x;
		temp->left = temp->right = NULL;
		return temp;
	}



	Node* insertRec(Node* root, glm::vec2 data, unsigned depth,Node* father,bool is_x,int index);
	Node* insert(Node* root, glm::vec2 new_data, int index);
	Node* brother(Node* node);
	Node* search(Node* root, glm::vec2 data);
	int nearest(Node* root, glm::vec2 to_find);
	float node_distance(glm::vec2 first, Node* second);
	float hyper_distance(glm::vec2 first, Node* second);
};

