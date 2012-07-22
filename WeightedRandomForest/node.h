/*
 * node.h
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#ifndef NODE_H_
#define NODE_H_
#include"utility.h"

class Node{
private:

	NodeType type_;
	int case_num_;   //added for result file
	int id_;   //added for result file
	int level_; //the level which the node in
	/*
	 *the following is added for BFS
	 */
	Node* parent_;
	Node* right_sibling_;
	vector<int> attribute_list_;
	vector<int> training_set_index_;
public:
	static int node_num_;
	Node();
	void set_type_(NodeType);
	NodeType GetNodeType();
	void set_case_num_(int case_num);
	int get_case_num_();
	int get_id_();
	/*
	 * the following is added for BFS
	 */
	void set_level_(int level);
	int get_level_();
	void set_parent_(Node* parent);
	Node* get_parent_();
	void set_right_sibling_(Node* right_sibling);
	Node* get_right_sibling_();
	void set_attribute_list_(vector<int> attribute_list);
	vector<int> get_attribute_list_();
	void set_training_set_index_(vector<int> training_set_index);
	vector<int> get_training_set_index();

};

#endif
