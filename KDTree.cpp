#include "KDTree.h"

Node* KDTree::insertRec(Node* root,glm::vec2 new_data, unsigned depth, Node* father,bool is_x , int index)
{
	 
	if (root == NULL)
		return new_node(new_data,father,is_x,index);

	
	unsigned cd = depth % 2;

	
	if (cd)
	{
		if (new_data.y < (root->data.y))
			root->left = insertRec(root->left, new_data, depth + 1, root,!is_x,index);
		else
			root->right = insertRec(root->right, new_data, depth + 1, root, !is_x,index);
	}
	else
	{
		if (new_data.x < (root->data.x))
			root->left = insertRec(root->left, new_data, depth + 1,root, !is_x,index);
		else
			root->right = insertRec(root->right, new_data, depth + 1,root, !is_x, index);
	}
	

	return root;
}

Node* KDTree::insert(Node* root, glm::vec2 new_data,int index)
{
	return insertRec(root, new_data, 0, NULL,true,index);
}

float KDTree::node_distance(glm::vec2 first, Node* second)
{
	float d1 = (first.x - second->data.x);
	float d2 = (first.y - second->data.y);

	float ans = sqrt(d1 * d1 + d2 * d2);
	return ans;
}

float KDTree::hyper_distance(glm::vec2 first, Node* second)
{
	float ans;
	if (second->is_x) ans = abs(first.x - second->data.x);
	else ans = abs(first.y - second->data.y);
	return ans;
}
int KDTree::nearest(Node* root, glm::vec2 to_find)
{
	if (root == NULL)
	{
		return -1;
	}
	float dist_best = std::numeric_limits<float>::infinity();
	Node* nd_best = search(root, to_find);
	std::deque<Node*> queue1 = { root };
	std::deque<Node*> queue2 = { nd_best };
	while (!queue1.empty())
	{
		Node* nd_root = queue1.back();
		queue1.pop_back();
		Node* nd_cur = queue2.back();
		queue2.pop_back();
		while (true)
		{
			float dist = node_distance(to_find, nd_cur);
			if (dist < dist_best)
			{
				dist_best = dist;
				nd_best = nd_cur;
			}
			if (nd_cur != nd_root)
			{
				Node* nd_bro = brother(nd_cur);
				if (nd_bro != NULL) {
					float dist_hyper = hyper_distance(to_find, nd_cur->father);
					if (dist > dist_hyper) {
						Node* _nd_best = search(nd_bro, to_find);
						queue1.push_back(nd_bro);
						queue2.push_back(_nd_best);
					}

				}
				nd_cur = nd_cur->father;
			}
			else { break; }
            
		}
	}
	return nd_best->index;
}


Node* KDTree::search(Node* root, glm::vec2 data)
{
	int depth = 0;
	while (root->left != NULL || root->right != NULL)
	{
		if (root->left == NULL)
		{
			root = root->right;
			depth++;
		}
		else if (root->right == NULL)
		{
			root = root->left;
			depth++;
		}
		else
		{
			if (!depth % 2)
			{
				if (root->data.x >= data.x) root = root->left;
				else root = root->right;
			}
			else
			{
				if (root->data.y >= data.y) root = root->left;
				else root = root->right;
			}
			depth++;
		}
	}
	return root;
}

Node* KDTree::brother(Node* node)
{
	Node* ans;
	if (node->father == NULL)
	{
		ans = NULL;
	}
	else
	{
		if (node->father->left == node)
		{
			ans = node->father->right;
		}
		else
		{
			ans = node->father->left;
		}
	}
	return ans;
}

