/*
 * internal_node.cpp
 *
 *  Created on: 20 Feb, 2012
 *      Author: meng
 *       email: qinghan.meng@gmail.com
 */

#include"internal_node.h"

InternalNode::InternalNode(){
	this->set_type_(INTERNALNODE);
}

void InternalNode::set_attribute_(int attribute){
	this->attribute_=attribute;
}
int InternalNode::get_attribute_(){
	return this->attribute_;
}
void InternalNode::set_split_value(double split_value){
	this->split_value_=split_value;
}
double InternalNode::get_split_value(){
	return this->split_value_;
}
void  InternalNode::set_child_node_(int attribute,Node* node){
	this->child_node_.insert(map<int,Node*>::value_type(attribute,node));
}
Node* InternalNode::get_child_node_(int value){
	return this->child_node_[value];
}

map<int,Node*> InternalNode::get_all_child_nodes_(){
	return this->child_node_;
}

//NodeType InternalNode::GetNodeType(){
//	return INTERNALNODE;
//}

void InternalNode::set_info_gain_(double info_gain){
	this->info_gain_ = info_gain;
}
double InternalNode::get_info_gain(){
	return this->info_gain_;
}
