/*
 * node.cpp
 *
 *  Created on: 31 Dec, 2011
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#include"node.h"

int Node::node_num_ = 0;

Node::Node(){
	Node::node_num_ ++;
	this->id_ = Node::node_num_;
}

void Node::set_type_(NodeType type){
	this->type_=type;
}

NodeType Node::GetNodeType(){
	return this->type_;
}

void Node::set_case_num_(int case_num){
	this->case_num_ = case_num;
}
int Node::get_case_num_(){
	return this->case_num_;
}
int Node::get_id_(){
	return this->id_;
}

void Node::set_level_(int level){
	this->level_ = level;
}
int Node::get_level_(){
	return this->level_;
}
void Node::set_parent_(Node* parent){
	this->parent_ = parent;
}

Node* Node::get_parent_(){
	return this->parent_;
}
void Node::set_right_sibling_(Node* right_sibling){
	this->right_sibling_ = right_sibling;
}
Node* Node::get_right_sibling_(){
	return this->right_sibling_;
}

void Node::set_attribute_list_(vector<int> attribute_list){
	this->attribute_list_ = attribute_list;
}
vector<int> Node::get_attribute_list_(){
	return this->attribute_list_;
}

void Node::set_training_set_index_(vector<int> training_set_index){
	this->training_set_index_ = training_set_index;
}
vector<int> Node::get_training_set_index(){
	return this->training_set_index_;
}
